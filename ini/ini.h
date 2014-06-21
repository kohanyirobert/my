#pragma once

#include <string>
#include <vector>
#include <map>

namespace my
{
	namespace ini
	{
		class file;
		class section;
	}
}

namespace my
{
	namespace ini
	{
		class file final
		{
		private:
			unsigned long const & buffer_size;
			std::string const & path;
			std::map<std::string const, nullptr_t const> mutable names;

			file();

		public:
			file(unsigned long const & buffer_size, std::string const & path);
			~file();

			unsigned long const & get_buffer_size() const;
			std::string const & get_path() const;
			std::vector<my::ini::section const> const get_sections() const;
			my::ini::section const get_section(std::string const & name) const;
		};

		class section final
		{
		private:
			my::ini::file const & file;
			std::string const & name;

			section();

		public:
			section(my::ini::file const & file, std::string const & name);
			~section();

			my::ini::file const & get_file() const;
			std::string const & get_name() const;

			bool get_bool(std::string const & key) const;
			int get_int(std::string const & key) const;
			std::vector<std::string const> get_strings(std::string const & key) const;
			std::string get_string(std::string const & key) const;
		};
	}
}
