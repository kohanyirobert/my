#pragma once

#include <string>
#include <map>

namespace my
{
	namespace browser
	{
		class ie final
		{
		private:
			std::map<std::string const, std::string const> mutable headers;
			unsigned long mutable cookie;
			void mutable * browser;
			bool mutable opened;

		public:
			ie();
			~ie();

			std::map<std::string const, std::string const> const get_headers() const;
			void set_headers(std::map<std::string const, std::string const> const & headers) const;

			std::string const get_header(std::string const & name) const;
			void set_header(std::string const & name, std::string const & value) const;

			bool const & get_opened() const;

			bool open() const;
			void close() const;

			bool get_visible(bool & visible) const;
			bool set_visible(bool const & visible) const;

			bool navigate(
				std::string const & url,
				unsigned long const & wait_milliseconds_between_busy,
				unsigned long const & wait_milliseconds_between_ready) const;

			bool execute(std::string const & js) const;

			bool get_inner_text(std::string const & id, std::string & inner_text) const;
		};
	}
}
