#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "psapi.lib")

#include <csignal>
#include <map>
#include <regex>
#include <iostream>

#include <audiopolicy.h>
#include <mmdeviceapi.h>
#include <psapi.h>
#include <shlwapi.h>
#include <windows.h>

#include "..\path\path.h"
#include "..\ini\ini.h"
#include "..\log\log.h"
#include "..\proc\proc.h"
#include "..\str\str.h"
#include "..\io\io.h"

enum ErrorCodes
{
	ERR_INI_NAME = 1,
	ERR_INI_SECTIONS,
	ERR_INI_MAX,
	ERR_INI_RXP,
	ERR_CO_INITIALIZE,
	ERR_DEVICE_ENUMERATOR,
	ERR_DEVICE_COLLECTION,
	ERR_DEVICE_COLLECTION_COUNT,
	ERR_DEVICE_COLLECTION_ITEM,
	ERR_SESSION_MANAGER2,
	ERR_SESSION_ENUMERATOR,
	ERR_SESSION_ENUMERATOR_COUNT,
	ERR_SESSION_ENUMERATOR_GET_SESSION,
	ERR_SESSION_CONTROL2,
	ERR_CONTROL_PROCESS,
	ERR_CONTROL_NAME,
	ERR_CONTROL_VOLUME
};

typedef std::map<::IAudioSessionManager2 *, ::IAudioSessionNotification *> NotificationMap;
typedef std::pair<::IAudioSessionManager2 *, ::IAudioSessionNotification *> NotificationPair;

typedef std::map<::IAudioSessionControl2 *, ::IAudioSessionEvents *> EventsMap;
typedef std::pair<::IAudioSessionControl2 *, ::IAudioSessionEvents *> EventsPair;

unsigned long const buffer_size(32);
std::string const exe_path(my::proc::get_file());
std::string const exe_basename(my::path::basename(exe_path));
std::string const exe_name(my::path::remove_ext(exe_basename));
std::string const ini_path(my::path::change_ext(exe_path, "ini"));
std::string const log_path(my::path::change_ext(exe_path, "log"));
my::ini::file const ini_file(my::ini::file(ini_path, buffer_size));
my::ini::section const ini_section(ini_file, exe_name);
std::ios::openmode const ch_in_mode = std::ios::in;
std::ios::openmode const ch_out_mode = std::ios::out | std::ios::app;
my::io::ch const channel(buffer_size, ch_in_mode, ch_out_mode);
bool const log_enabled(ini_section.get_bool("log"));
unsigned char const log_lvl(ini_section.get_int("lvl"));
my::log::file const log_file(log_path, log_enabled, log_lvl, channel);

::IMMDeviceEnumerator * enumerator;
::IMMNotificationClient * client;
::NotificationMap notification_map;
::EventsMap events_map;

VOID SignalHandler(::INT signal);
::IMMDeviceEnumerator * GetEnumerator();
::IAudioSessionManager2 * GetManager2(::IMMDevice * device);
::IAudioSessionControl2 * GetControl2(::IAudioSessionControl * control);
::IMMNotificationClient * RegisterClient(::IMMDeviceEnumerator * enumerator);
VOID UnregisterClient(::IMMDeviceEnumerator * enumerator, ::IMMNotificationClient * client);
VOID RegisterAllNotification(::IMMDeviceEnumerator * enumerator);
VOID RegisterNotification(::IAudioSessionManager2 * manager2);
VOID UnregisterAllNotification();
VOID RegisterAllEvents(::IAudioSessionManager2 * manager2);
VOID RegisterEvents(::IAudioSessionControl2 * control2);
VOID UnregisterAllEvents();
VOID ControlVolume(::IAudioSessionControl2 * control2);
VOID ExitApp(::INT code);

class MMNotificationClient : public ::IMMNotificationClient
{
	::IMMDeviceEnumerator * enumerator;

public:
	MMNotificationClient(::IMMDeviceEnumerator * enumerator) : enumerator(enumerator) {}
	~MMNotificationClient() {}

	::ULONG STDMETHODCALLTYPE AddRef()
	{
		log_file.log(0, "MMNotificationClient.AddRef");
		return 0;
	}

