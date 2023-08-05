/*
		hAx State Changer for FFXI v1.1
		by Zak Farrington alias fritz <fritz@hax-studios.net>

		Copyright (C) 2005 hAx Studios Ltd.
		All rights reserved.

		http://hax-studios.net
*/

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>	
	#include <crtdbg.h>
#else
	//** Compiler and linker optimization.
	#pragma comment(linker,"/FILEALIGN:512 /MERGE:.rdata=.text /MERGE:.data=.text /SECTION:.text,EWR /IGNORE:4078")
	#include <cstdlib>	
#endif

#define STRICT				//** Enable STRICT type checking.
#define WIN32_LEAN_AND_MEAN	//** Exclude rarely-used stuff from Windows headers.

#include <windows.h>
#include <cstdio>
#include <tlhelp32.h>		//** For CreateToolhelp32Snapshot().
#include "Main.h"
#include "resource.h"		//** For our resources.


CConfig		cConfig;		//** Initalize configuration class.
CFFXI		cFFXI;			//** Initalize FFXI class.
CPlayer		cPlayer;		//** Intialize player class.


//** Program entry.
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
#endif

	HANDLE hProcess = GetCurrentProcess();							//** hProcess is the handle to our application
	HMODULE hKernel = LoadLibrary("kernel32.dll");					//** Load our library
	FARPROC iAddr = GetProcAddress(hKernel, "WriteProcessMemory");	//** Get the address of WriteProcessMemory in kernel32.dll
	BYTE bByte;														//** The byte that will hold the information retreived via ReadProcessMemory

	if(ReadProcessMemory(hProcess, (LPVOID)iAddr, &bByte, sizeof(bByte), NULL))
	{
		if(bByte == 0xCC) //** If theres a breakpoint (0xCC == breakpoint!).
		{
			return -1;
		}
		
		iAddr = GetProcAddress(hKernel, "ReadProcessMemory");
		ReadProcessMemory(hProcess, (LPVOID)iAddr, &bByte, sizeof(bByte), NULL);
		
		if(bByte == 0xCC)
		{
			return -1;
		}

		CloseHandle(hProcess);
		FreeLibrary(hKernel);
	}

	//** Custom IsDebuggerPresent() function... not Win32API version.
	if(IsDebuggerPresent() == TRUE)
	{
		return -1;
	}
	
	g_hInstance = hInstance;
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MAINDLG), NULL, DialogProc, NULL);

	return 0;
}

//** Debugger detection.
BOOL IsDebuggerPresent()
{
	DWORD dwDword;

	__asm
	{
		push eax  
		push ecx
		mov eax, fs:[0x18]  
		mov eax, dword ptr [eax + 0x30]
		mov ecx, dword ptr [eax]    
		mov dwDword, ecx
		pop ecx 
		pop eax
	}

	return (BOOL)(dwDword & 0x00010000 ? TRUE : FALSE);
}

