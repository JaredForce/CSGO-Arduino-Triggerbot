#include "arduino.h"
#include "memory.h"
#include <iostream>
#include <string>
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <iostream>

memory mem;
PModule bClient;

#define dwLocalPlayer	0xDC14CC
#define dwEntityList	0x4DDD91C

#define m_iTeamNum		0xF4
#define m_iHealth		0x100
#define m_iCrossHairID	0x11838


int main()
{
	std::string clickNumber = std::to_string(0x1337);
	std::string setDelayNumber = std::to_string(0x1338);

	char com_name[100] = "\\.\\";
	while (!Arduino::get_device("Arduino Leonardo", com_name)) { Sleep(1000); }
	Arduino mouse(com_name);

	while (!mem.Attach("csgo.exe", PROCESS_ALL_ACCESS)) { Sleep(1000); }
	bClient = mem.GetModule("client.dll");

	std::string delay = "";
	std::cout << "Enter Delay (ms) ->";
	std::cin >> delay;

	std::cout << "Started!\n";


	while (true)
	{
		if (GetAsyncKeyState(VK_MENU))
		{

			bool result = mouse.send_data(setDelayNumber.c_str(), setDelayNumber.length() + 1);
			if (!result)
				return -1;

			result = mouse.send_data(delay.c_str(), delay.length() + 1);
			if (!result)
				return -1;

			DWORD pLocal = mem.Read<DWORD>(bClient.dwBase + dwLocalPlayer);
			DWORD lHealth = mem.Read<DWORD>(pLocal + m_iHealth);

			if (!lHealth)
				continue;

			DWORD lTeamNum = mem.Read<DWORD>(pLocal + m_iTeamNum);


			DWORD lCrosshairId = mem.Read<DWORD>(pLocal + m_iCrossHairID);
			if (!lCrosshairId || lCrosshairId > 64)
				continue;

			DWORD entityBase = mem.Read<int>(bClient.dwBase + dwEntityList + (lCrosshairId - 1) * 0x10);
			if (!entityBase)
				continue;

			DWORD entityHp = mem.Read<DWORD>(entityBase + m_iHealth);
			DWORD entityTeam = mem.Read<DWORD>(entityBase + m_iTeamNum);

			if (!entityHp || entityTeam == lTeamNum)
				continue;


			result = mouse.send_data(clickNumber.c_str(), clickNumber.length() + 1);
			if (!result)
				break;
		}
	}




	return 0;
}