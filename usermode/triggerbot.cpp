#include "triggerbot.hpp"

void triggerbot::run()
{
	driver_control driver("\\\\.\\norsefire");

	const char process_name[] = "csgo.exe";
	const wchar_t module_name[] = L"client.dll";

	printf("[norsefire]: Looking for %s process...\n", process_name);
	
	ULONG process_id = driver.get_process_id_by_name(process_name, sizeof(process_name));
	if (process_id)
	{
		printf("[norsefire]: Process identifier found: %d\n", process_id);
		printf("[norsefire]: Looking for %ws...\n", module_name);
		ULONG client_base = driver.get_module_base(process_id, module_name, sizeof(module_name));
		if (client_base)
		{
			printf("[norsefire]: %ws found: 0x%x\n", module_name, client_base);
			printf("[norsefire]: Running...\n");
			for (;;)
			{
				ULONG my_player = driver.read_memory<ULONG>(process_id, client_base + m_dwLocalPlayer);
				if (my_player <= 0) { continue; }

				ULONG my_team = driver.read_memory<ULONG>(process_id, my_player + m_iTeamNum);
				ULONG my_cross = driver.read_memory<ULONG>(process_id, my_player + m_iCrossHairID);

				if (my_cross > 0 && my_cross <= 64)
				{
					ULONG entity = driver.read_memory<ULONG>(process_id, client_base + m_dwEntityList + ((my_cross - 1) * 0x10));
					if (entity <= 0) { continue; }

					ULONG entity_health = driver.read_memory<ULONG>(process_id, entity + m_iHealth);
					ULONG entity_team = driver.read_memory<ULONG>(process_id, entity + m_iTeamNum);

					if (entity_health > 0 && entity_team != my_team && entity_team > 1)
					{
						Sleep(3);
						driver.mouse_event(0, 0, 0x1);
						Sleep(1);
						driver.mouse_event(0, 0, 0x2);
					}
				}
				Sleep(1);
			}
		}
		else
		{
			printf("[norsefire]: Failed to obtain %ws\n", module_name);
		}
	}
	else
	{
		printf("[norsefire]: Failed to find %s process\n", process_name);
	}
}
