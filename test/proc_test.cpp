#pragma comment(lib, "psapi.lib")

#include <cppunittest.h>
#include <algorithm>

#include "..\proc\proc.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(proc_test_class)
{
private:
	void to_lower_case(std::string & str)
	{
		// https://stackoverflow.com/a/313990
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	}

public:
	TEST_METHOD(proc_test)
	{
		std::string & path1(my::proc::get_file());
		Assert::IsTrue(path1.size() > 0);
		std::string _;
		Assert::IsFalse(my::proc::get_file(0, _));
		unsigned long const & id(my::proc::get_id());
		Assert::IsTrue(id > 0);
		std::string path2;
		Assert::IsTrue(my::proc::get_file(id, path2));
		to_lower_case(path1);
		to_lower_case(path2);
		Assert::AreEqual(path1, path2);
	}
};