	::HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID riid, _Out_ VOID ** ppvObject)
	{
		log_file.log(0, "MMNotificationClient.QueryInterface");
		return 0;
	}

	::ULONG STDMETHODCALLTYPE Release()
	{
		log_file.log(0, "MMNotificationClient.Release");
		return 0;
	}

	::HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(
		_In_ ::EDataFlow flow,
		_In_ ::ERole role,
		_In_ ::LPCWSTR pwstrDefaultDevice)
	{
		log_file.log(1, "MMNotificationClient.OnDefaultDeviceChanged");
		::UnregisterAllNotification();
		::RegisterAllNotification(this->enumerator);
		return S_OK;
	}

	::HRESULT STDMETHODCALLTYPE OnDeviceAdded(_In_ ::LPCWSTR pwstrDeviceId)
	{
		log_file.log(1, "MMNotificationClient.OnDeviceAdded");
		::UnregisterAllNotification();
		::RegisterAllNotification(this->enumerator);
		return S_OK;
	}

	::HRESULT STDMETHODCALLTYPE OnDeviceRemoved(_In_ ::LPCWSTR pwstrDeviceId)
	{
		log_file.log(1, "MMNotificationClient.OnDeviceRemoved");
		::UnregisterAllNotification();
		::RegisterAllNotification(this->enumerator);
		return S_OK;
	}

	::HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(_In_ ::LPCWSTR pwstrDeviceId, _In_ ::DWORD dwNewState)
	{
		log_file.log(1, "MMNotificationClient.OnDeviceStateChanged");
		::UnregisterAllNotification();
		::RegisterAllNotification(this->enumerator);
		return S_OK;
	}

	::HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(_In_ ::LPCWSTR pwstrDeviceId, _In_ const ::PROPERTYKEY key)
	{
		log_file.log(1, "MMNotificationClient.OnPropertyValueChanged");
		return S_OK;
	}
};

class AudioSessionNotification : public ::IAudioSessionNotification
{
public:
	AudioSessionNotification() {}
	~AudioSessionNotification() {}

	::ULONG STDMETHODCALLTYPE AddRef()
	{
		log_file.log(0, "AudioSessionNotification.AddRef");
		return 0;
	}

	::HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID riid, _Out_ VOID ** ppvObject)
	{
		log_file.log(0, "AudioSessionNotification.QueryInterface");
		return 0;
	}

	::ULONG STDMETHODCALLTYPE Release()
	{
		log_file.log(0, "AudioSessionNotification.Release");
		return 0;
	}

	::HRESULT STDMETHODCALLTYPE OnSessionCreated(_In_ ::IAudioSessionControl * NewSession)
	{
		log_file.log(0, "AudioSessionNotification.OnSessionCreated");
		::IAudioSessionControl2 * control2 = ::GetControl2(NewSession);
		::ControlVolume(control2);
		::RegisterEvents(control2);
		return S_OK;
	}
};

class AudioSessionEvents : public ::IAudioSessionEvents
{
	::IAudioSessionControl2 * control2;

public:
	AudioSessionEvents(::IAudioSessionControl2 * control2) : control2(control2) {}
	~AudioSessionEvents() {}

	::ULONG STDMETHODCALLTYPE AddRef()
	{
		log_file.log(0, "AudioSessionEvents.AddRef");
		return 0;
	}

	::HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID riid, _Out_ VOID ** ppvObject)
	{
		log_file.log(0, "AudioSessionEvents.QueryInterface");
		return S_OK;
	}

	::ULONG STDMETHODCALLTYPE Release()
	{
		log_file.log(0, "AudioSessionEvents.QueryInterface");
		return 0;
	}

	::HRESULT STDMETHODCALLTYPE OnChannelVolumeChanged(
		_In_ ::DWORD ChannelCount,
		_In_ ::FLOAT NewChannelVolumeArray[],
		_In_ ::DWORD ChangedChannel,
		_In_ ::LPCGUID EventContext)
	{
		log_file.log(1, "AudioSessionEvents.OnChannelVolumeChanged");
		return S_OK;
	}

	::HRESULT STDMETHODCALLTYPE OnDisplayNameChanged(_In_ ::LPCWSTR NewDisplayName, _In_ ::LPCGUID EventContext)
	{
		log_file.log(1, "AudioSessionEvents.OnDisplayNameChanged");
		return S_OK;
	}

	::HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(_In_ ::LPCGUID NewGroupingParam, _In_ ::LPCGUID EventContext)
	{
		log_file.log(1, "AudioSessionEvents.OnGroupingParamChanged");
		return S_OK;
	}

	::HRESULT STDMETHODCALLTYPE OnIconPathChanged(_In_ ::LPCWSTR NewIconPath, _In_ ::LPCGUID EventContext)
	{
		log_file.log(1, "AudioSessionEvents.OnIconPathChanged");
		return S_OK;
	}

	::HRESULT STDMETHODCALLTYPE OnSessionDisconnected(_In_ ::AudioSessionDisconnectReason DisconnectReason)
	{
		log_file.log(1, "AudioSessionEvents.OnSessionDisconnected");
		return S_OK;
	}

	::HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(
		_In_ ::FLOAT NewVolume,
		_In_ ::BOOL NewMute,
		_In_ ::LPCGUID EventContext)
	{
		log_file.log(1, "AudioSessionEvents.OnSimpleVolumeChanged");
		::ControlVolume(this->control2);
		return S_OK;
	}

	::HRESULT STDMETHODCALLTYPE OnStateChanged(
		_In_ ::AudioSessionState NewState)
	{
		log_file.log(1, "AudioSessionEvents.OnStateChanged");
		return S_OK;
	}
};