//** Dialog callback.
BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			OnInitDlg(hWnd); //** Program initialization procedures.

			//** Sets icon.
			SendMessage(hWnd, WM_SETICON, IMAGE_ICON, (LPARAM)LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_MAIN)));
		break;

		case WM_COMMAND:
			switch(wParam)
			{
				case IDC_CLOSE:
					SendMessage(hWnd, WM_CLOSE, NULL, NULL);						
				break;

				case IDC_ABOUT:
					MessageBox(hWnd, "hAx FFXI State Changer by fritz\r\n\r\n- Function:\tScrew over Square-Enix.\r\n- Platform(s):\tWin9x/Me/NT/XP\r\n- Language:\tC++ Win32API\r\n- Author:\t\tZak Farrington alias fritz\r\n- Homepage:\thttp://hax-studios.net\r\n\r\n- Default Speed Hotkeys:\r\nNormal:\tInsert\r\nStrider:\tDelete\r\nFlee:\tHome\r\nHyper:\tEnd\r\n\r\nCopyright (C) 2005 hAx Studios Ltd.\r\nAll rights reserved.", "[:: hAx FFXI State Changer ::]", MB_OK + MB_ICONINFORMATION);
				break;

				case IDC_STATE_CHOCO:
					if(cPlayer.ChangeState(STATE_CHOCO) != TRUE)
					{
					}
				break;

				case IDC_STATE_NORMAL:
					if(cPlayer.ChangeState(STATE_NORMAL) != TRUE)
					{
					}
				break;

				case IDC_STATE_DEATH:
					if(cPlayer.ChangeState(STATE_DEATH) != TRUE)
					{
					}
				break;

				case IDC_STATE_ATTACK:
					if(cPlayer.ChangeState(0x01) != TRUE)
					{
					}
				break;

				case IDC_STATE_FISH:
					if(cPlayer.ChangeState(STATE_FISHING) != TRUE)
					{
					}
				break;

				case IDC_STATE_REST:
					if(cPlayer.ChangeState(STATE_RESTING) != TRUE)
					{
					}
				break;

				case IDC_STATE_SIT:
					if(cPlayer.ChangeState(STATE_SITTING) != TRUE)
					{
					}
				break;

				case IDC_SPEED_NORMAL:
					if(cPlayer.ChangeSpeed(SPEED_NORMAL) != TRUE)
					{
					}
				break;

				case IDC_SPEED_STRIDER:
					if(cPlayer.ChangeSpeed(SPEED_STRIDER) != TRUE)
					{
					}
				break;

				case IDC_SPEED_FLEE:
					if(cPlayer.ChangeSpeed(SPEED_FLEE) != TRUE)
					{
					}
				break;

				case IDC_SPEED_HYPER:
					if(cPlayer.ChangeSpeed(SPEED_HYPER) != TRUE)
					{
					}
				break;
			}
		break;

		case WM_TIMER:
			OnTimer(hWnd, wParam); //** Timer procedure.	
		break;

		case WM_CLOSE:
			//** START: Kill timers
			KillTimer(hWnd, CHECK_STATE_ADDRESS_TIMER);
			KillTimer(hWnd, CHECK_STATE_TIMER);
			KillTimer(hWnd, CHECK_SPEED_TIMER);
			KillTimer(hWnd, CHECK_NAME_TIMER);
			KillTimer(hWnd, CHECK_TNL_TIMER);
			KillTimer(hWnd, CHECK_HOTKEY_TIMER);
			//** END

#ifdef _DEBUG
			_CrtDumpMemoryLeaks();
#endif
			
			EndDialog(hWnd, 0);	//** Quit.
		break;
	}

	return FALSE;
}

//** Initialization code.
void OnInitDlg(HWND hWnd)
{
	//** Get the working directory of our binary.
	if(GetCurrentDirectory(sizeof(cConfig.strBaseDirectory), cConfig.strBaseDirectory) != NULL)
	{
		strcpy(cConfig.strConfigFile, cConfig.strBaseDirectory);
		strcat(cConfig.strConfigFile, "\\hAxFFXIConfig.cfg");

		if(cConfig.ReadConfigFile(cConfig.strConfigFile) != TRUE)	
		{
			//** Couldn't open configuration file.
			MessageBox(hWnd, "Warning: Could not read configuration file.  Program will attempt to continue by using default values.", "[:: hAx FFXI State Changer: Warning ::]", MB_OK + MB_ICONWARNING);
				
			cConfig.dwPcMapOffset		= 0x05C4A70;
			cConfig.dwPlayerStatsOffset	= 0x05C42F0;	
			cConfig.dwPlayerInfoOffset	= 0x06AF07E;
		
		}
	}

	if((cFFXI.IsGameRunning = cFFXI.CheckFFXI()) != FALSE) //** Check if FFXI is running.
	{
		//** Set POL PID on our dialog.
		sprintf(strBuffer1, "POL PID: 0x%X", cFFXI.dwPID);
		SetDlgItemText(hWnd, IDC_POL_PID, strBuffer1);

		//** Set FFXiMain.dll base address on our dialog.
		sprintf(strBuffer1, "FFXiMain.dll Base: 0x%X", cFFXI.dwModBase);
		SetDlgItemText(hWnd, IDC_FFXI_BASE, strBuffer1);

		//** Set information on dialog.
		if(cFFXI.GetStateAddress())
		{
			UpdateInformation(hWnd, IDC_CHARACTER_NAME);
			UpdateInformation(hWnd, IDC_TNL);
			UpdateInformation(hWnd, IDC_CURRENT_SPEED);
			UpdateInformation(hWnd, IDC_CURRENT_STATE);
		}

		//** START: Set timers
		SetTimer(hWnd, CHECK_STATE_ADDRESS_TIMER, 3000, NULL);	//** Updates address every 3 seconds.
		SetTimer(hWnd, CHECK_STATE_TIMER, 1500, NULL);			//** Updates state every 1.5 seconds.
		SetTimer(hWnd, CHECK_SPEED_TIMER, 2000, NULL);			//** Updates speed every 2 seconds.
		SetTimer(hWnd, CHECK_NAME_TIMER, (1000 * 60), NULL);	//** Updates name every minute.
		SetTimer(hWnd, CHECK_TNL_TIMER, (1000 * 15), NULL);		//** Updates TNL every 15 seconds.
		SetTimer(hWnd, CHECK_HOTKEY_TIMER, 700, NULL);			//** Checks hotkey every .7 seconds.
		//** END
	}
	else
	{
		MessageBox(hWnd, "Error: Could not locate FFXI process!", "[:: hAx FFXI State Changer: Error ::]", MB_OK + MB_ICONERROR);
		SendMessage(hWnd, WM_CLOSE, NULL, NULL);
	}
}

