#include "framework.h"
#include <shellapi.h>
#include "config.h"
#include "devinfo.h"
#include "shared.h"

#define MAX_GLIDE_MODES 8

DISP_MODE a_dmGlideModes[MAX_GLIDE_MODES] = {
	{ 320, 200 },
	{ 400, 300 },
	{ 512, 384 },
	{ 640, 480 },
	{ 800, 600 },
	{ 1024, 768 },
	{ 1280, 1024 },
	{ 1600, 1200 },
};

DISP_MODE dmDefaultRes = { 1024, 768 };

char *szDegePath = ".\\dgVoodoo.conf";
char *szUbiPath = ".\\Ubi.ini";

//
// GLOBAL VARS
//

DISP_MODE CFG_dmDispMode = { 0, 0 };

BOOL CFG_bIsMainModuleR2 = FALSE;
BOOL CFG_bIsFixEnabled = TRUE;

BOOL CFG_bTainted = FALSE;
TWEAKS CFG_lTweaks = TWK_NO_TWEAKS;

//
// FUNCTIONS
//

// Returns largest Glide resolution smaller than the provided display mode.
DISP_MODE* fn_lpdmGetClosestGlideMode( DISP_MODE *lpMode )
{
	for ( int i = MAX_GLIDE_MODES - 1; i >= 0; i-- )
	{
		DISP_MODE *lpGlideMode = &a_dmGlideModes[i];

		if ( lpGlideMode->dwWidth <= lpMode->dwWidth && lpGlideMode->dwHeight <= lpMode->dwHeight )
		{
			return lpGlideMode;
		}
	}

	// Ruh roh, somehow even the smallest Glide mode is larger than lpMode.
	// That's probably an error, so let's just return the "default" value (1024x768).

	return &dmDefaultRes;
}

BOOL fn_bReadDispModeFromDegeConfig( DISP_MODE *lpDst )
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
	char *szSection = "Rayman2";

	DISP_MODE dmFromConfig = { 0 };

	// GLI library
	GetPrivateProfileString(szSection, "GLI_Dll", NULL, szBuffer, sizeof(szBuffer), szUbiPath);

	if ( strcmp(szBuffer, GLI_szName) != 0 )
	{
		// Ray2Fix is loaded but not enabled, set the global var to FALSE
		// This will show a prompt in GLI_DRV_fn_lGetAllDisplayConfig.
		CFG_bIsFixEnabled = FALSE;
	}

	// Display mode (resolution)
	GetPrivateProfileString(szSection, "GLI_Mode", NULL, szBuffer, sizeof(szBuffer), szUbiPath);

	int nParsed = sscanf_s(szBuffer, "1 - %u x %u", &dmFromConfig.dwWidth, &dmFromConfig.dwHeight);

	if ( nParsed < 2 || dmFromConfig.dwWidth <= 0 || dmFromConfig.dwHeight <= 0 )
	{
		// Try to parse resolution from dgvoodoo.conf
		if ( !fn_bReadDispModeFromDegeConfig(&dmFromConfig) )
		{
			// Give up and apply sane defaults (1024x768)
			dmFromConfig = dmDefaultRes;
		}
	}
	CFG_dmDispMode = dmFromConfig;
}

void fn_vReadFixConfig( void )
{
	char szBuffer[128];
	char *szSection = "Ray2Fix";

	// Tweaks
	GetPrivateProfileString(szSection, "Tweaks", "0", szBuffer, sizeof(szBuffer), szUbiPath);

	CFG_lTweaks = strtol(szBuffer, NULL, 10);
	CFG_bTainted = CFG_lTweaks > 0;
}

void CFG_vInitGlobals( void )
{
	CFG_bIsMainModuleR2 = CFG_bDetermineMainModule();

	fn_vReadR2Config();
	fn_vReadFixConfig();

	DISP_MODE *lpGlideMode = fn_lpdmGetClosestGlideMode(&CFG_dmDispMode);
	CFG_dmDispMode = *lpGlideMode;
}

BOOL CFG_bOpenConfigTool( void )
{
	int lResult = (int)ShellExecute(NULL, NULL, ".\\R2FixCfg.exe", NULL, NULL, SW_SHOW);

	if ( lResult > 32 )
		return TRUE;

	return FALSE;
}

BOOL CFG_bDetermineMainModule( void )
{
	char szModuleName[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL), szModuleName, MAX_PATH);
	char *pBaseName = strrchr(szModuleName, '\\') + 1;

	if ( !strcmp(pBaseName, "Rayman2.exe") )
	{
		return TRUE;
	}

	return FALSE;
}
