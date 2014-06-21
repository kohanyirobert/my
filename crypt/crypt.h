#pragma once

#include <string>

#include "..\io\io.h"

namespace my
{
	class crypt final
	{
	private:
		static wchar_t const * const provider_name;
		static unsigned long const provider_type;
		static unsigned long const hash_algorithm;
		static unsigned long const key_length;
		static unsigned long const key_algorithm;
		static unsigned long const padding_mode;
		static unsigned long const cipher_mode;

		my::io::ch const & channel;

		static bool encrypt_or_decrypt_str_to_str(
			bool const & encrypt,
			std::string const & password,
			std::string & plaintext,
			std::string & ciphertext);

		crypt();

	public:
		crypt(my::io::ch const & channel);
		~crypt();

		my::io::ch const & get_channel() const;

		bool encrypt_file(
			std::string const & password,
			std::string const & file) const;

		bool decrypt_file(
			std::string const & password,
			std::string const & file) const;

		bool encrypt_file_to_file(
			std::string const & password,
			std::string const & plaintext_file,
			std::string const & ciphertext_file) const;

		bool decrypt_file_to_file(
			std::string const & password,
			std::string const & ciphertext_file,
			std::string const & plaintext_file) const;

		bool encrypt_str_to_str(
			std::string const & password,
			std::string const & plaintext,
			std::string & ciphertext) const;

		bool decrypt_str_to_str(
			std::string const & password,
			std::string const & ciphertext,
			std::string & plaintext) const;
	};
}
