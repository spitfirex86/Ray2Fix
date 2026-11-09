#include "framework.h"
#include "config.h"
#include "pad.h"
#include "main.h"
#include "../gitver.h"


/*
 * Global Vars
 */

BOOL g_bFixState = FALSE;
BOOL g_bFixPrevState = FALSE;

tdstDisplayMode g_stCurrentMode = { 0 };
tdeRefRate g_eRefRate = e_RR_Full;
tdeBitDepth g_eBitDepth = e_BPP_16;
BOOL g_bFullscreen = FALSE;
BOOL g_bPatchWidescreen = FALSE;
int g_DEBUG_lWaitFrame = 0;
BOOL g_bCleanupSnapShot = FALSE;

tdeErrorState g_eError = e_ES_Ok;
tdeVerifyErr g_eErrorDetails = e_VE_Ok;

char g_szReadVersion[40] = "";


/*
 * Strings
 */

char const *szUbiPath = ".\\Ubi.ini";

// Keep the original [Ray2Fix] Ubi.ini section for upstream compatibility.
static void fn_vReadFixSetting( char const *szName, char const *szFallback, char *szBuffer, DWORD dwCapacity )
{
	GetPrivateProfileString("Ray2Fix", szName, szFallback, szBuffer, dwCapacity, szUbiPath);
}


char const *a_szFilesToDelete[] = {
	"nglide_config.exe",
	"nglide_config.ini",
	"nglide_readme.txt",
	"nGlideEULA.txt",
	"3DfxSpl.dll",
	"3DfxSpl3.dll",
	"glide.dll",
	"glide3x.dll",
	"dxcfg.exe",
	"dxcfg.ini",
	"goginput.ini",
};

char const *a_szToManualDelete[] = {
	"goggame.sdb",
	"goglog.ini",
	"gog.ico",
	"EULA.txt",
	"webcache.zip",
};


/*
 * Functions
 */


void fn_vReadUbiIni( void )
{
	char szBuffer[128];

	// GLI library
	GetPrivateProfileString("Rayman2", "GLI_Dll", NULL, szBuffer, sizeof(szBuffer), szUbiPath);
	if ( !strcmp(szBuffer, "Ray2Fix") )
	{
		g_bFixState = g_bFixPrevState = TRUE;
	}

	DWORD dwWidth = 0;
	DWORD dwHeight = 0;
	DWORD dwBitDepth = 0;

	// Display mode
	GetPrivateProfileString("Rayman2", "GLI_Mode", NULL, szBuffer, sizeof(szBuffer), szUbiPath);
	int nParsed = sscanf_s(szBuffer, "1 - %d x %d x %d", &dwWidth, &dwHeight, &dwBitDepth);

	if ( nParsed >= 2 && dwWidth > 0 && dwHeight > 0 )
	{
		g_stCurrentMode.dwWidth = dwWidth;
		g_stCurrentMode.dwHeight = dwHeight;

		if ( nParsed == 3 )
			switch ( dwBitDepth )
			{
			case e_BPP_16:
				g_eBitDepth = e_BPP_16;
				break;
			case e_BPP_32:
				g_eBitDepth = e_BPP_32;
				break;
			}
	}

	// Widescreen patch
	fn_vReadFixSetting("PatchWidescreen", NULL, szBuffer, sizeof(szBuffer));
	if( strtol(szBuffer, NULL, 10) > 0 )
		g_bPatchWidescreen = TRUE;

	// DEBUG Wait frame
	fn_vReadFixSetting("Debug_WaitFrame", NULL, szBuffer, sizeof(szBuffer));
	int lResult = strtol(szBuffer, NULL, 10);
	if ( lResult > 0 )
		g_DEBUG_lWaitFrame = lResult;

	// Delete screenshots
	fn_vReadFixSetting("CleanupSnapShot", NULL, szBuffer, sizeof(szBuffer));
	if( strtol(szBuffer, NULL, 10) > 0 )
		g_bCleanupSnapShot = TRUE;

	// Version
	fn_vReadFixSetting("Version", NULL, szBuffer, sizeof(szBuffer));
	strcpy_s(g_szReadVersion, sizeof(g_szReadVersion), szBuffer);
	fn_vReadFixSetting("ForceFirstRun", NULL, szBuffer, sizeof(szBuffer));
	if( strtol(szBuffer, NULL, 10) > 0 )
		g_bFirstRun = TRUE;
}

