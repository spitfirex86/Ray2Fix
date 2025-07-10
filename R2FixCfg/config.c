#include "framework.h"
#include "config.h"
#include "pad.h"


/*
 * Global Vars
 */

BOOL g_bFixState = FALSE;
BOOL g_bFixPrevState = FALSE;

tdstDisplayMode g_stCurrentMode = { 0 };
tdeRefRate g_eRefRate = e_RR_Full;
BOOL g_bForceVsync = FALSE;
BOOL g_bFullscreen = FALSE;
BOOL g_bPatchWidescreen = FALSE;
int g_DEBUG_lWaitFrame = 0;

tdeErrorState g_eError = e_ES_Ok;
tdeVerifyErr g_eErrorDetails = e_VE_Ok;


/*
 * Strings
 */

char const *szDegePath = ".\\dgVoodoo.conf";
char const *szUbiPath = ".\\Ubi.ini";

char const *a_szFilesToDelete[] = {
	"nglide_config.exe",
	"nglide_readme.txt",
	"nGlideEULA.txt",
	"3DfxSpl.dll",
	"3DfxSpl3.dll",
	"glide.dll",
	"glide3x.dll"
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

	// Display mode
	GetPrivateProfileString("Rayman2", "GLI_Mode", NULL, szBuffer, sizeof(szBuffer), szUbiPath);
	int nParsed = sscanf_s(szBuffer, "1 - %d x %d", &dwWidth, &dwHeight);

	if ( nParsed == 2 && dwWidth > 0 && dwHeight > 0 )
	{
		g_stCurrentMode.dwWidth = dwWidth;
		g_stCurrentMode.dwHeight = dwHeight;
	}

	// Widescreen patch
	GetPrivateProfileString("Ray2Fix", "PatchWidescreen", NULL, szBuffer, sizeof(szBuffer), szUbiPath);
	if( strtol(szBuffer, NULL, 10) > 0 )
		g_bPatchWidescreen = TRUE;

	// DEBUG Wait frame
	GetPrivateProfileString("Ray2Fix", "Debug_WaitFrame", NULL, szBuffer, sizeof(szBuffer), szUbiPath);
	int lResult = strtol(szBuffer, NULL, 10);
	if ( lResult > 0 )
		g_DEBUG_lWaitFrame = lResult;
}

void fn_vReadDegeIni( void )
{
	char szBuffer[128];

	DWORD dwRefRate = 0;

	// Refresh rate
	GetPrivateProfileString("Glide", "Resolution", NULL, szBuffer, sizeof(szBuffer), szDegePath);
	char *lpLastComma = strrchr(szBuffer, ',');

	if ( lpLastComma != NULL )
	{
		int nParsed = sscanf_s(++lpLastComma, " refrate:%d", &dwRefRate);

		if ( nParsed == 1 && dwRefRate > 0 )
			g_eRefRate = dwRefRate;
	}

	// Force VSync
	GetPrivateProfileString("Glide", "ForceVerticalSync", NULL, szBuffer, sizeof(szBuffer), szDegePath);
	if ( !strcmp(szBuffer, "true") )
		g_bForceVsync = TRUE;

	// Fullscreen mode
	GetPrivateProfileString("General", "FullScreenMode", NULL, szBuffer, sizeof(szBuffer), szDegePath);
	if ( !strcmp(szBuffer, "true") )
		g_bFullscreen = TRUE;
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
	sprintf_s(szBuffer, sizeof(szBuffer), "1 - %i x %i x 32", g_stCurrentMode.dwWidth, g_stCurrentMode.dwHeight);
	WritePrivateProfileString("Rayman2", "GLI_Mode", szBuffer, szUbiPath);

	// Tweaks - removed
	WritePrivateProfileString("Ray2Fix", "Tweaks", "0", szUbiPath);

	// Widescreen patch
	sprintf_s(szBuffer, sizeof(szBuffer), "%i", g_bPatchWidescreen);
	WritePrivateProfileString("Ray2Fix", "PatchWidescreen", szBuffer, szUbiPath);

	// Refresh rate
	sprintf_s(szBuffer, sizeof(szBuffer), "%i", (g_eRefRate == e_RR_Half));
	WritePrivateProfileString("Ray2Fix", "HalfRefRate", szBuffer, szUbiPath);

	// DEBUG wait frame
	sprintf_s(szBuffer, sizeof(szBuffer), "%d", g_DEBUG_lWaitFrame);
	WritePrivateProfileString("Ray2Fix", "Debug_WaitFrame", szBuffer, szUbiPath);
}