//** Timer procedure.
void OnTimer(HWND hWnd, UINT nID)
{
	//** Check state address timer.  Keep in mind the address changes every time you change maps.
	if(nID == CHECK_STATE_ADDRESS_TIMER)
	{
		if(cFFXI.GetStateAddress() == NULL)
		{
			MessageBox(hWnd, "Error: Failed to retrieve state address!", "[:: hAx FFXI State Changer: Error ::]", MB_OK | MB_ICONERROR);
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}
	else if(nID == CHECK_STATE_TIMER) //** Check state timer.
	{
		UpdateInformation(hWnd, IDC_CURRENT_STATE);
	}
	else if(nID == CHECK_TNL_TIMER)
	{
		UpdateInformation(hWnd, IDC_TNL);
	}
	else if(nID == CHECK_NAME_TIMER) //** Check name timer.
	{
		UpdateInformation(hWnd, IDC_CHARACTER_NAME);
	}
	else if(nID == CHECK_SPEED_TIMER)
	{
		UpdateInformation(hWnd, IDC_CURRENT_SPEED);
	}
	else if(nID == CHECK_HOTKEY_TIMER) //** Hotkey timer.
	{
		/*
		//** Bad place for this... but it's the fastest timer.
		if(IsDebuggerPresent() == TRUE)
		{
			TerminateProcess(GetCurrentProcess(), -1);
		}
		*/

		if(GetAsyncKeyState(cConfig.sHotkey.dwSpeedNormal))
		{
			SendMessage(hWnd, WM_COMMAND, (WPARAM)IDC_SPEED_NORMAL, NULL);
		}
		else if(GetAsyncKeyState(cConfig.sHotkey.dwSpeedStrider))
		{
			SendMessage(hWnd, WM_COMMAND, (WPARAM)IDC_SPEED_STRIDER, NULL);
		}
		else if(GetAsyncKeyState(cConfig.sHotkey.dwSpeedFlee))
		{
			SendMessage(hWnd, WM_COMMAND, (WPARAM)IDC_SPEED_FLEE, NULL);
		}
		else if(GetAsyncKeyState(cConfig.sHotkey.dwSpeedHyper))
		{
			SendMessage(hWnd, WM_COMMAND, (WPARAM)IDC_SPEED_HYPER, NULL);
		}
	}
}

//** Updates information item on the dialog.
void UpdateInformation(HWND hWnd, DWORD dwItem)
{
	switch(dwItem)
	{
		case IDC_CHARACTER_NAME:
			sprintf(strBuffer1, "Name: %s", cPlayer.GetName());

			//** Let's prevent unnecessary flickering at expense of a few extra CPU cycles.
			GetDlgItemText(hWnd, IDC_CHARACTER_NAME, strBuffer2, 16);
			if(stricmp(strBuffer1, strBuffer2) != 0)
			{
				SetDlgItemText(hWnd, IDC_CHARACTER_NAME, strBuffer1);
			}
		break;

		case IDC_TNL:
			sprintf(strBuffer1, "TNL: %d", cPlayer.GetTNL());

			GetDlgItemText(hWnd, IDC_TNL, strBuffer2, 16);
			if(stricmp(strBuffer1, strBuffer2) != 0)
			{
				SetDlgItemText(hWnd, IDC_TNL, strBuffer1);
			}
		break;

		case IDC_CURRENT_STATE:
			sprintf(strBuffer1, "State: %s", cPlayer.GetStateString(hWnd, cPlayer.GetState()));

			//** Let's prevent unnecessary flickering at expense of a few extra CPU cycles.
			GetDlgItemText(hWnd, IDC_CURRENT_STATE, strBuffer2, 16);
			if(stricmp(strBuffer1, strBuffer2) != 0)
			{
				SetDlgItemText(hWnd, IDC_CURRENT_STATE, strBuffer1); //** Set control caption.
			}
		break;

		case IDC_CURRENT_SPEED:
			sprintf(strBuffer1, "Speed: %s", cPlayer.GetSpeedString(hWnd, cPlayer.GetSpeed()));

			//** Let's prevent unneccessary flickering at the expense of a few extra CPU cycles.
			GetDlgItemText(hWnd, IDC_CURRENT_SPEED, strBuffer2, 16);
			if(stricmp(strBuffer1, strBuffer2) != 0)
			{
				SetDlgItemText(hWnd, IDC_CURRENT_SPEED, strBuffer1);
			}
		break;
	}
}

//** CheckFFXI() detects FFXI by EXE name and retrieves FFXiMain.dll module base.
BOOL CFFXI::CheckFFXI()
{
	HANDLE	hProcessSnap;
	HANDLE	hModuleSnap;
	PROCESSENTRY32	pe32;
	MODULEENTRY32	me32;

	//** Time to locate 'pol.exe'
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		//** Error		
	}
	else
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);


		if(Process32First(hProcessSnap, &pe32) == NULL)
		{
			//** Error
			CloseHandle(hProcessSnap); //** Close handle, don't forget to do this or you'll have a memory leak!
			
			return FALSE;
		}
		else
		{
			do
			{
				if(stricmp(pe32.szExeFile, cConfig.strPolExeName) == 0)
				{
					//** FFXI found, get teh pID which is stored in our PROCESSENTRY32 structure
					dwPID = pe32.th32ProcessID;

					//** Time to locate FFXiMain.dll inside pol.exe
					hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

					if(hModuleSnap == INVALID_HANDLE_VALUE)
					{
						//** Error
						CloseHandle(hProcessSnap);
						CloseHandle(hModuleSnap);

						return FALSE;
					}
					else
					{
						me32.dwSize = sizeof(MODULEENTRY32);


						if(Module32First(hModuleSnap, &me32) == NULL)
						{
							//** Error
							CloseHandle(hModuleSnap);
							return FALSE;
						}
						else
						{
							do
							{
								if(stricmp(me32.szModule, cConfig.strPolExeName) == 0)
								{
									//** Found pol.exe, let's save it's base.
									dwPolBase = (DWORD)me32.modBaseAddr;
								}
								else if(stricmp(me32.szModule, cConfig.strFFXIModName) == 0)
								{
									//** Found FFXiMain.dll!  Time to save it's base address
									dwModBase = (DWORD)me32.modBaseAddr;
								}
		
							} while(Module32Next(hModuleSnap, &me32));

							CloseHandle(hModuleSnap);	//** Close handle to module snapshot
						}
					}
				}

			} while(Process32Next(hProcessSnap, &pe32));

			CloseHandle(hProcessSnap);	//** Close handle to process snapshot
		}
	}

	if((dwPID != NULL) && (dwModBase != NULL))
	{								
		//** FFXI is running.
		return TRUE;
	}
	else if(dwPID == NULL)
	{
		//** Error, FFXI not runing.
		return FALSE;
	}
	else if(dwModBase == NULL)
	{
		//** Error, could not locate base address for FFXiMain.dll.
		return FALSE;
	}
	
	return FALSE;
}


