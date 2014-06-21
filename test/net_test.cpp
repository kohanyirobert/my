#pragma comment(lib, "wininet.lib")

#include <cppunittest.h>

#include "..\net\net.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(net_test_class)
{
private:
	void assert_url_to_str(my::net::dl const & dl, std::string const & url, std::string const & expected)
	{
		std::string actual;
		Assert::IsTrue(dl.url_to_str(url, actual));
		Assert::AreEqual(expected, actual);
		Assert::AreEqual(expected.c_str(), actual.c_str());
	}

public:
	TEST_METHOD(net_test)
	{
		std::ios::openmode out_mode(std::ios::out | std::ios::binary);
		my::net::dl dl_zero_buffer_size(0, out_mode);
		my::net::dl dl_small_buffer_size(1, out_mode);
		my::net::dl dl_big_buffer_size(4906, out_mode);

		std::string en_utf8_string("Hello world");
		std::string hu_utf8_string("Árvíztűrő tükörfúrógép");
		std::string ja_utf8_string("こんにちは世界");

		std::string bad_url("http://127.0.0.1:8000/bad.txt");
		std::string en_utf8_url("http://127.0.0.1:8000/en_utf8.txt");
		std::string hu_utf8_url("http://127.0.0.1:8000/hu_utf8.txt");
		std::string ja_utf8_url("http://127.0.0.1:8000/ja_utf8.txt");

		std::string str;
		Assert::IsFalse(dl_zero_buffer_size.url_to_str(bad_url, str));
		Assert::IsFalse(dl_small_buffer_size.url_to_str(bad_url, str));
		Assert::IsFalse(dl_big_buffer_size.url_to_str(bad_url, str));

		Assert::IsFalse(dl_zero_buffer_size.url_to_str(en_utf8_url, str));

		assert_url_to_str(dl_small_buffer_size, en_utf8_url, en_utf8_string);
		assert_url_to_str(dl_small_buffer_size, hu_utf8_url, hu_utf8_string);
		assert_url_to_str(dl_small_buffer_size, ja_utf8_url, ja_utf8_string);

		assert_url_to_str(dl_big_buffer_size, en_utf8_url, en_utf8_string);
		assert_url_to_str(dl_big_buffer_size, hu_utf8_url, hu_utf8_string);
		assert_url_to_str(dl_big_buffer_size, ja_utf8_url, ja_utf8_string);
	}
};
