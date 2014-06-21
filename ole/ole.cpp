#include <windows.h>
#include <ole2.h>

#include "ole.h"

bool my::ole::initialized;

bool my::ole::get_initialized()
{
	return my::ole::initialized;
}

bool my::ole::initialize()
{
	if (!my::ole::get_initialized() && SUCCEEDED(::OleInitialize(nullptr)))
	{
		my::ole::initialized = true;
		return true;
	}
	return false;
}

void my::ole::uninitialize()
{
	if (my::ole::get_initialized())
	{
		::OleUninitialize();
		my::ole::initialized = false;
	}
}