/** Main.cpp:	NOTE: We no longer use this... was for the beta,
				but then I decided to use a more efficent and less complex method.

				InjectCode() is probably one of the most advanced techniques in this program
				where we use an empty space in the FFXI program(known as a code cave) to
				inject some of our home brewed ASM.
				
				In this instance, we use a code cave to read register 'EAX'. in which holds
				the address we need to modify FFXI states.

				Tip: ALWAYS remember to contain the original code you overwrote to jmp/call
				your code cave inside of it!

				In this case I chose to CALL my code instead of using JMP due to the fact
				that the game code is loaded dynamically and the base address is always different
				thus extra code would be needed to jmp back to the normal procedure.

BOOL InjectCode()
{
	BYTE bAssemblyCall[6] =	
							{
							0xE8, 0x4E, 0x21, 0xAC, 0xFA,			//** CALL 0x04381B0
							0x90									//** NOP
							};

	BYTE bAssemblyCode[14] =
							{
							0x89, 0x90, 0x20, 0x01, 0x00, 0x00,		//** MOV [EAX+120],EDX
							0xA3, 0xF0, 0x81, 0x43, 0x00,			//** MOV 0x4381F0,EAX
							0xC3									//** RETN
							};

	//** Open FFXI for memory writing
	cFFXI.hFFXI = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, cFFXI.dwPID);

	if(cFFXI.hFFXI != NULL)
	{
		if(WriteProcessMemory(cFFXI.hFFXI, (LPVOID)(cFFXI.dwModBase + dwCallOffset), &bAssemblyCall, sizeof(bAssemblyCall), NULL) == FALSE)
		{
			//** Error, could not write call.
			CloseHandle(cFFXI.hFFXI);
			return FALSE;
		}



		if(WriteProcessMemory(cFFXI.hFFXI, (LPVOID)(cFFXI.dwPolBase + dwCodeCave), &bAssemblyCode, sizeof(bAssemblyCode), NULL) == FALSE)
		{
			//** Error, could not write code.
			CloseHandle(cFFXI.hFFXI);
			return FALSE;
		}

		//** Our code has been injected!
		CloseHandle(cFFXI.hFFXI);
		return TRUE;
	}
	else
	{
		//** Error, could not open FFXI.
		return FALSE;
	}
	return TRUE;
}
*/

