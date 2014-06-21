#include <windows.h>
#include <wincrypt.h>

#include "..\io\io.h"
#include "..\str\str.h"
#include "..\misc\misc.h"

#include "crypt.h"

wchar_t const * const my::crypt::provider_name(my::misc::get_win_ver() == "win_xp"
	? MS_ENH_RSA_AES_PROV_XP
	: MS_ENH_RSA_AES_PROV);

::DWORD const my::crypt::provider_type(PROV_RSA_AES);
::DWORD const my::crypt::hash_algorithm(CALG_MD5);
::DWORD const my::crypt::key_algorithm(CALG_DES);
::DWORD const my::crypt::key_length(0x00380000);
::DWORD const my::crypt::padding_mode(PKCS5_PADDING);
::DWORD const my::crypt::cipher_mode(CRYPT_MODE_CBC);

my::crypt::crypt(my::io::ch const & channel)
	: channel(channel)
{
}

my::crypt::~crypt()
{
}

my::io::ch const & my::crypt::get_channel() const
{
	return this->channel;
}

bool my::crypt::encrypt_or_decrypt_str_to_str(
	bool const & encrypt,
	std::string const & password,
	std::string & plaintext,
	std::string & ciphertext)
{
	::BOOL ok(FALSE);

	::HCRYPTPROV context;
	ok = ::CryptAcquireContextW(
		&context,
		NULL,
		my::crypt::provider_name,
		my::crypt::provider_type,
		0);

	if (!ok)
	{
		if (my::misc::get_last_error_code() == NTE_BAD_KEYSET)
		{
			ok = ::CryptAcquireContextW(
				&context,
				NULL,
				my::crypt::provider_name,
				my::crypt::provider_type,
				CRYPT_NEWKEYSET);
			if (!ok)
			{
				goto RETURN;
			}
		}
		else
		{
			goto RETURN;
		}
	}

	::HCRYPTHASH hash;
	ok = ::CryptCreateHash(
		context,
		my::crypt::hash_algorithm,
		0,
		0,
		&hash);
	if (!ok)
	{
		goto RELEASE_CONTEXT;
	}

	::DWORD password_data_size(password.size());
	::BYTE * password_data(new ::BYTE[password_data_size]);
	std::memcpy(password_data, (::BYTE *)password.c_str(), password_data_size);
	ok = ::CryptHashData(
		hash,
		password_data,
		password_data_size,
		0);
	if (!ok)
	{
		goto DESTROY_PASSWORD_DATA;
	}

	::HCRYPTKEY key;
	ok = ::CryptDeriveKey(
		context,
		my::crypt::key_algorithm,
		hash,
		my::crypt::key_length | CRYPT_NO_SALT,
		&key);
	if (!ok)
	{
		goto DESTROY_HASH;
	}

	::DWORD block_length_size(sizeof(::DWORD));
	::DWORD block_length;
	ok = ::CryptGetKeyParam(
		key,
		KP_BLOCKLEN,
		(BYTE *)&block_length,
		&block_length_size,
		0);
	if (!ok)
	{
		goto DESTROY_KEY;
	}

	ok = ::CryptSetKeyParam(
		key,
		KP_MODE,
		(::BYTE *)&my::crypt::cipher_mode,
		0);
	if (!ok)
	{
		goto DESTROY_KEY;
	}

	ok = ::CryptSetKeyParam(
		key,
		KP_PADDING,
		(::BYTE *)&my::crypt::padding_mode,
		0);
	if (!ok)
	{
		goto DESTROY_KEY;
	}

	::DWORD iv_data_size(block_length / CHAR_BIT);
	::BYTE * iv_data(new ::BYTE[iv_data_size]);
	std::memset(iv_data, '\0', iv_data_size);
	ok = ::CryptSetKeyParam(
		key,
		KP_IV,
		(::BYTE *)iv_data,
		0);
	if (!ok)
	{
		goto DESTROY_IV_DATA;
	}

	if (encrypt)
	{
		::DWORD ciphertext_data_size(plaintext.size());
		::DWORD maximum_ciphertext_data_size(ciphertext_data_size + (block_length / CHAR_BIT));
		::BYTE * ciphertext_data(new ::BYTE[maximum_ciphertext_data_size]);
		std::memcpy(ciphertext_data, (::BYTE *)plaintext.c_str(), ciphertext_data_size);
		ok = ::CryptEncrypt(
			key,
			NULL,
			TRUE,
			0,
			ciphertext_data,
			&ciphertext_data_size,
			maximum_ciphertext_data_size);
		if (!ok)
		{
			goto DESTROY_CIPHERTEXT_DATA;
		}

		ciphertext.resize(ciphertext_data_size);
		std::memcpy((::BYTE *)ciphertext.c_str(), ciphertext_data, ciphertext_data_size);

DESTROY_CIPHERTEXT_DATA:
		delete[] ciphertext_data;
	}
	else
	{
		::DWORD plaintext_data_size(ciphertext.size());
		::BYTE * plaintext_data(new ::BYTE[plaintext_data_size]);
		std::memcpy(plaintext_data, (::BYTE *)ciphertext.c_str(), plaintext_data_size);
		ok = ::CryptDecrypt(
			key,
			NULL,
			TRUE,
			0,
			plaintext_data,
			&plaintext_data_size);
		if (!ok)
		{
			goto DESTROY_PLAINTEXT_DATA;
		}

		plaintext.resize(plaintext_data_size);
		std::memcpy((::BYTE *)plaintext.c_str(), plaintext_data, plaintext_data_size);

DESTROY_PLAINTEXT_DATA:
		delete[] plaintext_data;
	}

DESTROY_IV_DATA:
	delete[] iv_data;

DESTROY_KEY:
	::CryptDestroyKey(key);

DESTROY_HASH:
	::CryptDestroyHash(hash);

DESTROY_PASSWORD_DATA:
	delete[] password_data;

RELEASE_CONTEXT:
	::CryptReleaseContext(context, 0);

RETURN:
	return ok == TRUE;
}

