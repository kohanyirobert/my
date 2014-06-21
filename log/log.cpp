#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>

#include "..\str\str.h"

#include "log.h"

my::log::file::file(std::string const & path, bool const & enabled, my::io::ch const & channel)
	: path(path), enabled(enabled), channel(channel)
{
}

my::log::file::~file()
{
}

my::io::ch const & my::log::file::get_channel() const
{
	return this->channel;
}

std::string const & my::log::file::get_path() const
{
	return this->path;
}

bool const & my::log::file::get_enabled() const
{
	return this->enabled;
}

void my::log::file::log(std::string const & str) const
{
	if (this->get_enabled())
	{
		std::time_t const time(std::time(nullptr));
		std::tm tm;
		if (::gmtime_s(&tm, &time) == 0)
		{
			unsigned long const buffer_size(21);
			std::string buffer;
			buffer.resize(buffer_size);
			std::strftime(&buffer[0], buffer.size(), "%Y-%m-%dT%H:%M:%SZ", &tm);
			buffer.erase(buffer.find_last_of('\0'));
			std::stringstream in(buffer);
			in << " - " << str << std::endl;
			this->get_channel().str_to_file(in.str(), this->get_path());
		}
	}
}