//** GetStateAddress() retrieves the address that points to your current state, which we can also modify.
DWORD CFFXI::GetStateAddress()
{
	hFFXI = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION, FALSE, dwPID); 

	if(hFFXI != NULL)
	{
		if(ReadProcessMemory(hFFXI, (LPVOID)(dwModBase + cConfig.dwPcMapOffset), &dwStateAddress, 4, NULL) != FALSE)
		{
			dwStateAddress = (dwStateAddress + 0x120);
		}
		else
		{
			return -1;
		}
	}

	CloseHandle(hFFXI);

	return dwStateAddress;
}

//** GetState() retrieves your current state information.
DWORD CPlayer::GetState()
{
	cFFXI.hFFXI = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, cFFXI.dwPID); 

	if(cFFXI.hFFXI != NULL)
	{
		if(ReadProcessMemory(cFFXI.hFFXI, (LPVOID)cFFXI.dwStateAddress, &sPlayerInfo.dwCurrentState, sizeof(BYTE), NULL) == FALSE)
		{
			return -1;
		}
	}

	CloseHandle(cFFXI.hFFXI);
	
	return sPlayerInfo.dwCurrentState;
}

//** GetSpeed() retrieves your current speed information.
DWORD CPlayer::GetSpeed()
{
	cFFXI.hFFXI = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION, FALSE, cFFXI.dwPID);

	if(cFFXI.hFFXI != NULL)
	{
		if(ReadProcessMemory(cFFXI.hFFXI, (LPVOID)(cFFXI.dwStateAddress - 0x0A), &sPlayerInfo.dwCurrentSpeed, 4, NULL) == FALSE)
		{
			return -1;
		}
	}

	CloseHandle(cFFXI.hFFXI);

	return sPlayerInfo.dwCurrentSpeed;
}

