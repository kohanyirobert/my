#pragma once

#include <string>
#include <vector>
#include <chrono>

namespace my
{
	class misc final
	{
	public:
		static std::string get_win_ver();

		static unsigned long get_last_error_code();
		static std::string get_last_error();
		static std::string get_last_error(unsigned long const & last_error_code);

		static unsigned long long get_milliseconds_since_epoch();
		static unsigned long long get_milliseconds_since_epoch(std::chrono::system_clock::time_point const & time_point);

		static void shuffle(std::vector<std::string const> & vector);
		static void shuffle(std::chrono::system_clock::time_point const & time_point, std::vector<std::string const> & vector);
		static void shuffle(unsigned long long const & seed, std::vector<std::string const> & vector);

		static void sleep_minutes(unsigned long long const & minutes);
		static void sleep_seconds(unsigned long long const & seconds);
		static void sleep_milliseconds(unsigned long long const & milliseconds);
	};
}
