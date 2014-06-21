#include <cppunittest.h>

#include "..\path\path.h"
#include "..\ini\ini.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(ini_test_class)
{
public:
	TEST_METHOD(ini_test)
	{
		std::string empty_utf8_string("");
		std::string en_utf8_string("Hello world");
		std::string hu_utf8_string("Árvíztűrő tükörfúrógép");
		std::string ja_utf8_string("こんにちは世界");
		std::string long_utf8_string("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse imperdiet bibendum scelerisque. Sed sollicitudin enim ac orci molestie, vel eleifend nisl pellentesque. Cras vitae malesuada quam. Sed nisl dui, pellentesque ut elit ut, feugiat sollicitudin neque. Pellentesque et justo sollicitudin, fringilla dolor non, laoreet lectus. Donec purus lorem, iaculis ut tellus at, fringilla pharetra tellus. Sed venenatis dictum fermentum. Pellentesque laoreet dignissim nisi eu cursus. Aenean sit amet sem eget elit dictum ornare. Cras nec felis volutpat, congue sapien vitae, dictum risus. Curabitur facilisis, turpis quis pellentesque placerat, nisi lorem euismod risus, nec iaculis mauris quam et nulla. Phasellus sit amet leo nec justo semper feugiat sit amet vitae quam. Nam auctor velit dui, ultrices ornare mi iaculis eu. Nunc tristique justo dictum luctus porta.");
		std::string string0("string0");
		std::string string1("string1");
		std::string string2("string2");

		std::string const & dir(my::path::get_current_dir());
		std::string const & path(my::path::combine(dir, "ini_test.ini"));
		my::ini::file const & file(my::ini::file(32, path));
		std::vector<my::ini::section const> const & sections(file.get_sections());
		Assert::IsTrue(sections.size() == 3);
		for (auto const & section : sections)
		{
			Assert::AreEqual(section.get_string("name").c_str(), section.get_name().c_str()); 
		}

		my::ini::section const & section(file.get_section("ini_test0"));
		Assert::AreEqual(empty_utf8_string, section.get_string("string_empty"));
		Assert::AreEqual(en_utf8_string, section.get_string("string_en"));
		Assert::AreEqual(hu_utf8_string, section.get_string("string_hu"));
		Assert::AreEqual(ja_utf8_string, section.get_string("string_ja"));
		Assert::AreEqual(long_utf8_string, section.get_string("string_long"));
		Assert::AreEqual(42, section.get_int("int_positive"));
		Assert::AreEqual(0, section.get_int("int_zero"));
		Assert::AreEqual(-42, section.get_int("int_negative"));
		Assert::AreEqual(false, section.get_bool("bool_false"));
		Assert::AreEqual(true, section.get_bool("bool_true"));

		std::vector<std::string const> const & strings(section.get_strings("string"));
		Assert::IsTrue(strings.size() == 3);
		Assert::AreEqual(string0, strings[0]);
		Assert::AreEqual(string1, strings[1]);
		Assert::AreEqual(string2, strings[2]);

		std::vector<std::string const> const & empty_strings(section.get_strings("empty_string"));
		Assert::IsTrue(empty_strings.size() == 0);
	}
};
