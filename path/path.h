#pragma once

#include <string>

namespace my
{
	class path final
	{
	private:
		static bool get_special_dir(int const & id, std::string & path);

	public:
		static bool delete_file(std::string const & path);
		static bool delete_dir(std::string const & path);
		static bool create_dir(std::string const & path);

		static bool is_file(std::string const & path);
		static bool is_dir(std::string const & path);
		static bool is_empty_dir(std::string const & path);

		static std::string remove_ext(std::string const & path);
		static std::string change_ext(std::string const & path, std::string const & ext);

		static std::string basename(std::string const & path);
		static std::string dirname(std::string const & path);
		static std::string combine(std::string const & path1, std::string const & path2);

		static std::string get_current_dir();
		static bool set_current_dir(std::string const & path);

		static std::string get_temp_dir();
		static std::string get_temp_file(std::string const & prefix);
		static std::string get_temp_file(std::string const & path, std::string const & prefix);

		static bool get_data_dir(std::string & path);
	};
}
