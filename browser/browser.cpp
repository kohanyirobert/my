#include <windows.h>
#include <mshtml.h>
#include <exdisp.h>

#include <sstream>
#include <thread>
#include <chrono>

#include "..\str\str.h"

#include "browser.h"

/*
struct dispatch final : ::IDispatch
{
::ULONG STDMETHODCALLTYPE AddRef()
{
return 0;
}

::HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID riid, _Out_ void ** ppvObject)
{
if (riid == ::IID_IUnknown) {
*ppvObject = (::IUnknown *)this;
return S_OK;
} else if (riid == ::IID_IDispatch) {
*ppvObject = (::IDispatch *)this;
return S_OK;
}
*ppvObject = nullptr;
return E_NOINTERFACE;
}

::ULONG STDMETHODCALLTYPE Release()
{
return 0;
}

::HRESULT STDMETHODCALLTYPE GetIDsOfNames(
_In_ REFIID riid,
_In_ ::LPOLESTR * rgszNames,
_In_ ::UINT cNames,
_In_ ::LCID lcid,
_Out_ ::DISPID * rgDispId)
{
return S_OK;
}

::HRESULT STDMETHODCALLTYPE GetTypeInfo(_In_ ::UINT iTInfo, _In_ ::LCID lcid, _Out_ ::ITypeInfo ** ppTInfo)
{
return S_OK;
}

::HRESULT STDMETHODCALLTYPE GetTypeInfoCount(_Out_ ::UINT * pctinfo)
{
return S_OK;
}

::HRESULT STDMETHODCALLTYPE Invoke(
_In_ ::DISPID dispIdMember,
_In_ REFIID riid,
_In_ ::LCID lcid,
_In_ ::WORD wFlags,
_Inout_ ::DISPPARAMS * pDispParams,
_Out_ ::VARIANT * pVarResult,
_Out_ ::EXCEPINFO * pExcepInfo,
_Out_ ::UINT * puArgErr)
{
return S_OK;
}
} dispatch;
*/

my::browser::ie::ie()
	: opened(false), cookie(0), browser(nullptr)
{
}

my::browser::ie::~ie()
{
}

std::map<std::string const, std::string const> const my::browser::ie::get_headers() const
{
	return this->headers;
}

void my::browser::ie::set_headers(std::map<std::string const, std::string const> const & headers) const
{
	this->headers = headers;
}

std::string const my::browser::ie::get_header(std::string const & name) const
{
	return this->headers[name];
}

void my::browser::ie::set_header(std::string const & name, std::string const & value) const
{
	this->headers.insert(std::pair<std::string const, std::string const>(name, value));
}

bool const & my::browser::ie::get_opened() const
{
	return this->opened;
}

bool my::browser::ie::open() const
{
	::IWebBrowser2 * browser(static_cast<::IWebBrowser2 *>(this->browser));
	bool result(false);

	if (this->get_opened())
	{
		goto RETURN;
	}

	if (FAILED(
		::CoCreateInstance(
		::CLSID_InternetExplorer,
		nullptr,
		::CLSCTX_LOCAL_SERVER,
		::IID_IWebBrowser2,
		(void **)&browser)))
	{
		goto RETURN;
	}

	if (FAILED(browser->QueryInterface(::IID_IWebBrowser2, (void **)&browser)))
	{
		browser->Release();
		goto RETURN;
	}

	/*
	::IConnectionPointContainer * connection_point_container;
	if (FAILED(browser->QueryInterface(::IID_IConnectionPointContainer, (void **)&connection_point_container)))
	{
	browser->Quit();
	browser->Release();
	goto RETURN;
	}

	::IConnectionPoint * connection_point;
	if (FAILED(connection_point_container->FindConnectionPoint(::DIID_DWebBrowserEvents2, &connection_point)))
	{
	goto RELEASE_CONNECTION_POINT_CONTAINER;
	}

	if (FAILED(connection_point->Advise(&dispatch, &this->cookie)))
	{
	goto RELEASE_CONNECTION_POINT;
	}
	*/

	this->opened = true;
	this->browser = browser;
	result = true;

	/*
	RELEASE_CONNECTION_POINT:
	connection_point->Release();

	RELEASE_CONNECTION_POINT_CONTAINER:
	connection_point_container->Release();
	*/

RETURN:
	return result;
}