//** GetTNL() retrieves your current TNL.
DWORD CPlayer::GetTNL()
{
	cFFXI.hFFXI = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, cFFXI.dwPID);

	if(cFFXI.hFFXI != NULL)
	{
		//** Check TNL by retrieving current EXP and EXP needed to level, then find the difference.
		if((ReadProcessMemory(cFFXI.hFFXI, (LPVOID)((cFFXI.dwModBase + cConfig.dwPlayerStatsOffset) + 0x0C), &sPlayerInfo.iCurExp, sizeof(WORD), NULL) &&
			ReadProcessMemory(cFFXI.hFFXI, (LPVOID)((cFFXI.dwModBase + cConfig.dwPlayerStatsOffset) + 0x0E), &sPlayerInfo.iMaxExp, sizeof(WORD), NULL)) != FALSE)
		{
			if(sPlayerInfo.iTnlExp != (sPlayerInfo.iMaxExp - sPlayerInfo.iCurExp)) 
			{
				sPlayerInfo.iTnlExp = (sPlayerInfo.iMaxExp - sPlayerInfo.iCurExp);
			}
		}
	}

	CloseHandle(cFFXI.hFFXI);

	return sPlayerInfo.iTnlExp;
}

//** GetName() retrieves your current character name.
char *CPlayer::GetName()
{
	cFFXI.hFFXI = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION, FALSE, cFFXI.dwPID);

	if(cFFXI.hFFXI != NULL)
	{
		if(ReadProcessMemory(cFFXI.hFFXI, (LPVOID)(cFFXI.dwModBase + cConfig.dwPlayerInfoOffset), &sPlayerInfo.strCharacterName, 16, NULL) == FALSE)
		{
			return "Unknown";
		}
	}
	CloseHandle(cFFXI.hFFXI);

	return sPlayerInfo.strCharacterName;
}

//** ChangeState() modifies your current state to a user defined value.
BOOL CPlayer::ChangeState(DWORD dwState)
{	
	if((cFFXI.IsGameRunning = cFFXI.CheckFFXI()) != FALSE)
	{
		//** Open FFXI for memory writing.
		cFFXI.hFFXI = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, cFFXI.dwPID); 

		if(cFFXI.hFFXI != NULL)
		{
			if((WriteProcessMemory(cFFXI.hFFXI, (LPVOID)(cFFXI.dwStateAddress), &dwState, sizeof(dwState), NULL) &&
				WriteProcessMemory(cFFXI.hFFXI, (LPVOID)(cFFXI.dwStateAddress + 0x04), &dwState, sizeof(dwState), NULL)) == FALSE)
			{
				//** Error, could not change state.
				return FALSE;
			}

			CloseHandle(cFFXI.hFFXI);
			return TRUE;
		}
		else
		{
			//** Error, could not OpenProcess.
			return FALSE;
		}
	}
	else
	{
		//** Error, FFXI is not open.
		return FALSE;
	}
}

//** ChangeSpeed() modifies your current speed to a user defined value.
BOOL CPlayer::ChangeSpeed(DWORD dwSpeed)
{
	if((cFFXI.IsGameRunning = cFFXI.CheckFFXI()) != FALSE)
	{
		//** Open FFXI for memory writing.
		cFFXI.hFFXI = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, cFFXI.dwPID); 

		if(cFFXI.hFFXI != NULL)
		{
			if(WriteProcessMemory(cFFXI.hFFXI, (LPVOID)(cFFXI.dwStateAddress - 0x0A), &dwSpeed, sizeof(dwSpeed), NULL)  == FALSE)
			{
				//** Error, could not change speed.
				return FALSE;
			}

			CloseHandle(cFFXI.hFFXI);
			return TRUE;
		}
		else
		{
			//** Error, could not OpenProcess.
			return FALSE;
		}
	}
	else
	{
		//** Error, FFXI is not open.
		return FALSE;
	}
	return TRUE;
}

