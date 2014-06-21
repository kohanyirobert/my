#pragma once

#include <string>

namespace my
{
	class str final
	{
	public:
		static std::string narrow(std::wstring const & str);
		static std::string narrow(std::wstring const * const str);
		static std::string narrow(wchar_t const * const str);

		static std::wstring widen(std::string const & str);
		static std::wstring widen(std::string const * const str);
		static std::wstring widen(char const * const str);
	};
}
