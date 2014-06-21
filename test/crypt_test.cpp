#include <cppunittest.h>

#include <vector>

#include "..\crypt\crypt.h"
#include "..\io\io.h"
#include "..\path\path.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(crypt_test_class)
{
private:
	void assert_aes(my::crypt const & crypt, std::string const & password, std::string const & plaintext_file)
	{
		std::string plaintext;
		Assert::IsTrue(crypt.get_channel().file_to_str(plaintext_file, plaintext));

		std::string ciphertext;
		Assert::IsTrue(crypt.encrypt_str_to_str(password, plaintext, ciphertext));

		std::string decipheredtext;
		Assert::IsTrue(crypt.decrypt_str_to_str(password, ciphertext, decipheredtext));

		Assert::AreEqual(plaintext, decipheredtext);
		Assert::AreEqual(plaintext.c_str(), decipheredtext.c_str());
	}

public:
	TEST_METHOD(crypt_test)
	{
		my::crypt const & crypt(my::io::ch(32, std::ios::in | std::ios::binary, std::ios::out | std::ios::binary));

		std::vector<std::string> passwords;
		passwords.push_back("");
		passwords.push_back("hello world");
		passwords.push_back("árvíztűrő tükörfúrógép");
		passwords.push_back("こんにちは世界");
		passwords.push_back("\0");
		passwords.push_back("hello\0world");
		passwords.push_back("árvíztűrő\0tükörfúrógép");
		passwords.push_back("こんにち\0は\0世界");

		std::vector<std::string> plaintexts;
		plaintexts.push_back("en_plaintext_utf8.txt");
		plaintexts.push_back("hu_plaintext_utf8.txt");
		plaintexts.push_back("ja_plaintext_utf8.txt");

		std::vector<std::string> ciphertexts;
		ciphertexts.push_back("en_plaintext_utf8.txt");
		ciphertexts.push_back("hu_plaintext_utf8.txt");
		ciphertexts.push_back("ja_plaintext_utf8.txt");

		for (unsigned int i(0); i < passwords.size(); ++i)
		{
			for (unsigned int j(0); j < plaintexts.size(); ++j)
			{
				assert_aes(crypt, passwords[i], plaintexts[j]);
			}
		}

		std::string tmp_plaintext_and_ciphertext(my::path::get_temp_file("tmp"));
		Assert::IsTrue(crypt.get_channel().file_to_file("en_plaintext_utf8.txt", tmp_plaintext_and_ciphertext));
		Assert::IsTrue(crypt.encrypt_file("password", tmp_plaintext_and_ciphertext));
		Assert::IsFalse(crypt.decrypt_file("", tmp_plaintext_and_ciphertext));
		Assert::IsTrue(crypt.decrypt_file("password", tmp_plaintext_and_ciphertext));
		Assert::IsTrue(my::path::delete_file(tmp_plaintext_and_ciphertext));
	}
};