// Migrate existing dgVoodoo settings once when the corresponding Ray2Fix
// option is absent. The legacy file is never required or modified.
void fn_vReadRendererOptions( void )
{
	char szBuffer[128];
	fn_vReadFixSetting("Fullscreen", "", szBuffer, sizeof(szBuffer));
	if ( szBuffer[0] )
		g_bFullscreen = (strtol(szBuffer, NULL, 10) != 0);
	else
	{
		GetPrivateProfileString("General", "FullScreenMode", "false",
			szBuffer, sizeof(szBuffer), ".\\dgVoodoo.conf");
		g_bFullscreen = !_stricmp(szBuffer, "true");
	}

	fn_vReadFixSetting("HalfRefRate", "", szBuffer, sizeof(szBuffer));
	if ( szBuffer[0] )
		g_eRefRate = (strtol(szBuffer, NULL, 10) != 0) ? e_RR_Half : e_RR_Full;
	else
	{
		GetPrivateProfileString("Glide", "Resolution", "",
			szBuffer, sizeof(szBuffer), ".\\dgVoodoo.conf");
		char *lpLastComma = strrchr(szBuffer, ',');
		if ( lpLastComma )
		{
			unsigned int refresh = 0;
			if ( sscanf_s(lpLastComma + 1, " refrate:%u", &refresh) == 1 && refresh == 30 )
				g_eRefRate = e_RR_Half;
		}
	}
}

void fn_vWriteUbiIni( void )
{
	char szBuffer[128];

	// GLI library path & name
	char *szDllFile = g_bFixState ? "GliFix" : "GliVd1";
	WritePrivateProfileString("Rayman2", "GLI_DllFile", szDllFile, szUbiPath);

	char *szDll = g_bFixState ? "Ray2Fix" : "Glide2";
	WritePrivateProfileString("Rayman2", "GLI_Dll", szDll, szUbiPath);

	// Doesn't really matter but write "Default" anyway
	char *szDevice = "Default";
	WritePrivateProfileString("Rayman2", "GLI_Driver", szDevice, szUbiPath);
	WritePrivateProfileString("Rayman2", "GLI_Device", szDevice, szUbiPath);

	// Display mode
	sprintf_s(szBuffer, sizeof(szBuffer), "1 - %i x %i x %i", g_stCurrentMode.dwWidth, g_stCurrentMode.dwHeight, g_eBitDepth);
	WritePrivateProfileString("Rayman2", "GLI_Mode", szBuffer, szUbiPath);

	// Tweaks - removed
	WritePrivateProfileString("Ray2Fix", "Tweaks", "0", szUbiPath);

	// Widescreen patch
	sprintf_s(szBuffer, sizeof(szBuffer), "%i", g_bPatchWidescreen);
	WritePrivateProfileString("Ray2Fix", "PatchWidescreen", szBuffer, szUbiPath);

	// GameFrame window style and Rayman engine timing (30 or 60 Hz).
	sprintf_s(szBuffer, sizeof(szBuffer), "%i", g_bFullscreen);
	WritePrivateProfileString("Ray2Fix", "Fullscreen", szBuffer, szUbiPath);
	sprintf_s(szBuffer, sizeof(szBuffer), "%i", (g_eRefRate == e_RR_Half));
	WritePrivateProfileString("Ray2Fix", "HalfRefRate", szBuffer, szUbiPath);

	// DEBUG wait frame
	sprintf_s(szBuffer, sizeof(szBuffer), "%d", g_DEBUG_lWaitFrame);
	WritePrivateProfileString("Ray2Fix", "Debug_WaitFrame", szBuffer, szUbiPath);

	// Delete screenshots
	sprintf_s(szBuffer, sizeof(szBuffer), "%i", g_bCleanupSnapShot);
	WritePrivateProfileString("Ray2Fix", "CleanupSnapShot", szBuffer, szUbiPath);

	// Version
	WritePrivateProfileString("Ray2Fix", "Version", C_GIT_VER, szUbiPath);
	WritePrivateProfileString("Ray2Fix", "ForceFirstRun", NULL, szUbiPath);
}

