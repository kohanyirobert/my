#pragma once

namespace my
{
	class ole final
	{
	private:
		static bool initialized;

	public:
		static bool get_initialized();

		static bool initialize();
		static void uninitialize();
	};
}