//** GetStateString() converts state information into a human readable string.
char *CPlayer::GetStateString(HWND hWnd, DWORD dwState)
{
	switch(dwState)
	{
		case STATE_NORMAL:
			strcpy(sPlayerInfo.strStateString, "Normal");
			CheckRadioButton(hWnd, IDC_STATE_NORMAL, IDC_STATE_FISH, IDC_STATE_NORMAL);
			break;
		case STATE_ATTACKING:
			strcpy(sPlayerInfo.strStateString, "Attacking");
			CheckRadioButton(hWnd, IDC_STATE_NORMAL, IDC_STATE_FISH, IDC_STATE_ATTACK);
			break;
		case STATE_DEATH:
			strcpy(sPlayerInfo.strStateString, "Dead");
			CheckRadioButton(hWnd, IDC_STATE_NORMAL, IDC_STATE_FISH, IDC_STATE_DEATH);
			break;
		case STATE_DEATH2:
			strcpy(sPlayerInfo.strStateString, "Dead");
			CheckRadioButton(hWnd, IDC_STATE_NORMAL, IDC_STATE_FISH, IDC_STATE_DEATH);
			break;
		case STATE_DEATH3:
			strcpy(sPlayerInfo.strStateString, "Dead");
			CheckRadioButton(hWnd, IDC_STATE_NORMAL, IDC_STATE_FISH, IDC_STATE_DEATH);
			break;
		case STATE_CHOCO:
			strcpy(sPlayerInfo.strStateString, "Choco");
			CheckRadioButton(hWnd, IDC_STATE_NORMAL, IDC_STATE_FISH, IDC_STATE_CHOCO);
			break;
		case STATE_FISHING:
			strcpy(sPlayerInfo.strStateString, "Fishing");
			CheckRadioButton(hWnd, IDC_STATE_NORMAL, IDC_STATE_FISH, IDC_STATE_FISH);
			break;
		case STATE_RESTING:
			strcpy(sPlayerInfo.strStateString, "Resting");
			CheckRadioButton(hWnd, IDC_STATE_NORMAL, IDC_STATE_FISH, IDC_STATE_REST);
			break;
		case STATE_SITTING:
			strcpy(sPlayerInfo.strStateString, "Sitting");
			CheckRadioButton(hWnd, IDC_STATE_NORMAL, IDC_STATE_FISH, IDC_STATE_SIT);
			break;
		default:
			strcpy(sPlayerInfo.strStateString, "Unknown");
		break;
	}

	return sPlayerInfo.strStateString;
}

//** GetStateString() converts speed information into a human readable string.
char *CPlayer::GetSpeedString(HWND hWnd, DWORD dwSpeed)
{
	switch(dwSpeed)
	{
		case SPEED_NORMAL:
			strcpy(sPlayerInfo.strSpeedString, "Normal");
			CheckRadioButton(hWnd, IDC_SPEED_NORMAL, IDC_SPEED_HYPER, IDC_SPEED_NORMAL);
		break;
		
		case SPEED_STRIDER:
			strcpy(sPlayerInfo.strSpeedString, "Strider");
			CheckRadioButton(hWnd, IDC_SPEED_NORMAL, IDC_SPEED_HYPER, IDC_SPEED_STRIDER);
		break;

		case SPEED_FLEE:
			strcpy(sPlayerInfo.strSpeedString, "Flee");
			CheckRadioButton(hWnd, IDC_SPEED_NORMAL, IDC_SPEED_HYPER, IDC_SPEED_FLEE);
		break;

		case SPEED_HYPER:
			strcpy(sPlayerInfo.strSpeedString, "Hyper");
			CheckRadioButton(hWnd, IDC_SPEED_NORMAL, IDC_SPEED_HYPER, IDC_SPEED_HYPER);
		break;
	}

	return sPlayerInfo.strSpeedString;
}

