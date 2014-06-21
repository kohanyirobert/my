#include <windows.h>

#include <sstream>
#include <algorithm>
#include <iterator>

#include "..\str\str.h"

#include "ini.h"

my::ini::file::file(std::string const & path, unsigned long const & buffer_size)
	: path(path), buffer_size(buffer_size), names(std::map<std::string const, nullptr_t const>())
{
}

my::ini::file::~file()
{
}

std::string const & my::ini::file::get_path() const
{
	return this->path;
}

unsigned long const & my::ini::file::get_buffer_size() const
{
	return this->buffer_size;
}

std::vector<my::ini::section const> const my::ini::file::get_sections() const
{
	std::wstring buffer;
	buffer.resize(this->get_buffer_size());
	while (::GetPrivateProfileSectionNamesW(
		&buffer[0],
		buffer.size(),
		str::widen(this->get_path()).c_str()) + 2 >= buffer.size())
	{
		buffer.resize(buffer.size() + this->get_buffer_size());
	}
	this->names.clear();
	wchar_t * wname(&buffer[0]);
	while (*wname != '\0')
	{
		std::string name(my::str::narrow(wname));
		this->names.insert(std::pair<std::string const, nullptr_t const>(name, nullptr));
		wname += std::wstring(wname).size() + 1;
		if (*wname == '\0')
		{
			break;
		}
	}
	std::vector<my::ini::section const> sections;
	for(auto const & name : names)
	{
		sections.push_back(my::ini::section(*this, name.first));
	}
	return sections;
}

my::ini::section const my::ini::file::get_section(std::string const & name) const
{
	this->get_sections();
	return my::ini::section(*this, this->names.find(name)->first);
}

my::ini::section::section(my::ini::file const & file, std::string const & name)
	: file(file), name(name)
{
}

my::ini::section::~section()
{
}

my::ini::file const & my::ini::section::get_file() const
{
	return this->file;
}

std::string const & my::ini::section::get_name() const
{
	return this->name;
}

bool my::ini::section::get_bool(std::string const & key) const
{
	return this->get_int(key) == 1;
}

int my::ini::section::get_int(std::string const & key) const
{
	return ::GetPrivateProfileIntW(
		my::str::widen(this->get_name()).c_str(),
		my::str::widen(key).c_str(),
		0,
		my::str::widen(this->get_file().get_path()).c_str());
}

std::string my::ini::section::get_string(std::string const & key) const
{
	std::wstring string;
	string.resize(this->get_file().get_buffer_size());
	unsigned long actual_size(0);
	while (true)
	{
		actual_size = ::GetPrivateProfileStringW(
			my::str::widen(this->get_name()).c_str(),
			my::str::widen(key).c_str(),
			nullptr,
			&string[0],
			string.size(),
			my::str::widen(this->get_file().get_path()).c_str());
		if (actual_size == (string.size() - 1))
		{
			string.resize(string.size() * 2);
			continue;
		}
		else if (actual_size < string.size())
		{
			break;
		}
	}
	return my::str::narrow(string);
}

std::vector<std::string const> my::ini::section::get_strings(std::string const & key) const
{
	std::vector<std::string const> strings;
	unsigned int index(0);
	while (true)
	{
		std::string const string = this->get_string(key + std::to_string(index));
		if (string.empty())
		{
			break;
		}
		else
		{
			strings.push_back(string);
			++index;
			continue;
		}
	}
	return strings;
}
