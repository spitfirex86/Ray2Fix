#include "framework.h"
#include <shellapi.h>
#include <time.h>
#include "config.h"
#include "devinfo.h"
#include "../gitver.h"


#define C_MaxGlideModes 8

tdstDisplayMode a_stGlideModes[C_MaxGlideModes] = {
	{ 320, 200 },
	{ 400, 300 },
	{ 512, 384 },
	{ 640, 480 },
	{ 800, 600 },
	{ 1024, 768 },
	{ 1280, 1024 },
	{ 1600, 1200 },
};

tdstDisplayMode stDefaultRes = { 1024, 768 };

char const *szUbiPath = ".\\Ubi.ini";


/*
 * Global Vars
 */

tdstDisplayMode CFG_stActualDispMode = { 0 };
float CFG_xActualRatio = 0;

tdstDisplayMode CFG_stDispMode = { 0 };
BOOL CFG_bHalfRefRate = FALSE;
int CFG_DEBUG_lWaitFrame = 0;
BOOL CFG_bCleanupSnapShot = FALSE;
BOOL CFG_bGameFrameFullScreen = FALSE;

BOOL CFG_bIsMainModuleR2 = FALSE;
BOOL CFG_bIsFixEnabled = TRUE;

BOOL CFG_bPatchWidescreen = FALSE;
BOOL CFG_bIsWidescreen = FALSE;

char CFG_szModuleName[MAX_PATH] = "";
char CFG_szModuleDate[20] = "";

BOOL CFG_bSomeConfigFilesAreOutdated = FALSE;

char CFG_szReadVersion[40] = "";
BOOL CFG_bFirstRun = FALSE;

/*
 * Functions
 */

// Returns largest Glide resolution smaller than the provided display mode.
tdstDisplayMode * fn_p_stGetClosestGlideMode( tdstDisplayMode *lpMode )
{
	for ( int i = C_MaxGlideModes - 1; i >= 0; i-- )
	{
		tdstDisplayMode *lpGlideMode = &a_stGlideModes[i];

		if ( lpGlideMode->dwWidth <= lpMode->dwWidth && lpGlideMode->dwHeight <= lpMode->dwHeight )
		{
			return lpGlideMode;
		}
	}

	// Ruh roh, somehow even the smallest Glide mode is larger than lpMode.
	// That's probably an error, so let's just return the "default" value (1024x768).

	return &stDefaultRes;
}

void fn_vReadR2Config( void )
{
	char szBuffer[128];
	tdstDisplayMode stFromConfig = { 0 };

	// GLI library
	GetPrivateProfileString("Rayman2", "GLI_Dll", NULL, szBuffer, sizeof(szBuffer), szUbiPath);

	if ( strcmp(szBuffer, GLI_szName) != 0 )
	{
		// Ray2Fix is loaded but not enabled, set the global var to FALSE
		// This will show a prompt in GLI_DRV_fn_lGetAllDisplayConfig.
		CFG_bIsFixEnabled = FALSE;
	}

	// Display mode (resolution)
	GetPrivateProfileString("Rayman2", "GLI_Mode", NULL, szBuffer, sizeof(szBuffer), szUbiPath);

	int nParsed = sscanf_s(szBuffer, "1 - %u x %u", &stFromConfig.dwWidth, &stFromConfig.dwHeight);

	if ( nParsed < 2 || stFromConfig.dwWidth <= 0 || stFromConfig.dwHeight <= 0 )
	{
		// Use a safe default when Ubi.ini has no usable game mode.
		stFromConfig = stDefaultRes;
	}
	CFG_stActualDispMode = stFromConfig;
}

// Continue using the upstream [Ray2Fix] section in Ubi.ini.
static void fn_vReadFixSetting( char const *szName, char const *szFallback, char *szBuffer, DWORD dwCapacity )
{
	GetPrivateProfileString("Ray2Fix", szName, szFallback, szBuffer, dwCapacity, szUbiPath);
}

