#pragma once

#include <string>

namespace my
{
	class proc final
	{
	public:
		static unsigned long get_id();

		static std::string get_file();
		static bool get_file(unsigned long const & id, std::string & path);
	};
}
