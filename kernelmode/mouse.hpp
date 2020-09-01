#pragma once

#include "definitions.hpp"

extern "C" POBJECT_TYPE* IoDriverObjectType;

typedef VOID
(*MouseClassServiceCallback)(
	PDEVICE_OBJECT DeviceObject,
	PMOUSE_INPUT_DATA InputDataStart,
	PMOUSE_INPUT_DATA InputDataEnd,
	PULONG InputDataConsumed
);

typedef struct _MOUSE_OBJECT
{
	PDEVICE_OBJECT mouse_device;
	MouseClassServiceCallback service_callback;
} MOUSE_OBJECT, * PMOUSE_OBJECT;

namespace mouse
{
	NTSTATUS init_mouse(PMOUSE_OBJECT mouse_obj);

	void mouse_event(MOUSE_OBJECT mouse_obj, long x, long y, unsigned short button_flags);
}