void fn_vReadFixConfig( void )
{
	char szBuffer[128];

	// FIRST RUN
	fn_vReadFixSetting("ForceFirstRun", NULL, szBuffer, sizeof(szBuffer));
	if( strtol(szBuffer, NULL, 10) > 0 )
		CFG_bFirstRun = TRUE;

	// Version
	fn_vReadFixSetting("Version", NULL, szBuffer, sizeof(szBuffer));
	strcpy_s(CFG_szReadVersion, sizeof(CFG_szReadVersion), szBuffer);

	// Widescreen patch
	fn_vReadFixSetting("PatchWidescreen", "0", szBuffer, sizeof(szBuffer));
	if( strtol(szBuffer, NULL, 10) > 0 )
		CFG_bPatchWidescreen = TRUE;

	// Refresh rate
	fn_vReadFixSetting("HalfRefRate", "0", szBuffer, sizeof(szBuffer));
	if( strtol(szBuffer, NULL, 10) > 0 )
		CFG_bHalfRefRate = TRUE;

	// DEBUG Wait frame
	fn_vReadFixSetting("Debug_WaitFrame", "0", szBuffer, sizeof(szBuffer));
	int lResult = strtol(szBuffer, NULL, 10);
	if ( lResult > 0 )
		CFG_DEBUG_lWaitFrame = lResult;

	// Delete screenshots
	fn_vReadFixSetting("CleanupSnapShot", "0", szBuffer, sizeof(szBuffer));
	if( strtol(szBuffer, NULL, 10) > 0 )
		CFG_bCleanupSnapShot = TRUE;

	// Renderer selection now lives in Ubi.ini, not dgVoodoo.conf.
	fn_vReadFixSetting("Fullscreen", "0", szBuffer, sizeof(szBuffer));
	CFG_bGameFrameFullScreen = (strtol(szBuffer, NULL, 10) != 0);
}

void CFG_fn_vInitGlobals( void )
{
	CFG_bIsMainModuleR2 = CFG_fn_bDetermineMainModule();

	fn_vReadR2Config();
	fn_vReadFixConfig();

	if ( _stricmp(CFG_szReadVersion, C_GIT_VER) != 0 )
	{
		/* version changed ? */
		CFG_bFirstRun = TRUE;
	}

	unsigned int ratio = 100 * CFG_stActualDispMode.dwHeight / CFG_stActualDispMode.dwWidth;
	CFG_bIsWidescreen = (ratio <= 73);

	tdstDisplayMode *lpGlideMode = fn_p_stGetClosestGlideMode(&CFG_stActualDispMode);
	CFG_stDispMode = *lpGlideMode;

	CFG_xActualRatio = (float)CFG_stActualDispMode.dwHeight / (float)CFG_stActualDispMode.dwWidth;

	// The underlying Rayman GLI driver may load glide2x.dll as soon as it is
	// loaded itself. Configure GameFrame BEFORE IMP_fn_vLoadGliLibrary().
	if ( CFG_bIsMainModuleR2 || !_stricmp(CFG_szModuleName, "MainWinR.exe") )
	{
		char szOutputSize[48];
		sprintf_s(szOutputSize, sizeof(szOutputSize), "%lux%lu",
			CFG_stActualDispMode.dwWidth, CFG_stActualDispMode.dwHeight);
		SetEnvironmentVariableA("GLIDE_SIZE", szOutputSize);
		SetEnvironmentVariableA("RAY2FIX_GLIDE_WINDOWED", CFG_bGameFrameFullScreen ? "0" : "1");
		SetEnvironmentVariableA("RAY2FIX_GAMEFRAME", "1");
	}
}

BOOL CFG_fn_bOpenConfigTool( char const *szParams )
{
	int lResult = (int)ShellExecute(NULL, NULL, ".\\R2FixCfg.exe", szParams, NULL, SW_SHOW);

	if ( lResult > 32 )
		return TRUE;

	return FALSE;
}

BOOL CFG_fn_bDetermineMainModule( void )
{
	char szModuleName[MAX_PATH];
	HMODULE hModule = GetModuleHandle(NULL);

	GetModuleFileName(hModule, szModuleName, MAX_PATH);
	char *pBaseName = strrchr(szModuleName, '\\') + 1;
	strcpy_s(CFG_szModuleName, sizeof(CFG_szModuleName), pBaseName);

	if ( !strcmp(pBaseName, "Rayman2.exe") )
		return TRUE;

	// query the nt header
	unsigned char *pBase = (unsigned char *)hModule;
	IMAGE_DOS_HEADER *pDosHeader = (IMAGE_DOS_HEADER *)pBase;
	IMAGE_NT_HEADERS *pNtHeader = (IMAGE_NT_HEADERS *)(pBase + pDosHeader->e_lfanew);

	// module date
	time_t timestamp = pNtHeader->FileHeader.TimeDateStamp;
	struct tm const *pTime = gmtime(&timestamp);
	strftime(CFG_szModuleDate, sizeof(CFG_szModuleDate), "%F", pTime);

	// module name
	if ( !pNtHeader->OptionalHeader.NumberOfRvaAndSizes )
		return FALSE;

	IMAGE_DATA_DIRECTORY *pExpDir = &pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	IMAGE_EXPORT_DIRECTORY *pExports = (IMAGE_EXPORT_DIRECTORY *)(pBase + pExpDir->VirtualAddress);
	if ( !pExpDir->Size || !pExports->Name )
		return FALSE;

	// got original module name
	char const *pName = (char *)(pBase + pExports->Name);
	strcpy_s(CFG_szModuleName, sizeof(CFG_szModuleName), pName);

	if ( !_stricmp(pName, "MainWinf.exe") ) // retail RII
		return TRUE;

	return FALSE;
}