//** Main.cpp: WriteLog()
BOOL WriteLog(char strFileName[], char strLineToWrite[])
{
	strcat(strLineToWrite, "\r\n");
	for(int x = 0; strLineToWrite[x] != 0; x++);

	FILE *fpLogFile = fopen(strFileName, "a+");

	if(fpLogFile != NULL)							//** If we can open/create the file...
	{
		fwrite(strLineToWrite, x, 1, fpLogFile);	//** ... write the line
	}
	else											//** Error
	{
		return FALSE;
	}

	fclose(fpLogFile);								//** and close

	return TRUE;
}

BOOL CConfig::ReadConfigFile(char strConfigFile[])
{
	FILE *fpConfigFile;

	//** Attempt to open our configuration file, if we can't, it doesn't exist.
	fpConfigFile = fopen(strConfigFile, "r");

	if(fpConfigFile != NULL) //** File exists, read it.
	{
		for(; fgets(strBuffer1, sizeof(strBuffer1), fpConfigFile) != NULL;)
		{
			if(strBuffer1[0] == '/' && strBuffer1[1] == '/')
			{
				//** Line is a comment, ignore it.
			}
			else if(strBuffer1[0] == '/' && strBuffer1[1] == '*')
			{
				//** Comment block, ignore lines until the end.

				for(;fgets(strBuffer1, sizeof(strBuffer1), fpConfigFile) != NULL;)
				{
					if(strBuffer1[0] == '*' && strBuffer1[1] == '/')
					{
						break;
					}
				}
			}
			else if(sscanf(strBuffer1, "%[^=]= %[^\r\n]", strBuffer2, strBuffer3) == 2)
			{		
				//** Found a line, time to save the value.

				if(stricmp(strBuffer2, "PolExeName") == 0)
				{
					sprintf(strPolExeName, strBuffer3);
				}
				else if(stricmp(strBuffer2, "FFXIModName") == 0)
				{
					sprintf(strFFXIModName, strBuffer3);
				}
				else if(stricmp(strBuffer2, "PcMapOffset") == 0)
				{
					dwPcMapOffset = atoi(strBuffer3);
				}
				else if(stricmp(strBuffer2, "PlayerStatOffset") == 0)
				{
					dwPlayerStatsOffset = atoi(strBuffer3);
				}
				else if(stricmp(strBuffer2, "PlayerInfoOffset") == 0)
				{
					dwPlayerInfoOffset = atoi(strBuffer3);
				}
				else if(stricmp(strBuffer2, "HotkeySpeedNormal") == 0)
				{
					sHotkey.dwSpeedNormal = atoi(strBuffer3);
				}
				else if(stricmp(strBuffer2, "HotkeySpeedStrider") == 0)
				{
					sHotkey.dwSpeedStrider = atoi(strBuffer3);
				}
				else if(stricmp(strBuffer2, "HotkeySpeedFlee") == 0)
				{
					sHotkey.dwSpeedFlee = atoi(strBuffer3);
				}
				else if(stricmp(strBuffer2, "HotkeySpeedHyper") == 0)
				{
					sHotkey.dwSpeedHyper = atoi(strBuffer3);
				}
			}
		}
	
		//** Close file.
		fclose(fpConfigFile);

		return TRUE;
	}
	else //** Configuration doesn't exist
	{
		return FALSE;
	}
}


//** START: Constructors and deconstructors.
CConfig::CConfig()
{
	//** Default values incase a config file isn't found.
	strcpy(strPolExeName, "pol.exe");
	strcpy(strFFXIModName, "FFXiMain.dll");


	sHotkey.dwSpeedNormal		= VK_INSERT;
	sHotkey.dwSpeedStrider		= VK_DELETE;
	sHotkey.dwSpeedFlee			= VK_HOME;
	sHotkey.dwSpeedHyper		= VK_END;
}

CConfig::~CConfig()
{

}

CFFXI::CFFXI()
{
	hFFXI				= NULL;
	dwPID				= NULL;
	dwModBase			= NULL;
	dwStateAddress		= NULL;

	IsGameRunning		= FALSE;
}

CFFXI::~CFFXI()
{

}

CPlayer::CPlayer()
{

}

CPlayer::~CPlayer()
{

}
//** END
