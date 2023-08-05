/*
		hAx State Changer for FFXI v1.1 
		by Zak Farrington alias fritz <fritz@hax-studios.net>

		Copyright (C) 2005 hAx Studios Ltd.
		All rights reserved.

		http://hax-studios.net
*/

//** START: Global functions.
BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnInitDlg(HWND hWnd);
void OnTimer(HWND hWnd, UINT nID);
BOOL IsDebuggerPresent();
void UpdateInformation(HWND hWnd, DWORD dwItem);
BOOL WriteLog(char *strFileName, char* strLineToWrite);
//** END


//** START: Global variables.
HINSTANCE	g_hInstance;
char		strMessage[128];
char		strBuffer1[128];
char		strBuffer2[128];
char		strBuffer3[128];
char		strLogFile[128] = "hAx.log";
DWORD		iKey;
//** END


//** START: CFFXI Class.
class CFFXI
{
public:
	CFFXI();
	~CFFXI();

	//** START: Functions.
	BOOL	CheckFFXI();
	DWORD	GetStateAddress();
	//** END

	//** START: Variables.
	HANDLE	hFFXI;

	DWORD	dwPID;
	DWORD	dwPolBase;
	DWORD	dwModBase;
	DWORD	dwStateAddress;

	BOOL	IsGameRunning;
	//** END
};
//** END


//** START: CPlayer class.
class CPlayer
{
public:
	CPlayer();
	~CPlayer();

	//** START: Functions.
	DWORD		GetState();
	DWORD		GetSpeed();
	DWORD		GetTNL();
	char		*GetName();
	char		*GetStateString(HWND hWnd, DWORD dwState);
	char		*GetSpeedString(HWND hWnd, DWORD dwSpeed);
	BOOL		ChangeState(DWORD dwState);
	BOOL		ChangeSpeed(DWORD dwSpeed);
	//** END

	//** START: Variables.
	struct 
	{
		char	strCharacterName[32];
		char	strStateString[16];
		char	strSpeedString[16];
		DWORD	dwCurrentState;
		DWORD	dwCurrentSpeed;
		int		iCurExp;
		int		iMaxExp;
		int		iTnlExp;
	} sPlayerInfo;
	//** END
};
//** END


//** START: CConfig class.
class CConfig
{
public:
	CConfig();
	~CConfig();

	//** START: Functions.
	ReadConfigFile(char strConfigFile[]);
	//** END

	//** START: Variables.
	char	strBaseDirectory[128];
	char	strConfigFile[128];

	char	strPolExeName[16];
	char	strFFXIModName[16];
	DWORD	dwPcMapOffset;
	DWORD	dwPlayerStatsOffset;
	DWORD	dwPlayerInfoOffset;

	struct 
	{
		DWORD	dwSpeedNormal;
		DWORD	dwSpeedStrider;
		DWORD	dwSpeedFlee;
		DWORD	dwSpeedHyper;
	} sHotkey;
	//** END
};
//** END


//** START: Timers.
#define	CHECK_STATE_ADDRESS_TIMER	5001
#define	CHECK_STATE_TIMER			5002
#define	CHECK_NAME_TIMER			5003
#define CHECK_TNL_TIMER				5004
#define CHECK_HOTKEY_TIMER			5005
#define CHECK_SPEED_TIMER			5006
//** END


//** START: FFXI state flags.
#define	STATE_NORMAL				0x00
#define STATE_ATTACKING				0x01
#define STATE_DEATH					0x02
#define STATE_DEATH2				0x03
#define	STATE_DEATH3				0x04
#define	STATE_CHOCO					0x05
#define STATE_FISHING				0x06
#define	STATE_RESTING				0x21
#define	STATE_SITTING				0x2F
//** END


//** START: FFXI speed flags.
#define SPEED_NORMAL				0x4080
#define SPEED_STRIDER				0x4090
#define SPEED_FLEE					0x4100
#define	SPEED_HYPER					0x7080