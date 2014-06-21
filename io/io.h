#pragma once

#include <string>
#include <istream>
#include <ostream>

namespace my
{
	namespace io
	{
		class ch final
		{
		private:
			unsigned long const & buffer_size;
			std::ios::openmode const & in_mode;
			std::ios::openmode const & out_mode;

			ch();

		public:
			ch(unsigned long const & buffer_size, std::ios::openmode const & in_mode, std::ios::openmode const & out_mode);
			~ch();

			unsigned long const & get_buffer_size() const;
			std::ios::openmode const & get_in_mode() const;
			std::ios::openmode const & get_out_mode() const;

			bool in_to_out(std::istream & in, std::ostream & out) const;
			bool in_to_str(std::istream & in, std::string & str) const;
			bool str_to_out(std::string const & str, std::ostream & out) const;
			bool file_to_str(std::string const & file, std::string & str) const;
			bool str_to_file(std::string const & str, std::string const & file) const;
			bool file_to_file(std::string const & file1, std::string const & file2) const;
		};
	}
}
