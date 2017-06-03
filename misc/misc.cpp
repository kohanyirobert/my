#include <windows.h>

#include <algorithm>
#include <random>
#include <thread>

#include "..\str\str.h"

#include "misc.h"

std::string my::misc::get_win_ver()
{
	std::string version;
	::OSVERSIONINFOEX os_version_info;
	os_version_info.dwOSVersionInfoSize = sizeof(::OSVERSIONINFOEX);
	if (::GetVersionExW((::LPOSVERSIONINFOW) &os_version_info) == TRUE)
	{
		if (os_version_info.dwMajorVersion == 5 && os_version_info.dwMinorVersion == 0)
		{
			version = "win_2000";
		}
		else if (os_version_info.dwMajorVersion == 5 && os_version_info.dwMinorVersion == 1)
		{
			version = "win_xp";
		}
		else if (os_version_info.dwMajorVersion == 6 && os_version_info.dwMinorVersion == 0)
		{
			version = "win_vista";
		}
		else if (os_version_info.dwMajorVersion == 6 && os_version_info.dwMinorVersion == 1)
		{
			version = "win_7";
		}
		else if (os_version_info.dwMajorVersion == 6 && os_version_info.dwMinorVersion == 2)
		{
			version = "win_8";
		}
	}
	return version;
}

unsigned long my::misc::get_last_error_code()
{
	return ::GetLastError();
}

std::string my::misc::get_last_error()
{
	return my::misc::get_last_error(my::misc::get_last_error_code());
}

std::string my::misc::get_last_error(unsigned long const & last_error_code)
{
	std::string result;
	wchar_t * buffer;
	if (::FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		last_error_code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(wchar_t *)&buffer,
		0,
		nullptr) != 0)
	{
		result = my::str::narrow(buffer);
	}
	::LocalFree(buffer);
	return result;
}

unsigned long long my::misc::get_milliseconds_since_epoch()
{
	return my::misc::get_milliseconds_since_epoch(std::chrono::system_clock::now());
}

unsigned long long my::misc::get_milliseconds_since_epoch(std::chrono::system_clock::time_point const & time_point)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch()).count();
}

void my::misc::shuffle(std::vector<std::string> & vector)
{
	my::misc::shuffle(my::misc::get_milliseconds_since_epoch(), vector);
}

void my::misc::shuffle(std::chrono::system_clock::time_point const & time_point, std::vector<std::string> & vector)
{
	my::misc::shuffle(my::misc::get_milliseconds_since_epoch(time_point), vector);
}

void my::misc::shuffle(unsigned long long const & seed, std::vector<std::string> & vector)
{
	std::shuffle(vector.begin(), vector.end(), std::mt19937_64(seed));
}

void my::misc::sleep_minutes(unsigned long long const & minutes)
{
	std::this_thread::sleep_for(std::chrono::minutes(minutes));
}

void my::misc::sleep_seconds(unsigned long long const & seconds)
{
	std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void my::misc::sleep_milliseconds(unsigned long long const & milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
