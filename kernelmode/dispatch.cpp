#include "dispatch.hpp"

MOUSE_OBJECT mouse_obj = { 0 };

NTSTATUS dispatch::handler(PDEVICE_OBJECT device_object, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_object);

	if (!mouse_obj.service_callback || !mouse_obj.mouse_device) { mouse::init_mouse(&mouse_obj); }

	NTSTATUS status = STATUS_INVALID_PARAMETER;
	ULONG bytes_io = 0;
	PIO_STACK_LOCATION pio = IoGetCurrentIrpStackLocation(irp);
	ULONG ioctl = pio->Parameters.DeviceIoControl.IoControlCode;

	if (ioctl == NF_GET_PROCESS_ID)
	{
		PNF_PROCESS_ID_REQUEST process_id_request = (PNF_PROCESS_ID_REQUEST)irp->AssociatedIrp.SystemBuffer;
		process_id_request->process_id = memory::get_process_id_by_name(IoGetCurrentProcess(), process_id_request->process_name);
		if (process_id_request->process_id) { status = STATUS_SUCCESS; }
		bytes_io = sizeof(NF_PROCESS_ID_REQUEST);
		DbgPrintEx(0, 0, "[norsefire]: NF_GET_PROCESS_ID\n");
	}
	else if (ioctl == NF_GET_MODULE)
	{
		PNF_MODULE_REQUEST module_request = (PNF_MODULE_REQUEST)irp->AssociatedIrp.SystemBuffer;
		PEPROCESS target_process = 0;
		if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)module_request->process_id, &target_process)))
		{
			KAPC_STATE apc;
			KeStackAttachProcess(target_process, &apc);
			ULONG base = memory::get_module_base(target_process, module_request->module_name);
			KeUnstackDetachProcess(&apc);
			if (base)
			{
				module_request->address = base;
				status = STATUS_SUCCESS;
			}
			bytes_io = sizeof(NF_MODULE_REQUEST);
			DbgPrintEx(0, 0, "[norsefire]: NF_GET_MODULE\n");
		}
	}
	else if (ioctl == NF_READ_MEMORY)
	{
		PNF_READ_REQUEST read_request = (PNF_READ_REQUEST)irp->AssociatedIrp.SystemBuffer;
		PEPROCESS target_process = 0;
		if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)read_request->process_id, &target_process)))
		{
			status = memory::read_memory(target_process, (void*)read_request->address, &read_request->buffer, read_request->size);
		}
		bytes_io = sizeof(NF_READ_REQUEST);
		DbgPrintEx(0, 0, "[norsefire]: NF_READ_MEMORY\n");
	}
	else if (ioctl == NF_WRITE_MEMORY)
	{
		PNF_WRITE_REQUEST write_request = (PNF_WRITE_REQUEST)irp->AssociatedIrp.SystemBuffer;
		PEPROCESS target_process = 0;
		if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)write_request->process_id, &target_process)))
		{
			status = memory::write_memory(target_process, &write_request->buffer, (void*)write_request->address, write_request->size);
		}
		bytes_io = sizeof(NF_WRITE_REQUEST);
		DbgPrintEx(0, 0, "[norsefire]: NF_WRITE_MEMORY\n");
	}
	else if (ioctl == NF_MOUSE_EVENT)
	{
		PNF_MOUSE_REQUEST mouse_request = (PNF_MOUSE_REQUEST)irp->AssociatedIrp.SystemBuffer;
		mouse::mouse_event(mouse_obj, mouse_request->x, mouse_request->y, mouse_request->button_flags);
		status = STATUS_SUCCESS;
		bytes_io = sizeof(NF_MOUSE_REQUEST);
		DbgPrintEx(0, 0, "[norsefire]: NF_MOUSE_EVENT\n");
	}
	else
	{
		status = STATUS_INVALID_PARAMETER;
		bytes_io = 0;
		DbgPrintEx(0, 0, "[norsefire]: NF_FAILED_REQUEST\n");
	}

	irp->IoStatus.Status = status;
	irp->IoStatus.Information = bytes_io;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS dispatch::create_call(PDEVICE_OBJECT device_object, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_object);
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS dispatch::close_call(PDEVICE_OBJECT device_object, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_object);
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
