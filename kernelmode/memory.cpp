#include "memory.hpp"

ULONG memory::get_process_id_by_name(PEPROCESS start_process, const char* process_name)
{
	PLIST_ENTRY active_process_links;
	PEPROCESS current_process = start_process;

	do
	{
		PKPROCESS kproc = (PKPROCESS)current_process;
		PDISPATCHER_HEADER header = (PDISPATCHER_HEADER)kproc;
		LPSTR current_process_name = (LPSTR)((PUCHAR)current_process + IMAGE_FILE_NAME);

		if (header->SignalState == 0 && strcmp(current_process_name, process_name) == 0)
		{	
			return (ULONG)PsGetProcessId(current_process);
		}

		active_process_links = (PLIST_ENTRY)((PUCHAR)current_process + ACTIVE_PROCESS_LINKS_FLINK);
		current_process = (PEPROCESS)(active_process_links->Flink);
		current_process = (PEPROCESS)((PUCHAR)current_process - ACTIVE_PROCESS_LINKS_FLINK);

	} while (start_process != current_process);

	return 0;
}

ULONG memory::get_module_base(PEPROCESS process, wchar_t* module_name)
{
	if (!process) { return 0; }

	__try
	{
		PPEB32 peb32 = (PPEB32)PsGetProcessWow64Process(process);
		if (!peb32 || !peb32->Ldr) { return 0; }

		for (PLIST_ENTRY32 plist_entry = (PLIST_ENTRY32)((PPEB_LDR_DATA32)peb32->Ldr)->InLoadOrderModuleList.Flink;
			plist_entry != &((PPEB_LDR_DATA32)peb32->Ldr)->InLoadOrderModuleList;
			plist_entry = (PLIST_ENTRY32)plist_entry->Flink)
		{
			PLDR_DATA_TABLE_ENTRY32 pentry = CONTAINING_RECORD(plist_entry, LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);

			if (wcscmp((PWCH)pentry->BaseDllName.Buffer, module_name) == 0)
			{
				return pentry->DllBase;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{

	}

	return 0;
}

NTSTATUS memory::read_memory(PEPROCESS target_process, void* source, void* target, size_t size)
{
	if (!target_process) { return STATUS_INVALID_PARAMETER; }

	size_t bytes = 0;
	NTSTATUS status = MmCopyVirtualMemory(target_process, source, IoGetCurrentProcess(), target, size, KernelMode, &bytes);
	if (!NT_SUCCESS(status) || !bytes)
	{
		return STATUS_INVALID_ADDRESS;
	}
	return status;
}

NTSTATUS memory::write_memory(PEPROCESS target_process, void* source, void* target, size_t size)
{
	if (!target_process) { return STATUS_INVALID_PARAMETER; }

	size_t bytes = 0;
	NTSTATUS status = MmCopyVirtualMemory(IoGetCurrentProcess(), source, target_process, target, size, KernelMode, &bytes);
	if (!NT_SUCCESS(status) || !bytes)
	{
		return STATUS_INVALID_ADDRESS;
	}
	return status;
}
