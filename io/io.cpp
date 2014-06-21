#include <fstream>
#include <sstream>

#include "..\str\str.h"

#include "io.h"

my::io::ch::ch(unsigned long const & buffer_size, std::ios::openmode const & in_mode, std::ios::openmode const & out_mode)
	: buffer_size(buffer_size), in_mode(in_mode), out_mode(out_mode)
{
}

my::io::ch::~ch()
{
}

unsigned long const & my::io::ch::get_buffer_size() const
{
	return this->buffer_size;
}

std::ios::openmode const & my::io::ch::get_in_mode() const
{
	return this->in_mode;
}

std::ios::openmode const & my::io::ch::get_out_mode() const
{
	return this->out_mode;
}

bool my::io::ch::in_to_out(std::istream & in, std::ostream & out) const
{
	bool result(false);
	if (this->get_buffer_size() > 0)
	{
		std::string buffer;
		buffer.resize(this->get_buffer_size());
		while(true)
		{
			in.read(&buffer[0], this->get_buffer_size());
			if (in.gcount() > 0) {
				out.write(buffer.c_str(), in.gcount());
			}
			if (in.eof())
			{
				result = true;
				break;
			}
			else if (in.bad() || in.fail())
			{
				break;
			}
			else
			{
				continue;
			}
		}
	}
	return result;
}

bool my::io::ch::in_to_str(std::istream & in, std::string & str) const
{
	std::ostringstream out(this->get_out_mode());
	bool result(this->in_to_out(in, out));
	str = out.str();
	return result;
}

bool my::io::ch::str_to_out(std::string const & str, std::ostream & out) const
{
	std::istringstream in(str, this->get_in_mode());
	return this->in_to_out(in, out);
}

bool my::io::ch::file_to_str(std::string const & file, std::string & str) const
{
	std::ifstream in(my::str::widen(file), this->get_in_mode());
	bool result(this->in_to_str(in, str));
	in.close();
	return result;
}

bool my::io::ch::str_to_file(std::string const & str, std::string const & file) const
{
	std::ofstream out(my::str::widen(file), this->get_out_mode());
	bool result(this->str_to_out(str, out));
	out.close();
	return result;
}

bool my::io::ch::file_to_file(std::string const & file1, std::string const & file2) const
{
	std::ifstream in(my::str::widen(file1), this->get_in_mode());
	std::ofstream out(my::str::widen(file2), this->get_out_mode());
	bool result(this->in_to_out(in, out));
	out.close();
	in.close();
	return result;
}