void my::browser::ie::close() const
{
	::IWebBrowser2 * browser(static_cast<::IWebBrowser2 *>(this->browser));

	if (!this->get_opened())
	{
		goto RETURN;
	}

	/*
	::IConnectionPointContainer * connection_point_container;
	if (FAILED(browser->QueryInterface(::IID_IConnectionPointContainer, (void **)&connection_point_container)))
	{
	goto QUIT_AND_RELEASE_BROWSER;
	}

	::IConnectionPoint * connection_point;
	if (FAILED(connection_point_container->FindConnectionPoint(::DIID_DWebBrowserEvents2, &connection_point)))
	{
	goto RELEASE_CONNECTION_POINT_CONTAINER;
	}

	if (FAILED(connection_point->Unadvise(this->cookie)))
	{
	goto RELEASE_CONNECTION_POINT;
	}
	*/

	this->opened = false;

	/*
	RELEASE_CONNECTION_POINT:
	connection_point->Release();

	RELEASE_CONNECTION_POINT_CONTAINER:
	connection_point_container->Release();
	*/

	//QUIT_AND_RELEASE_BROWSER:
	browser->Quit();
	browser->Release();

RETURN:
	return;
}

bool my::browser::ie::get_visible(bool & visible) const
{
	::IWebBrowser2 * browser(static_cast<::IWebBrowser2 *>(this->browser));
	bool result(false);
	::VARIANT_BOOL visible_variant;
	if (SUCCEEDED(browser->get_Visible(&visible_variant)))
	{
		visible = visible_variant == VARIANT_TRUE;
		result = true;
	}
	return result;
}

bool my::browser::ie::set_visible(bool const & visible) const
{
	::IWebBrowser2 * browser(static_cast<::IWebBrowser2 *>(this->browser));
	bool result(false);
	::VARIANT_BOOL visible_variant(visible ? VARIANT_TRUE : VARIANT_FALSE);
	if (SUCCEEDED(browser->put_Visible(visible_variant)))
	{
		result = true;
	}
	return result;
}

bool my::browser::ie::navigate(
	std::string const & url,
	unsigned long const & wait_milliseconds_between_busy,
	unsigned long const & wait_milliseconds_between_ready) const
{
	::IWebBrowser2 * browser(static_cast<::IWebBrowser2 *>(this->browser));
	bool result(false);

	::VARIANT empty_variant;
	::VariantInit(&empty_variant);

	::VARIANT headers_variant;
	::VariantInit(&headers_variant);
	if (this->headers.size() > 0)
	{
		std::stringstream headers;
		for (std::map<std::string const, std::string const>::iterator it(this->headers.begin()); it != this->headers.end(); ++it)
		{
			headers << it->first << ": " << it->second << std::endl;
		}

		::VariantInit(&headers_variant);
		headers_variant.vt = ::VT_BSTR;
		headers_variant.bstrVal = ::SysAllocString(my::str::widen(headers.str()).c_str());
	}

	::BSTR url_bstr(::SysAllocString(my::str::widen(url).c_str()));
	if (FAILED(
		browser->Navigate(
		url_bstr,
		&empty_variant,
		&empty_variant,
		&empty_variant,
		&headers_variant)))
	{
		goto RELEASE_NAVIGATE_VARIABLES_AND_RETURN;
	}

	::VARIANT_BOOL busy_variant;
	while (true)
	{
		if (FAILED(browser->get_Busy(&busy_variant)))
		{
			goto RELEASE_NAVIGATE_VARIABLES_AND_RETURN;
		}

		if (busy_variant == VARIANT_TRUE)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_between_busy));
			continue;
		}
		else 
		{
			break;
		}
	}

	::READYSTATE readystate;
	while (true)
	{
		if (FAILED(browser->get_ReadyState(&readystate)))
		{
			goto RELEASE_NAVIGATE_VARIABLES_AND_RETURN;
		}

		if (readystate == ::READYSTATE_COMPLETE)
		{
			break;
		} else 
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_between_ready));
			continue;
		}
	}

	::IHTMLDocument2 * document;
	if (FAILED(browser->get_Document((::IDispatch **)&document)))
	{
		goto RELEASE_NAVIGATE_VARIABLES_AND_RETURN;
	}

	if (FAILED(document->QueryInterface(::IID_IHTMLDocument2, (void **)&document)))
	{
		goto RELEASE_DOCUMENT;
	}

	::BSTR complete_readystate_bstr(::SysAllocString(L"complete"));
	::BSTR readystate_bstr;
	while (true)
	{
		if (FAILED(document->get_readyState(&readystate_bstr)))
		{
			goto RELEASE_GET_READYSTATE_VARIABLES;
		}

		if (::VarBstrCmp(
			complete_readystate_bstr,
			readystate_bstr,
			LOCALE_INVARIANT,
			NORM_IGNORECASE) == VARCMP_EQ)
		{
			break;
		}
		else 
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}
	}

	result = true;

