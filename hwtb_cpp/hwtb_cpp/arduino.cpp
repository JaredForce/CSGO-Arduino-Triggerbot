#pragma comment(lib, "Setupapi.lib")

#include "arduino.h"
#include <devguid.h>
#include <SetupAPI.h>
#include <iostream>


bool Arduino::get_device(LPCSTR friendly_name, LPSTR com_port)
{
	bool result = false;
	const char com[] = "COM";

	HDEVINFO device_info = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, nullptr, nullptr, DIGCF_PRESENT);
	if (device_info == INVALID_HANDLE_VALUE)
		return result;

	SP_DEVINFO_DATA dev_info_data = { };
	dev_info_data.cbSize = sizeof(dev_info_data);

	DWORD device_count = 0;
	while (SetupDiEnumDeviceInfo(device_info, device_count++, &dev_info_data))
	{
		BYTE buff[256] = { 0 };
		if (SetupDiGetDeviceRegistryProperty(
			device_info,
			&dev_info_data,
			SPDRP_FRIENDLYNAME,
			nullptr,
			buff,
			sizeof(buff),
			nullptr
		))
		{
			DWORD out_buff_len = strlen(com_port);
			LPCSTR port_name_pos = strstr(reinterpret_cast<LPCSTR>(buff), com);
			if (port_name_pos == nullptr)
				continue;

			DWORD len = out_buff_len + strlen(port_name_pos);
			if (strstr(reinterpret_cast<LPCSTR>(buff), friendly_name))
			{
				for (DWORD i = 0; i < len; i++, out_buff_len++)
					com_port[out_buff_len] = port_name_pos[i];

				com_port[strlen(com_port) - 1] = 0;
				result = true;
				break;
			}
		}
	}

	return result;
}

Arduino::~Arduino()
{
	CloseHandle(m_hArduino);
}

Arduino::Arduino(LPCSTR com_port)
{
	m_hArduino = CreateFile(
		com_port,
		GENERIC_READ | GENERIC_WRITE,
		0,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);

	if(m_hArduino == INVALID_HANDLE_VALUE)
		return;

	DCB dcb = { };
	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(m_hArduino, &dcb))
		return;

	dcb.BaudRate = CBR_9600;
	dcb.ByteSize = 8;
	dcb.StopBits = ONESTOPBIT;
	dcb.Parity = NOPARITY;

	if (!SetCommState(m_hArduino, &dcb))
		return;

	COMMTIMEOUTS cto = { };
	cto.ReadIntervalTimeout = 50;
	cto.ReadTotalTimeoutConstant = 50;
	cto.ReadTotalTimeoutMultiplier = 10;
	cto.WriteTotalTimeoutConstant = 50;
	cto.WriteTotalTimeoutMultiplier = 10;
	if(!SetCommTimeouts(m_hArduino, &cto))
		return;

	printf("Connected!\n");
	
}

bool Arduino::send_data(const char* buffer, SIZE_T buffer_size) const
{
	DWORD bw = 0;
	return WriteFile(m_hArduino, buffer, buffer_size, &bw, nullptr);
}