void fn_vSoftCleanUp( void )
{
	for ( DWORD i = 0; i < ARRAYSIZE(a_szFilesToDelete); i++ )
	{
		char szFilePath[MAX_PATH];
		sprintf_s(szFilePath, MAX_PATH, ".\\%s", a_szFilesToDelete[i]);

		DeleteFile(szFilePath);
	}
}

void fn_vManualCleanUp( void )
{
	/* do regular cleanup first */
	fn_vSoftCleanUp();

	for ( DWORD i = 0; i < ARRAYSIZE(a_szToManualDelete); i++ )
	{
		char szFilePath[MAX_PATH];
		sprintf_s(szFilePath, MAX_PATH, ".\\%s", a_szToManualDelete[i]);

		DeleteFile(szFilePath);
	}
}

void CFG_fn_vRead( void )
{
	fn_vReadUbiIni();
	fn_vReadRendererOptions();
	PAD_fn_vRead();
}

void CFG_fn_vWrite( void )
{
	fn_vWriteUbiIni();
	PAD_fn_vWrite();
}

void CFG_fn_vVerify( void )
{
	if ( (GetFileAttributes(".\\nglide_config.exe") != INVALID_FILE_ATTRIBUTES) || 
		 (GetFileAttributes(".\\dxcfg.exe") != INVALID_FILE_ATTRIBUTES) )
	{
		// Delete unnecessary nGlide files
		fn_vSoftCleanUp();
	}

	if ( GetFileAttributes(szUbiPath) == INVALID_FILE_ATTRIBUTES )
	{
		g_eErrorDetails |= e_VE_FilesMissing | e_VE_UbiMissing;
	}

	if ( GetFileAttributes(".\\glide2x.dll") == INVALID_FILE_ATTRIBUTES )
	{
		g_eError |= e_ES_FixError;
		g_eErrorDetails |= e_VE_FilesMissing | e_VE_GameFrameMissing;
	}

	if ( GetFileAttributes(".\\DLL\\GliVd1Vf.dll") == INVALID_FILE_ATTRIBUTES )
	{
		BOOL bAutoFixed = FALSE;
		// is it in a subdir? (UbiConnect)
		if ( GetFileAttributes(".\\DLL\\Glide\\GliVd1Vf.dll") != INVALID_FILE_ATTRIBUTES )
			bAutoFixed = MoveFileEx(".\\DLL\\Glide\\GliVd1Vf.dll", ".\\DLL\\GliVd1Vf.dll", MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH);

		if ( !bAutoFixed )
		{
			g_eError |= e_ES_GameError;
			g_eErrorDetails |= e_VE_FilesMissing | e_VE_GlideMissing;
		}
	}

	if ( GetFileAttributes(".\\DLL\\GliFixVf.dll") == INVALID_FILE_ATTRIBUTES )
	{
		g_eError |= e_ES_FixError;
		g_eErrorDetails |= e_VE_FilesMissing | e_VE_FixMissing;
	}

	if ( GetFileAttributes(".\\dinput.dll") == INVALID_FILE_ATTRIBUTES )
	{
		g_eError |= e_ES_FixError;
		g_eErrorDetails |= e_VE_FilesMissing | e_VE_DinputMissing;
	}

	if ( GetFileAttributes(g_szXidiPath) == INVALID_FILE_ATTRIBUTES )
	{
		g_eError |= e_ES_Warning;
		g_eErrorDetails |= e_VE_FilesMissing | e_VE_XidiMissing;
	}
}