bool my::crypt::encrypt_file(
	std::string const & password,
	std::string const & file) const
{
	return my::crypt::encrypt_file_to_file(password, file, file);
}

bool my::crypt::decrypt_file(
	std::string const & password,
	std::string const & file) const
{
	return my::crypt::decrypt_file_to_file(password, file, file);
}

bool my::crypt::encrypt_file_to_file(
	std::string const & password,
	std::string const & plaintext_file,
	std::string const & ciphertext_file) const
{
	bool result(false);
	std::string plaintext;
	if (this->get_channel().file_to_str(plaintext_file, plaintext))
	{
		std::string ciphertext;
		if (my::crypt::encrypt_str_to_str(password, plaintext, ciphertext))
		{
			if (this->get_channel().str_to_file(ciphertext, ciphertext_file))
			{
				result = true;
			}
		}
	}
	return result;
}

bool my::crypt::decrypt_file_to_file(
	std::string const & password,
	std::string const & ciphertext_file,
	std::string const & plaintext_file) const
{
	bool result(false);
	std::string ciphertext;
	if (this->get_channel().file_to_str(ciphertext_file, ciphertext))
	{
		std::string plaintext;
		if (my::crypt::decrypt_str_to_str(password, ciphertext, plaintext))
		{
			if (this->get_channel().str_to_file(plaintext, plaintext_file))
			{
				result = true;
			}
		}
	}
	return result;
}

bool my::crypt::encrypt_str_to_str(
	std::string const & password,
	std::string const & plaintext,
	std::string & ciphertext) const
{
	return my::crypt::encrypt_or_decrypt_str_to_str(true, password, const_cast<std::string &>(plaintext), ciphertext);
}

bool my::crypt::decrypt_str_to_str(
	std::string const & password,
	std::string const & ciphertext,
	std::string & plaintext) const
{
	return my::crypt::encrypt_or_decrypt_str_to_str(false, password, plaintext, const_cast<std::string &>(ciphertext));
}