RELEASE_GET_READYSTATE_VARIABLES:
	::SysFreeString(readystate_bstr);
	::SysFreeString(complete_readystate_bstr);

RELEASE_DOCUMENT:
	document->Release();

RELEASE_NAVIGATE_VARIABLES_AND_RETURN:
	::SysFreeString(url_bstr);
	::VariantClear(&headers_variant);
	::VariantClear(&empty_variant);
	return result;
}

bool my::browser::ie::execute(std::string const & js) const
{
	::IWebBrowser2 * browser(static_cast<::IWebBrowser2 *>(this->browser));
	bool result(false);

	::IHTMLDocument2 * document;
	if (FAILED(browser->get_Document((IDispatch **)&document)))
	{
		goto RETURN;
	}

	if (FAILED(document->QueryInterface(::IID_IHTMLDocument2, (void **)&document)))
	{
		goto RELEASE_DOCUMENT;
	}

	::IHTMLWindow2 * window;
	if (FAILED(document->get_parentWindow((::IHTMLWindow2 **)&window)))
	{
		goto RELEASE_DOCUMENT;
	}

	if (FAILED(window->QueryInterface(::IID_IHTMLWindow2, (void **)&window)))
	{
		goto RELEASE_WINDOW;
	}

	::VARIANT return_variant;
	::VariantInit(&return_variant);
	::BSTR code_bstr(::SysAllocString(my::str::widen(js).c_str()));
	::BSTR language_bstr(::SysAllocString(L"JavaScript"));
	if (FAILED(window->execScript(code_bstr, language_bstr, &return_variant)))
	{
		goto RELEASE_EXEC_SCRIPT_VARIABLES;
	}

	result = true;

RELEASE_EXEC_SCRIPT_VARIABLES:
	::SysFreeString(language_bstr);
	::SysFreeString(code_bstr);
	::VariantClear(&return_variant);

RELEASE_WINDOW:
	window->Release();

RELEASE_DOCUMENT:
	document->Release();

RETURN:
	return result;
}

bool my::browser::ie::get_inner_text(std::string const & id, std::string & inner_text) const
{
	::IWebBrowser2 * browser(static_cast<::IWebBrowser2 *>(this->browser));
	bool result(false);

	::IHTMLDocument2 * document;
	if (FAILED(browser->get_Document((IDispatch **)&document)))
	{
		goto RETURN;
	}

	if (FAILED(document->QueryInterface(::IID_IHTMLDocument2, (void **)&document)))
	{
		goto RELEASE_DOCUMENT;
	}

	::IHTMLElement * body;
	if (FAILED(document->get_body(&body)))
	{
		goto RELEASE_DOCUMENT;
	}

	if (FAILED(body->QueryInterface(::IID_IHTMLElement, (void **)&body)))
	{
		goto RELEASE_BODY;
	}

	::IHTMLElementCollection * children;
	if (FAILED(body->get_children((::IDispatch **)&children)))
	{
		goto RELEASE_BODY;
	}

	if (FAILED(children->QueryInterface(::IID_IHTMLElementCollection, (void **)&children)))
	{
		goto RELEASE_CHILDREN;
	}

	long length;
	if (FAILED(children->get_length(&length)))
	{
		goto RELEASE_CHILDREN;
	}

	::VARIANT name_variant;
	::VariantInit(&name_variant);
	name_variant.vt = VT_BSTR;
	name_variant.bstrVal = ::SysAllocString(my::str::widen(id).c_str());

	::VARIANT index_variant;
	::VariantInit(&index_variant);
	index_variant.vt = VT_I4;
	index_variant.intVal = 0;

	::IHTMLElement * element;
	if (FAILED(children->item(name_variant, index_variant, (::IDispatch **)&element)))
	{
		goto RELEASE_ITEM_VARIABLES;
	}

	if (FAILED(element->QueryInterface(::IID_IHTMLElement, (void **)&element)))
	{
		goto RELEASE_ELEMENT;
	}

	::BSTR value_bstr;
	if (FAILED(element->get_innerText(&value_bstr)))
	{
		goto RELEASE_ELEMENT;
	}

	inner_text = my::str::narrow((wchar_t *)value_bstr);
	result = true;

	::SysFreeString(value_bstr);

RELEASE_ELEMENT:
	element->Release();

RELEASE_ITEM_VARIABLES:
	::VariantClear(&index_variant);
	::SysFreeString(name_variant.bstrVal);
	::VariantClear(&name_variant);

RELEASE_CHILDREN:
	children->Release();

RELEASE_BODY:
	body->Release();

RELEASE_DOCUMENT:
	document->Release();

RETURN:
	return result;
} 