::INT APIENTRY wWinMain(
	_In_ ::HINSTANCE hInstance,
	_In_opt_ ::HINSTANCE hPrevInstance,
	_In_ ::LPWSTR lpCmdLine,
	_In_ ::INT nCmdShow)
{
	log_file.log(1, "wWinMain");
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);
	if (FAILED(::CoInitialize(nullptr)))
	{
		::ExitApp(ERR_CO_INITIALIZE);
	}
	enumerator = ::GetEnumerator();
	client = ::RegisterClient(enumerator);
	std::signal(SIGABRT, ::SignalHandler);
	std::signal(SIGFPE, ::SignalHandler);
	std::signal(SIGILL, ::SignalHandler);
	std::signal(SIGINT, ::SignalHandler);
	std::signal(SIGSEGV, ::SignalHandler);
	std::signal(SIGTERM, ::SignalHandler);
	::Sleep(INFINITE);
}

VOID SignalHandler(::INT signal)
{
	log_file.log(1, "SignalHandler" + signal);
	::UnregisterClient(enumerator, client);
	::CoUninitialize();
	::ExitApp(EXIT_SUCCESS);
}

::IMMDeviceEnumerator * GetEnumerator()
{
	log_file.log(1, "GetEnumerator");
	::IMMDeviceEnumerator * enumerator(nullptr);
	if (FAILED(
		::CoCreateInstance(
		__uuidof(::MMDeviceEnumerator),
		nullptr,
		CLSCTX_ALL,
		__uuidof(::IMMDeviceEnumerator),
		(VOID **)&enumerator)))
	{
		::ExitApp(ERR_DEVICE_ENUMERATOR);
	}
	return enumerator;
}

::IAudioSessionManager2 * GetManager2(::IMMDevice * device)
{
	log_file.log(1, "GetManager2");
	::IAudioSessionManager2 * manager2(nullptr);
	if (FAILED(
		device->Activate(
		__uuidof(::IAudioSessionManager2),
		CLSCTX_ALL,
		nullptr,
		(VOID**)&manager2)))
	{
		::ExitApp(ERR_SESSION_MANAGER2);
	}
	return manager2;
}

::IAudioSessionControl2 * GetControl2(::IAudioSessionControl * control)
{
	log_file.log(1, "GetControl2");
	::IAudioSessionControl2 * control2 = nullptr;
	if (FAILED(
		control->QueryInterface(
		__uuidof(::IAudioSessionControl2),
		(VOID **)&control2)))
	{
		::ExitApp(ERR_SESSION_CONTROL2);
	}
	return control2;
}

::IMMNotificationClient * RegisterClient(::IMMDeviceEnumerator * enumerator)
{
	log_file.log(1, "RegisterClient");
	::RegisterAllNotification(enumerator);
	::IMMNotificationClient * client(new ::MMNotificationClient(enumerator));
	enumerator->RegisterEndpointNotificationCallback(client);
	return client;
}

VOID UnregisterClient(::IMMDeviceEnumerator * enumerator, ::IMMNotificationClient * client)
{
	log_file.log(1, "UnregisterClient");
	::UnregisterAllEvents();
	::UnregisterAllNotification();
	enumerator->UnregisterEndpointNotificationCallback(client);
	delete client;
	client = nullptr;
	enumerator->Release();
	enumerator = nullptr;
}

VOID RegisterAllNotification(::IMMDeviceEnumerator * enumerator)
{
	log_file.log(1, "RegisterAllNotification");
	::IMMDeviceCollection * collection(nullptr);
	if (FAILED(
		enumerator->EnumAudioEndpoints(
		::EDataFlow::eRender,
		DEVICE_STATE_ACTIVE,
		&collection)))
	{
		::ExitApp(ERR_DEVICE_COLLECTION);
	}
	::UINT count(0);
	if (FAILED(collection->GetCount(&count)))
	{
		::ExitApp(ERR_DEVICE_COLLECTION_COUNT);
	}
	for (::UINT i(0); i < count; ++i)
	{
		::IMMDevice * device(nullptr);
		if (FAILED(collection->Item(i, &device)))
		{
			::ExitApp(ERR_DEVICE_COLLECTION_ITEM);
		}
		::IAudioSessionManager2 * manager2(::GetManager2(device));
		::RegisterAllEvents(manager2);
		::RegisterNotification(manager2);
		device->Release();
		device = nullptr;
	}
	collection->Release();
	collection = nullptr;
}

