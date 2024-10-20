#include "framework.h"
#include <shellapi.h>
#include <time.h>
#include "config.h"
#include "devinfo.h"


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

char const *szDegePath = ".\\dgVoodoo.conf";
char const *szUbiPath = ".\\Ubi.ini";


/*
 * Global Vars
 */

tdstDisplayMode CFG_stActualDispMode = { 0 };
float CFG_xActualRatio = 0;

tdstDisplayMode CFG_stDispMode = { 0 };
BOOL CFG_bHalfRefRate = FALSE;
int CFG_DEBUG_lWaitFrame = 0;

BOOL CFG_bIsMainModuleR2 = FALSE;
BOOL CFG_bIsFixEnabled = TRUE;

BOOL CFG_bPatchWidescreen = FALSE;
BOOL CFG_bIsWidescreen = FALSE;

char CFG_szModuleName[MAX_PATH] = "";
char CFG_szModuleDate[20] = "";

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

BOOL fn_bReadDispModeFromDegeConfig( tdstDisplayMode *lpDst )
{
	char szBuffer[128];
	char *szSection = "Glide";

	GetPrivateProfileString(szSection, "Resolution", NULL, szBuffer, sizeof(szBuffer), szDegePath);

	int nParsed = sscanf_s(szBuffer, "h:%u, v:%u", &lpDst->dwWidth, &lpDst->dwHeight);

	if ( nParsed < 2 || lpDst->dwWidth <= 0 || lpDst->dwHeight <= 0 )
	{
		// Try the other format before giving up
		nParsed = sscanf_s(szBuffer, "%dx%d", &lpDst->dwWidth, &lpDst->dwHeight);

		if ( nParsed < 2 || lpDst->dwWidth <= 0 || lpDst->dwHeight <= 0 )
			return FALSE;
	}

	return TRUE;
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
		// Try to parse resolution from dgvoodoo.conf
		if ( !fn_bReadDispModeFromDegeConfig(&stFromConfig) )
		{
			// Give up and apply sane defaults (1024x768)
			stFromConfig = stDefaultRes;
		}
	}
	CFG_stActualDispMode = stFromConfig;
}

void fn_vReadFixConfig( void )
{
	char szBuffer[128];

	// Widescreen patch
	GetPrivateProfileString("Ray2Fix", "PatchWidescreen", "0", szBuffer, sizeof(szBuffer), szUbiPath);
	if( strtol(szBuffer, NULL, 10) > 0 )
		CFG_bPatchWidescreen = TRUE;

	// Refresh rate
	GetPrivateProfileString("Ray2Fix", "HalfRefRate", "0", szBuffer, sizeof(szBuffer), szUbiPath);
	if( strtol(szBuffer, NULL, 10) > 0 )
		CFG_bHalfRefRate = TRUE;

	// DEBUG Wait frame
	GetPrivateProfileString("Ray2Fix", "Debug_WaitFrame", "0", szBuffer, sizeof(szBuffer), szUbiPath);
	int lResult = strtol(szBuffer, NULL, 10);
	if ( lResult > 0 )
		CFG_DEBUG_lWaitFrame = lResult;
}

void CFG_fn_vInitGlobals( void )
{
	CFG_bIsMainModuleR2 = CFG_fn_bDetermineMainModule();

	fn_vReadR2Config();
	fn_vReadFixConfig();

	unsigned int ratio = 100 * CFG_stActualDispMode.dwHeight / CFG_stActualDispMode.dwWidth;
	CFG_bIsWidescreen = (ratio <= 73);

	tdstDisplayMode *lpGlideMode = fn_p_stGetClosestGlideMode(&CFG_stDispMode);
	CFG_stDispMode = *lpGlideMode;

	CFG_xActualRatio = (float)CFG_stActualDispMode.dwHeight / (float)CFG_stActualDispMode.dwWidth;
}

BOOL CFG_fn_bOpenConfigTool( void )
{
	int lResult = (int)ShellExecute(NULL, NULL, ".\\R2FixCfg.exe", NULL, NULL, SW_SHOW);

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
