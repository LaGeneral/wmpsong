#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

#include "Functions.h"

char name[MAX_PATH];

extern "C" __declspec(dllexport) char* GetWMPSong(void)
{
	RtlSecureZeroMemory(name, MAX_PATH);

	HWND hwnd = FindWindowA(NULL, "Windows Media Player");

	if (!hwnd)
	{
		strcpy_s(name, MAX_PATH, "Media Player nicht gefunden.");
		return name;
	}

	DWORD pid = NULL;
	GetWindowThreadProcessId(hwnd, &pid);

	if (!pid)
	{
		strcpy_s(name, MAX_PATH, "Prozess-ID nicht gefunden.");
		return name;
	}

	HANDLE process = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, false, pid);

	if (!process)
	{
		strcpy_s(name, MAX_PATH, "Zugriff wurde verweigert.");
		return name;
	}

	DWORD Addr = 0, Len = 0, Found = 0;

	MEMORY_BASIC_INFORMATION Mem;
	ZeroMemory(&Mem, sizeof(MEMORY_BASIC_INFORMATION));

	while (VirtualQueryEx(process, (LPCVOID)Addr, &Mem, sizeof(MEMORY_BASIC_INFORMATION)) && !Found)
	{
		if (Mem.Protect == PAGE_READWRITE && Mem.State == MEM_COMMIT && Mem.Type == MEM_PRIVATE)
		{
			BYTE* temp = new BYTE[Mem.RegionSize];

			ReadProcessMemory(process, (LPCVOID)Addr, temp, Mem.RegionSize, NULL);

			DWORD tempCount = 0;
			while (tempCount < Mem.RegionSize)
			{
				if (memcmp((temp + tempCount), "\x54\x41\x47", 0x3) == 0)
				{
					strcpy_s(name, MAX_PATH, (char*)(temp + tempCount + 0x3));
					delete[] temp;
					CloseHandle(process);
					return name;
				}
				tempCount++;
			}

			delete[] temp;
		}
		Addr += Mem.RegionSize;
	}

	CloseHandle(process);
	return name;
}