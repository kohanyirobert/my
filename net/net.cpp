#include <windows.h>
#include <wininet.h>

#include <fstream>
#include <sstream>

#include "..\str\str.h"

#include "net.h"

bool my::net::dl::get_session(::HINTERNET & session)
{
	bool result(false);
	if (::InternetAttemptConnect(0) == ERROR_SUCCESS)
	{
		session = ::InternetOpenW(
			nullptr,
			INTERNET_OPEN_TYPE_DIRECT,
			nullptr,
			nullptr,
			0);
		if (session != nullptr)
		{
			result = true;
		}
	}
	return result;
}

bool my::net::dl::get_stream(::HINTERNET const & session, std::string const & url, ::HINTERNET & stream)
{
	bool result(false);
	stream = ::InternetOpenUrlW(
		session,
		my::str::widen(url).c_str(),
		nullptr,
		0,
		INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD,
		0);
	if (stream != nullptr)
	{
		std::wstring buffer;
		buffer.resize(HTTP_QUERY_MAX);
		unsigned long read(buffer.size());
		if (::HttpQueryInfoW(
			stream,
			HTTP_QUERY_STATUS_CODE,
			&buffer[0],
			&read,
			0))
		{
			if (std::atoi(my::str::narrow(buffer).c_str()) == HTTP_STATUS_OK)
			{
				result = true;
			}
		}
	}
	return result;
}

bool my::net::dl::stream_to_out(unsigned long const & buffer_size, ::HINTERNET const & stream, std::ostream & out)
{
	bool result(false);
	if (buffer_size > 0)
	{
		std::string buffer;
		buffer.resize(buffer_size);
		unsigned long read;
		while (true)
		{
			if (::InternetReadFile(
				stream,
				&buffer[0],
				buffer_size,
				&read))
			{
				if (read > 0)
				{
					out.write(buffer.c_str(), read);
					continue;
				}
				else
				{
					result = true;
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	return result;
}

my::net::dl::dl(unsigned long const & buffer_size, std::ios::openmode const & out_mode)
	: buffer_size(buffer_size), out_mode(out_mode)
{
}

my::net::dl::~dl()
{
}

unsigned long const & my::net::dl::get_buffer_size() const
{
	return this->buffer_size;
}

std::ios::openmode const & my::net::dl::get_out_mode() const
{
	return this->out_mode;
}

bool my::net::dl::url_to_out(std::string const & url, std::ostream & out) const
{
	bool result(false);
	::HINTERNET session;
	if (my::net::dl::get_session(session))
	{
		::HINTERNET stream;
		if (my::net::dl::get_stream(session, url, stream))
		{
			if (my::net::dl::stream_to_out(this->get_buffer_size(), stream, out))
			{
				result = true;
			}
			::InternetCloseHandle(stream);
		}
		::InternetCloseHandle(session);
	}
	return result;
}

bool my::net::dl::url_to_str(std::string const & url, std::string & str) const
{
	std::ostringstream out;
	bool result(this->url_to_out(url, out));
	str = out.str();
	return result;
}

bool my::net::dl::url_to_file(std::string const & url, std::string const & file) const
{
	std::ofstream out(file, this->get_out_mode());
	bool result(this->url_to_out(url, out));
	out.close();
	return result;
}

bool my::net::dl::urls_to_file(std::vector<std::string const> const & urls, std::string const & file) const
{
	bool result(false);
	for (unsigned int i(0); i < urls.size(); ++i)
	{
		if (this->url_to_file(urls[i], file))
		{
			result = true;
			break;
		}
	}
	return result;
}
