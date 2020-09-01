#pragma once

#include "memory.hpp"

namespace dispatch
{
	NTSTATUS handler(PDEVICE_OBJECT device_object, PIRP irp);

	NTSTATUS create_call(PDEVICE_OBJECT device_object, PIRP irp);

	NTSTATUS close_call(PDEVICE_OBJECT device_object, PIRP irp);
}