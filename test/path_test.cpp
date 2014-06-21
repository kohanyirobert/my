#pragma comment(lib, "shlwapi.lib")

#include <cppunittest.h>

#include "..\path\path.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(path_test_class)
{
public:
	TEST_METHOD(path_test)
	{
		std::string const & parent_children("parent_dir\\children_file.txt");
		Assert::AreEqual(parent_children, my::path::combine("parent_dir", "children_file.txt"));
		Assert::AreEqual(parent_children, my::path::combine("parent_dir\\", "children_file.txt"));
		Assert::AreNotEqual(parent_children, my::path::combine("parent_dir", "\\children_file.txt"));
		Assert::AreNotEqual(parent_children, my::path::combine("parent_dir\\", "\\children_file.txt"));

		Assert::IsTrue("parent_dir" == my::path::dirname(parent_children));
		Assert::IsTrue("children_file.txt" == my::path::basename(parent_children));
		Assert::IsTrue("parent_dir\\children_file.ini" == my::path::change_ext(parent_children, "ini"));
		Assert::IsTrue("parent_dir\\children_file" == my::path::remove_ext(parent_children));

		std::string const & rel_dir("path_test");
		Assert::IsTrue(my::path::is_file("test.dll"));
		Assert::IsTrue(my::path::is_dir("c:\\windows"));
		Assert::IsTrue(my::path::is_file("c:\\windows\\notepad.exe"));
		if (my::path::is_dir(rel_dir))
		{
			Assert::IsTrue(my::path::delete_dir(rel_dir));
		}
		Assert::IsTrue(my::path::create_dir(rel_dir));
		Assert::IsTrue(my::path::is_empty_dir(rel_dir));

		std::string const & current_dir(my::path::get_current_dir());
		std::string const & abs_dir(my::path::combine(current_dir, rel_dir));
		Assert::IsTrue(current_dir.size() > 0);
		Assert::IsTrue(my::path::set_current_dir(abs_dir));
		Assert::AreEqual(abs_dir, my::path::get_current_dir());
		Assert::IsTrue(my::path::set_current_dir(current_dir));

		std::string const & local_temp_file(my::path::get_temp_file(rel_dir, "pat"));
		Assert::IsTrue(local_temp_file.size() > 0);
		Assert::IsTrue(my::path::is_file(local_temp_file));
		Assert::IsFalse(my::path::is_empty_dir(rel_dir));
		Assert::IsTrue(my::path::delete_file(local_temp_file));
		Assert::IsTrue(my::path::is_empty_dir(rel_dir));
		Assert::IsTrue(my::path::delete_dir(rel_dir));

		std::string const & temp_dir(my::path::get_temp_dir());
		Assert::IsTrue(temp_dir.size() > 0);
		Assert::IsTrue(my::path::is_dir(temp_dir));

		std::string const & global_temp_file(my::path::get_temp_file(temp_dir, "pat"));
		Assert::IsTrue(global_temp_file.size() > 0);
		Assert::IsTrue(my::path::is_file(global_temp_file));
		Assert::IsTrue(my::path::delete_file(global_temp_file));

		std::string data_dir;
		Assert::IsTrue(my::path::get_data_dir(data_dir));
		Assert::IsTrue(data_dir.size() > 0);
		Assert::IsTrue(my::path::is_dir(data_dir));
	}
};
