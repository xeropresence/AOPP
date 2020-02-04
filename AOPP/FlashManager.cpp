#include "FlashManager.h"
#include <tlhelp32.h>
#include <ostream>
#include <iostream>

void FlashManager::Flash() const
{
	//std::cout << "Flash requested " << hwnd <<  std::endl;
	FLASHWINFO fi;
	fi.cbSize = sizeof(FLASHWINFO);
	fi.hwnd = hwnd;
	fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
	fi.uCount = 0;
	fi.dwTimeout = 0;
	FlashWindowEx(&fi);
}

FlashManager::FlashManager()
{
	const auto dwOwnerPID = GetCurrentProcessId();
	const auto EnumThreadWndProc = [](_In_ HWND a, _In_ LPARAM b)
	{
		if (IsWindowVisible(a))
		{
			reinterpret_cast<FlashManager*>(b)->hwnd = a;
			return TRUE;
		}
		return FALSE;
	};

	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	// Take a snapshot of all running threads  
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
		return;

	// Fill in the size of the structure before using it. 
	te32.dwSize = sizeof(THREADENTRY32);

	// Retrieve information about the first thread,
	// and exit if unsuccessful
	if (!Thread32First(hThreadSnap, &te32))
	{
		CloseHandle(hThreadSnap); // Must clean up the snapshot object!
		return;
	}

	// Now walk the thread list of the system,
	// and display information about each thread
	// associated with the specified process
	do
	{
		if (te32.th32OwnerProcessID == dwOwnerPID)
		{
			if (EnumThreadWindows(te32.th32ThreadID, EnumThreadWndProc, reinterpret_cast<LPARAM>(this)))
			{
				break;
			}
		}
	}
	while (Thread32Next(hThreadSnap, &te32));


	//  Don't forget to clean up the snapshot object.
	CloseHandle(hThreadSnap);
}
