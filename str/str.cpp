#include <windows.h>

#include "str.h"

std::string my::str::narrow(std::wstring const & str)
{
	return my::str::narrow(str.c_str());
}

std::string my::str::narrow(std::wstring const * const str)
{
	return my::str::narrow(str->c_str());
}

std::string my::str::narrow(wchar_t const * const str)
{
	unsigned long const length(
		::WideCharToMultiByte(
		CP_UTF8,
		0,
		str,
		-1,
		nullptr,
		0,
		nullptr,
		nullptr));
	std::string result;
	result.resize(length - 1);
	::WideCharToMultiByte(
		CP_UTF8,
		0,
		str,
		-1,
		&result[0],
		length - 1,
		nullptr,
		nullptr);
	return result;
}

std::wstring my::str::widen(std::string const & str)
{
	return my::str::widen(str.c_str());
}

std::wstring my::str::widen(std::string const * const str)
{
	return my::str::widen(str->c_str());
}

std::wstring my::str::widen(char const * const str)
{
	unsigned long const length(
		::MultiByteToWideChar(
		CP_UTF8,
		0,
		str,
		-1,
		nullptr,
		0));
	std::wstring result;
	result.resize(length - 1);
	::MultiByteToWideChar(
		CP_UTF8,
		0,
		str,
		-1,
		&result[0],
		length - 1);
	return result;
}
