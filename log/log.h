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
			my::io::ch const & channel;
			std::string const & path;
			bool const & enabled;

			file();

		public:
			file(std::string const & path, bool const & enabled, my::io::ch const & channel);
			~file();

			my::io::ch const & get_channel() const;
			std::string const & get_path() const;
			bool const & get_enabled() const;

			void log(std::string const & str) const;
		};
	}
}
