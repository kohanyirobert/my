#include <windows.h>
#include <psapi.h>

#include "..\str\str.h"
#include "..\path\path.h"

#include "proc.h"

unsigned long my::proc::get_id()
{
	return ::GetCurrentProcessId();
}

std::string my::proc::get_file()
{
	std::wstring buffer;
	buffer.resize(MAX_PATH);
	::GetModuleFileNameW(
		nullptr,
		&buffer[0],
		buffer.size());
	return my::str::narrow(buffer);
}

bool my::proc::get_file(unsigned long const & id, std::string & path)
{
	bool result(false);
	::HANDLE process = ::OpenProcess(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE,
		id);
	if (process == nullptr)
	{
		goto RETURN;
	}
	{
		std::wstring buffer;
		buffer.resize(MAX_PATH);
		if (::GetModuleFileNameEx(
			process,
			NULL,
			&buffer[0],
			buffer.size()) == 0)
		{
			goto CLOSE_HANDLE;
		}
		path = my::str::narrow(buffer);
		result = true;
	}
CLOSE_HANDLE:
	::CloseHandle(process);
RETURN:
	return result;
}