#include "dispatch.hpp"

PDEVICE_OBJECT device_object;
UNICODE_STRING dev, dos;

void unload(PDRIVER_OBJECT driver_object)
{
	IoDeleteSymbolicLink(&dos);
	IoDeleteDevice(driver_object->DeviceObject);
	DbgPrintEx(0, 0, "[norsefire]: Driver unloaded\n");
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path)
{
	UNREFERENCED_PARAMETER(registry_path);

	RtlInitUnicodeString(&dev, L"\\Device\\norsefire");
	RtlInitUnicodeString(&dos, L"\\DosDevices\\norsefire");
	
	IoCreateDevice(driver_object, 0, &dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device_object);
	IoCreateSymbolicLink(&dos, &dev);

	driver_object->MajorFunction[IRP_MJ_CREATE] = dispatch::create_call;
	driver_object->MajorFunction[IRP_MJ_CLOSE] = dispatch::close_call;
	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = dispatch::handler;
	driver_object->DriverUnload = unload;

	device_object->Flags |= DO_DIRECT_IO;
	device_object->Flags &= ~DO_DEVICE_INITIALIZING;

	DbgPrintEx(0, 0, "[norsefire]: Driver loaded\n");
	return STATUS_SUCCESS;
}