void fn_vWriteDegeIni( void )
{
	char szBuffer[128];

	// These values should never change, but write them anyway in case the user messes up the config
	WritePrivateProfileString("General", "ProgressiveScanlineOrder", "true", szDegePath);
	WritePrivateProfileString("General", "EnumerateRefreshRates", "true", szDegePath);
	WritePrivateProfileString("General", "ScalingMode", "stretched_ar", szDegePath);
	WritePrivateProfileString("General", "KeepWindowAspectRatio", "true", szDegePath);
	WritePrivateProfileString("Glide", "VideoCard", "voodoo_2", szDegePath);
	WritePrivateProfileString("Glide", "OnboardRAM", "12", szDegePath);
	WritePrivateProfileString("Glide", "MemorySizeOfTMU", "4096", szDegePath);
	WritePrivateProfileString("Glide", "NumberOfTMUs", "2", szDegePath);
	WritePrivateProfileString("Glide", "EnableGlideGammaRamp", "true", szDegePath);
	WritePrivateProfileString("Glide", "EnableInactiveAppState", "false", szDegePath);
	/* this one is necessary to avoid refresh rate affecting timers/screenshot lag... */
	WritePrivateProfileString("GeneralExt", "FPSLimit", "60", szDegePath);

	// Display mode & refresh rate
	sprintf_s(szBuffer, sizeof(szBuffer), "h:%i, v:%i, refrate:%i",
	          g_stCurrentMode.dwWidth, g_stCurrentMode.dwHeight, g_eRefRate);
	WritePrivateProfileString("Glide", "Resolution", szBuffer, szDegePath);

	// Force VSync
	char const *szVsync = g_bForceVsync ? "true" : "false";
	WritePrivateProfileString("Glide", "ForceVerticalSync", szVsync, szDegePath);

	// Fullscreen mode
	char const *szFullScreen = g_bFullscreen ? "true" : "false";
	WritePrivateProfileString("General", "FullScreenMode", szFullScreen, szDegePath);
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
	fn_vReadDegeIni();
	PAD_fn_vRead();
}

void CFG_fn_vWrite( void )
{
	fn_vWriteUbiIni();
	fn_vWriteDegeIni();
	PAD_fn_vWrite();
}

void CFG_fn_vVerify( void )
{
	if ( GetFileAttributes(".\\nglide_config.exe") != INVALID_FILE_ATTRIBUTES )
	{
		// Delete unnecessary nGlide files
		fn_vSoftCleanUp();
	}

	if ( GetFileAttributes(szUbiPath) == INVALID_FILE_ATTRIBUTES )
	{
		g_eErrorDetails |= e_VE_FilesMissing | e_VE_UbiMissing;
	}

	if ( GetFileAttributes(szDegePath) == INVALID_FILE_ATTRIBUTES )
	{
		g_eError |= e_ES_FixError;
		g_eErrorDetails |= e_VE_FilesMissing | e_VE_DegeMissing;
	}

	if ( GetFileAttributes(".\\DLL\\GliVd1Vf.dll") == INVALID_FILE_ATTRIBUTES )
	{
		g_eError |= e_ES_GameError;
		g_eErrorDetails |= e_VE_FilesMissing | e_VE_GlideMissing;
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
