#include "framework.h"
#include <shellapi.h>
#include "config.h"
#include "devinfo.h"
#include "shared.h"

//
// GLOBAL VARS
//

int CFG_lDispWidth = 0;
int CFG_lDispHeight = 0;

BOOL CFG_bIsMainModuleR2 = FALSE;
BOOL CFG_bIsFixEnabled = TRUE;

BOOL CFG_bTainted = FALSE;
TWEAKS CFG_lTweaks = TWK_NO_TWEAKS;

//
// FUNCTIONS
//

char *szDegePath = ".\\dgVoodoo.conf";
char *szUbiPath = ".\\Ubi.ini";

BOOL fn_bReadDispModeFromDegeConfig( void )
{
	char szBuffer[128];
	char *szSection = "Glide";

	GetPrivateProfileString(szSection, "Resolution", NULL, szBuffer, sizeof(szBuffer), szDegePath);

	int nParsed = sscanf_s(szBuffer, "h:%d, v:%d", &CFG_lDispWidth, &CFG_lDispHeight);

	if ( nParsed < 2 || CFG_lDispWidth <= 0 || CFG_lDispHeight <= 0 )
	{
		// Try the other format before giving up
		nParsed = sscanf_s(szBuffer, "%dx%d", &CFG_lDispWidth, &CFG_lDispHeight);

		if ( nParsed < 2 || CFG_lDispWidth <= 0 || CFG_lDispHeight <= 0 )
			return FALSE;
	}

	return TRUE;
}

void fn_vReadR2Config( void )
{
	char szBuffer[128];
	char *szSection = "Rayman2";

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

	int nParsed = sscanf_s(szBuffer, "1 - %d x %d", &CFG_lDispWidth, &CFG_lDispHeight);

	if ( nParsed < 2 || CFG_lDispWidth <= 0 || CFG_lDispHeight <= 0 )
	{
		// Try to parse resolution from dgvoodoo.conf
		if ( !fn_bReadDispModeFromDegeConfig() )
		{
			// Give up and apply sane defaults
			CFG_lDispWidth = 1024;
			CFG_lDispHeight = 768;
		}
	}
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
