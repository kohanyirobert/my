#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>

#include "..\str\str.h"

#include "path.h"

bool my::path::get_special_dir(int const & id, std::string & path)
{
	bool result(false);
	std::wstring buffer;
	buffer.resize(MAX_PATH);
	if (SUCCEEDED(
		::SHGetFolderPathW(
		nullptr,
		id,
		nullptr,
		SHGFP_TYPE_CURRENT,
		&buffer[0])))
	{
		path = my::str::narrow(buffer);
		result = true;
	}
	return result;
}

bool my::path::delete_file(std::string const & path)
{
	return ::DeleteFileW(my::str::widen(path).c_str()) == TRUE;
}

bool my::path::delete_dir(std::string const & path)
{
	return ::RemoveDirectoryW(my::str::widen(path).c_str()) == TRUE;
}

bool my::path::create_dir(std::string const & path)
{
	return ::CreateDirectoryW(my::str::widen(path).c_str(), nullptr) == TRUE;
}

bool my::path::is_file(std::string const & path)
{
	return ::PathFileExistsW(my::str::widen(path).c_str()) == TRUE;
}

bool my::path::is_dir(std::string const & path)
{
	return ::PathIsDirectoryW(my::str::widen(path).c_str()) == FILE_ATTRIBUTE_DIRECTORY;
}

bool my::path::is_empty_dir(std::string const & path)
{
	return ::PathIsDirectoryEmptyW(my::str::widen(path).c_str()) == TRUE;
}

std::string my::path::combine(std::string const & path1, std::string const & path2)
{
	std::wstring buffer;
	buffer.resize(MAX_PATH);
	::PathCombineW(
		&buffer[0],
		my::str::widen(path1).c_str(),
		my::str::widen(path2).c_str());
	return my::str::narrow(buffer);
}

std::string my::path::remove_ext(std::string const & path)
{
	std::wstring buffer(my::str::widen(path));
	::PathRemoveExtensionW(&buffer[0]);
	return my::str::narrow(buffer);
}

std::string my::path::change_ext(std::string const & path, std::string const & ext)
{
	return my::path::remove_ext(path) + "." + ext;
}

std::string my::path::basename(std::string const & path)
{
	return my::str::narrow(PathFindFileNameW(my::str::widen(path).c_str()));
}

std::string my::path::dirname(std::string const & path)
{
	std::wstring buffer(my::str::widen(path));
	::PathRemoveFileSpecW(&buffer[0]);
	return my::str::narrow(buffer);
}

std::string my::path::get_current_dir()
{
	std::wstring buffer;
	buffer.resize(MAX_PATH);
	::GetCurrentDirectoryW(buffer.size(), &buffer[0]);
	return my::str::narrow(buffer);
}

bool my::path::set_current_dir(std::string const & path)
{
	return ::SetCurrentDirectoryW(my::str::widen(path).c_str()) == TRUE;
}

std::string my::path::get_temp_dir()
{
	std::wstring buffer;
	buffer.resize(MAX_PATH);
	::GetTempPathW(buffer.size(), &buffer[0]);
	return my::str::narrow(buffer);
}

std::string my::path::get_temp_file(std::string const & prefix)
{
	return my::path::get_temp_file(my::path::get_temp_dir(), prefix);
}

std::string my::path::get_temp_file(std::string const & path, std::string const & prefix)
{
	std::wstring buffer;
	buffer.resize(MAX_PATH);
	::GetTempFileNameW(
		my::str::widen(path).c_str(),
		my::str::widen(prefix).c_str(),
		0,
		&buffer[0]);
	return my::str::narrow(buffer);
}

bool my::path::get_data_dir(std::string & path)
{
	bool result(false);
	if (my::path::get_special_dir(CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, path)
		|| my::path::get_special_dir(CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, path)
		|| my::path::get_special_dir(CSIDL_APPDATA | CSIDL_FLAG_CREATE, path)
		|| my::path::get_special_dir(CSIDL_PROFILE | CSIDL_FLAG_CREATE, path))
	{
		result = true;
	}
	else
	{
		path = my::path::get_temp_dir();
		result = true;
	}
	return result;
}
