#pragma once

#include <string>
#include <ostream>
#include <vector>

namespace my
{
	namespace net
	{
		class dl final
		{
		private:
			static bool get_session(void * & session);
			static bool get_stream(void * const & session, std::string const & url, void * & stream);
			static bool stream_to_out(unsigned long const & buffer_size, void * const & stream, std::ostream & out);

			unsigned long const & buffer_size;
			std::ios::openmode const & out_mode;

			dl();

		public:
			dl(unsigned long const & buffer_size, std::ios::openmode const & out_mode);
			~dl();

			unsigned long const & get_buffer_size() const;
			std::ios::openmode const & get_out_mode() const;

			bool url_to_out(std::string const & url, std::ostream & out) const;
			bool url_to_str(std::string const & url, std::string & str) const;
			bool url_to_file(std::string const & url, std::string const & file) const;
			bool urls_to_file(std::vector<std::string const> const & urls, std::string const & file) const;
		};
	}
}

