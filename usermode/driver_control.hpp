#pragma once

#include "definitions.hpp"

class driver_control
{
	private:

		HANDLE driver_handle = 0;

	public:

		driver_control(const char* path);

		~driver_control();

		ULONG get_process_id_by_name(const char* process_name, size_t process_name_size);

		ULONG get_module_base(ULONG process_id, const wchar_t* module_name, size_t module_name_size);

		template<typename t>
		t read_memory(ULONG process_id, ULONG address);

		template<typename t>
		void write_memory(ULONG process_id, ULONG address, t buffer);

		void mouse_event(long x, long y, unsigned short button_flags);
};

template<typename t>
inline t driver_control::read_memory(ULONG process_id, ULONG address)
{
	if (this->driver_handle)
	{
		ULONG bytes;
		NF_READ_REQUEST read_request = { 0 };
		read_request.process_id = process_id;
		read_request.address = address;
		read_request.size = sizeof(t);
		if (DeviceIoControl(driver_handle, NF_READ_MEMORY, &read_request, sizeof(read_request), &read_request, sizeof(read_request), &bytes, 0))
		{
			return *(t*)&read_request.buffer;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

template<typename t>
inline void driver_control::write_memory(ULONG process_id, ULONG address, t buffer)
{
	if (this->driver_handle)
	{
		ULONG bytes;
		NF_WRITE_REQUEST write_request = { 0 };
		write_request.process_id = process_id;
		write_request.address = address;
		write_request.buffer = buffer;
		write_request.size = sizeof(t);
		DeviceIoControl(driver_handle, NF_WRITE_MEMORY, &write_request, sizeof(write_request), &write_request, sizeof(write_request), &bytes, 0);
	}
}
