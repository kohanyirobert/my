#pragma once

#include <string>

#include "..\io\io.h"

namespace my
{
	namespace log
	{
		class file final
		{
		private:
			std::string const & path;
			bool const & enabled;
			unsigned char const & lvl;
			my::io::ch const & channel;

			file();

		public:
			file(std::string const & path, bool const & enabled, unsigned char const & lvl, my::io::ch const & channel);
			~file();

			std::string const & get_path() const;
			bool const & get_enabled() const;
			unsigned char const & get_lvl() const;
			my::io::ch const & get_channel() const;

			void log(unsigned char const & lvl, std::string const & str) const;
		};
	}
}
