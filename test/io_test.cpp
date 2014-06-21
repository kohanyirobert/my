#include <cppunittest.h>

#include "..\io\io.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(io_test_class)
{
private:
	void assert_file_to_str(my::io::ch const & channel, std::string const & file, std::string const & expected)
	{
		std::string actual;
		Assert::IsTrue(channel.file_to_str(file, actual));
		Assert::AreEqual(expected, actual);
		Assert::AreEqual(expected.c_str(), actual.c_str());
	}

public:
	TEST_METHOD(io_test)
	{
		my::io::ch ch_zero_buffer_size(0, std::ios::in, std::ios::out);
		my::io::ch ch_small_buffer_size(1, std::ios::in, std::ios::out);
		my::io::ch ch_big_buffer_size(4096, std::ios::in, std::ios::out);

		std::string en_utf8_string("Hello world");
		std::string hu_utf8_string("Árvíztűrő tükörfúrógép");
		std::string ja_utf8_string("こんにちは世界");

		std::string bad_txt("bad.txt");
		std::string en_utf8_txt("en_utf8.txt");
		std::string hu_utf8_txt("hu_utf8.txt");
		std::string ja_utf8_txt("ja_utf8.txt");
		std::string en_plaintext_utf8_txt("en_plaintext_utf8.txt");
		std::string hu_plaintext_utf8_txt("hu_plaintext_utf8.txt");
		std::string ja_plaintext_utf8_txt("ja_plaintext_utf8.txt");

		std::string str;
		Assert::IsFalse(ch_zero_buffer_size.file_to_str(bad_txt, str));
		Assert::IsFalse(ch_small_buffer_size.file_to_str(bad_txt, str));
		Assert::IsFalse(ch_big_buffer_size.file_to_str(bad_txt, str));

		Assert::IsFalse(ch_zero_buffer_size.file_to_str(en_utf8_txt, str));

		assert_file_to_str(ch_small_buffer_size, en_utf8_txt, en_utf8_string);
		assert_file_to_str(ch_small_buffer_size, hu_utf8_txt, hu_utf8_string);
		assert_file_to_str(ch_small_buffer_size, ja_utf8_txt, ja_utf8_string);

		assert_file_to_str(ch_big_buffer_size, en_utf8_txt, en_utf8_string);
		assert_file_to_str(ch_big_buffer_size, hu_utf8_txt, hu_utf8_string);
		assert_file_to_str(ch_big_buffer_size, ja_utf8_txt, ja_utf8_string);

		Assert::IsTrue(ch_small_buffer_size.file_to_str(en_plaintext_utf8_txt, str));
		Assert::IsTrue(ch_small_buffer_size.file_to_str(hu_plaintext_utf8_txt, str));
		Assert::IsTrue(ch_small_buffer_size.file_to_str(ja_plaintext_utf8_txt, str));
	}
};
