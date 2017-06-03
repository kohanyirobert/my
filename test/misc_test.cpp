#include <cppunittest.h>

#include "..\misc\misc.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(misc_test_class)
{
private:
	std::vector<std::string> new_vector()
	{
		std::vector<std::string> vector;
		vector.push_back("a");
		vector.push_back("b");
		vector.push_back("c");
		vector.push_back("d");
		vector.push_back("e");
		return vector;
	}

	void assert_vectors(
		std::vector<std::string> const & vector_a,
		std::vector<std::string> const & vector_b,
		bool const & expected)
	{
		Assert::AreEqual(
			expected,
			vector_a[0] == vector_b[0]
		&& vector_a[1] == vector_b[1]
		&& vector_a[2] == vector_b[2]
		&& vector_a[3] == vector_b[3]
		&& vector_a[4] == vector_b[4]);
	}

public:
	TEST_METHOD(misc_test)
	{
		std::chrono::system_clock::time_point time_point_a(std::chrono::system_clock::now());

		my::misc::sleep_milliseconds(1);

		std::chrono::system_clock::time_point time_point_b(std::chrono::system_clock::now());

		for (unsigned int i(0); i < 3; ++i)
		{
			std::vector<std::string> vector_a(new_vector());
			std::vector<std::string> vector_b(new_vector());

			my::misc::shuffle(time_point_a, vector_a);
			my::misc::shuffle(time_point_a, vector_b);

			assert_vectors(vector_a, vector_b, true);
		}

		for (unsigned int i(0); i < 3; ++i)
		{
			std::vector<std::string> vector_a(new_vector());
			std::vector<std::string> vector_b(new_vector());

			my::misc::shuffle(time_point_a, vector_a);
			my::misc::shuffle(time_point_b, vector_b);

			assert_vectors(vector_a, vector_b, false);
		}
	}
};
