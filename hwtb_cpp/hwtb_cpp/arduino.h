#pragma once
#include <Windows.h>

class Arduino
{
public:
	~Arduino();
	Arduino(LPCSTR com_port);
	bool send_data(const char* buffer, SIZE_T buffer_size) const;
	static bool get_device(LPCSTR friendly_name, LPSTR com_port);

private:
	HANDLE m_hArduino;
};
