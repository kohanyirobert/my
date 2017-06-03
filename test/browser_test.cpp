#include <cppunittest.h>

#include "..\ole\ole.h"
#include "..\path\path.h"
#include "..\ini\ini.h"
#include "..\browser\browser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(browser_test_class)
{
public:
	TEST_METHOD(browser_test)
	{
		std::string const & dir(my::path::get_current_dir());
		std::string const & path(my::path::combine(dir, "browser_test.ini"));
		my::ini::file file(my::ini::file(path, 32));
		my::ini::section section(file.get_section("browser_test"));
		std::string js(section.get_string("js"));

		Assert::IsTrue(my::ole::initialize());

		my::browser::ie browser;

		for (int i(0); i < 3; ++i)
		{
			Assert::IsFalse(browser.get_opened());
			browser.close();
			Assert::IsFalse(browser.get_opened());
			Assert::IsTrue(browser.open());
			Assert::IsTrue(browser.get_opened());
			Assert::IsTrue(browser.set_visible(true));

			browser.set_header("Referer", "http://redtube.com");
			browser.navigate("http://google.com", 10, 10);

			Assert::IsTrue(browser.execute(js));

			std::string const & id("browser_test");
			std::string inner_text;
			Assert::IsTrue(browser.get_inner_text(id, inner_text));
			Assert::AreEqual(id, inner_text);

			Assert::IsFalse(browser.open());
			Assert::IsTrue(browser.get_opened());
			browser.close();
			Assert::IsFalse(browser.get_opened());
		}

		my::ole::uninitialize();
	}
};