VOID RegisterNotification(::IAudioSessionManager2 * manager2)
{
	log_file.log(1, "RegisterNotification");
	::IAudioSessionNotification * notification(new ::AudioSessionNotification());
	manager2->RegisterSessionNotification(notification);
	notification_map.insert(NotificationPair(manager2, notification));
}

VOID UnregisterAllNotification()
{
	log_file.log(1, "UnregisterAllNotification");
	::UnregisterAllEvents();
	for (auto const & notification_pair : notification_map)
	{
		::IAudioSessionManager2 * manager2(notification_pair.first);
		::IAudioSessionNotification * notification(notification_pair.second);
		manager2->UnregisterSessionNotification(notification);
		delete notification;
		notification = nullptr;
		manager2->Release();
		manager2 = nullptr;
	}
	notification_map.clear();
}

VOID RegisterAllEvents(::IAudioSessionManager2 * manager2)
{
	log_file.log(1, "RegisterAllEvents");
	::IAudioSessionEnumerator * enumerator(nullptr);
	if (FAILED(manager2->GetSessionEnumerator(&enumerator)))
	{
		::ExitApp(ERR_SESSION_ENUMERATOR);
	}
	::INT count(0);
	if (FAILED(enumerator->GetCount(&count)))
	{
		::ExitApp(ERR_SESSION_ENUMERATOR_COUNT);
	}
	for (::INT i(0); i < count; ++i)
	{
		::IAudioSessionControl * control(nullptr);
		if (FAILED(enumerator->GetSession(i, &control)))
		{
			::ExitApp(ERR_SESSION_ENUMERATOR_GET_SESSION);
		}
		::IAudioSessionControl2 * control2(::GetControl2(control));
		::ControlVolume(control2);
		::RegisterEvents(control2);
	}
	enumerator->Release();
	enumerator = nullptr;
}

VOID RegisterEvents(::IAudioSessionControl2 * control2)
{
	log_file.log(1, "RegisterEvents");
	::IAudioSessionEvents * events(new ::AudioSessionEvents(control2));
	control2->RegisterAudioSessionNotification(events);
	events_map.insert(::EventsPair(control2, events));
}

VOID UnregisterAllEvents()
{
	log_file.log(1, "UnregisterAllEvents");
	for (auto const & events_pair : events_map)
	{
		::IAudioSessionControl2 * control2(events_pair.first);
		::IAudioSessionEvents * events(events_pair.second);
		control2->UnregisterAudioSessionNotification(events);
		delete events;
		events = nullptr;
		control2->Release();
		control2 = nullptr;
	}
	events_map.clear();
}

VOID ControlVolume(::IAudioSessionControl2 * control2)
{
	log_file.log(1, "ControlVolume");
	::DWORD id;
	if (FAILED(control2->GetProcessId(&id)))
	{
		::ExitApp(ERR_CONTROL_PROCESS);
	}
	if (id == 0)
	{
		return;
	}
	std::string path;
	if (!my::proc::get_file(id, path))
	{
		::ExitApp(ERR_CONTROL_NAME);
	}
	::PWSTR display_name;
	control2->GetDisplayName(&display_name);
	for (auto const & section : ini_file.get_sections())
	{
		if (section.get_name() == ini_section.get_name())
		{
			continue;
		}
#pragma push_macro("max")
#undef max
		FLOAT max(section.get_int("max") / 100.0f);
#pragma pop_macro("max")
		if (max < 0 || max > 100)
		{
			::ExitApp(ERR_INI_MAX);
		}
		try
		{
			std::regex rxp(section.get_string("rxp"));
			if (std::regex_match(path, rxp) || std::regex_match(my::str::narrow(display_name), rxp))
			{
				::ISimpleAudioVolume * volume(nullptr);
				if (FAILED(
					control2->QueryInterface(
					__uuidof(::ISimpleAudioVolume),
					(VOID **)&volume)))
				{
					::ExitApp(ERR_CONTROL_VOLUME);
				}
				::FLOAT level;
				volume->GetMasterVolume(&level);
				if (level > max)
				{
					volume->SetMasterVolume(max, nullptr);
				}
				volume->Release();
				volume = nullptr;
				break;
			}
		}
		catch (std::regex_error&)
		{
			::ExitApp(ERR_INI_RXP);
		}
	}
	::CoTaskMemFree(display_name);
}

VOID ExitApp(::INT code)
{
	log_file.log(1, "ExitApp " + code);
	::ExitProcess(code);
}
