#pragma once

#include "mouse.hpp"

namespace memory
{
	ULONG get_process_id_by_name(PEPROCESS start_process, const char* process_name);

	ULONG get_module_base(PEPROCESS process, wchar_t* module_name);

	NTSTATUS read_memory(PEPROCESS target_process, void* source, void* target, size_t size);

	NTSTATUS write_memory(PEPROCESS target_process, void* source, void* target, size_t size);
}