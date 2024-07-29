#include "framework.h"
#include <shellapi.h>
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
tdstDisplayMode CFG_stDispMode = { 0 };
BOOL CFG_bHalfRefRate = FALSE;

BOOL CFG_bIsMainModuleR2 = FALSE;
BOOL CFG_bIsFixEnabled = TRUE;

BOOL CFG_bIsWidescreen = FALSE;

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

	// Refresh rate
	GetPrivateProfileString("Ray2Fix", "HalfRefRate", "0", szBuffer, sizeof(szBuffer), szUbiPath);
	if( strtol(szBuffer, NULL, 10) > 0 )
	{
		CFG_bHalfRefRate = TRUE;
	}
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
	GetModuleFileName(GetModuleHandle(NULL), szModuleName, MAX_PATH);
	char *pBaseName = strrchr(szModuleName, '\\') + 1;

	if ( !strcmp(pBaseName, "Rayman2.exe") )
		return TRUE;

	return FALSE;
}
