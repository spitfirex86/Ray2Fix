#include "framework.h"
#include "config.h"


/*
 * Global Vars
 */

BOOL g_bFixState = FALSE;
BOOL g_bFixPrevState = FALSE;

tdeTweaks g_eTweaks = e_TWK_NoTweaks;

tdstDisplayMode g_stCurrentMode = { 0 };
tdeRefRate g_eRefRate = e_RR_Full;
BOOL g_bForceVsync = FALSE;
BOOL g_bFullscreen = FALSE;

tdeVerifyErr g_eMissingFiles = e_VE_Ok;


/*
 * Strings
 */

char const *szDegePath = ".\\dgVoodoo.conf";
char const *szUbiPath = ".\\Ubi.ini";

char const *szUbiR2 = "Rayman2";
char const *szUbiFix = "Ray2Fix";

char const *szTrue = "true";
char const *szFalse = "false";

char const *a_szFilesToDelete[] = {
	"goggame.sdb",
	"goglog.ini",
	"gog.ico",
	"support.ico",
	"EULA.txt",
	"webcache.zip",
	"nglide_config.exe",
	"nglide_readme.txt",
	"nGlideEULA.txt",
	"3DfxSpl.dll",
	"3DfxSpl3.dll",
	"glide.dll",
	"glide3x.dll"
};


/*
 * Functions
 */


void fn_vReadUbiIni( void )
{
	char szBuffer[128];

	// GLI library
	GetPrivateProfileString(szUbiR2, "GLI_Dll", NULL, szBuffer, sizeof(szBuffer), szUbiPath);
	if ( !strcmp(szBuffer, "Ray2Fix") )
	{
		g_bFixState = g_bFixPrevState = TRUE;
	}

	DWORD dwWidth = 0;
	DWORD dwHeight = 0;

	// Display mode
	GetPrivateProfileString(szUbiR2, "GLI_Mode", NULL, szBuffer, sizeof(szBuffer), szUbiPath);
	int nParsed = sscanf_s(szBuffer, "1 - %d x %d", &dwWidth, &dwHeight);

	if ( nParsed == 2 && dwWidth > 0 && dwHeight > 0 )
	{
		g_stCurrentMode.dwWidth = dwWidth;
		g_stCurrentMode.dwHeight = dwHeight;
	}

	// Tweaks
	g_eTweaks = GetPrivateProfileInt(szUbiFix, "Tweaks", 0, szUbiPath);
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
	if ( !strcmp(szBuffer, szTrue) )
		g_bForceVsync = TRUE;

	// Fullscreen mode
	GetPrivateProfileString("General", "FullScreenMode", NULL, szBuffer, sizeof(szBuffer), szDegePath);
	if ( !strcmp(szBuffer, szTrue) )
		g_bFullscreen = TRUE;
}

void fn_vWriteUbiIni( void )
{
	char szBuffer[128];

	// GLI library path & name
	char *szDllFile = g_bFixState ? "GliFix" : "GliVd1";
	WritePrivateProfileString(szUbiR2, "GLI_DllFile", szDllFile, szUbiPath);

	char *szDll = g_bFixState ? "Ray2Fix" : "Glide2";
	WritePrivateProfileString(szUbiR2, "GLI_Dll", szDll, szUbiPath);

	// Doesn't really matter but write "Default" anyway
	char *szDevice = "Default";
	WritePrivateProfileString(szUbiR2, "GLI_Driver", szDevice, szUbiPath);
	WritePrivateProfileString(szUbiR2, "GLI_Device", szDevice, szUbiPath);

	// Display mode
	sprintf_s(szBuffer, sizeof(szBuffer), "1 - %i x %i x 16", g_stCurrentMode.dwWidth, g_stCurrentMode.dwHeight);
	WritePrivateProfileString(szUbiR2, "GLI_Mode", szBuffer, szUbiPath);

	// Tweaks
	sprintf_s(szBuffer, sizeof(szBuffer), "%i", g_eTweaks);
	WritePrivateProfileString(szUbiFix, "Tweaks", szBuffer, szUbiPath);

	// Refresh rate
	sprintf_s(szBuffer, sizeof(szBuffer), "%i", (g_eRefRate == e_RR_Half));
	WritePrivateProfileString(szUbiFix, "HalfRefRate", szBuffer, szUbiPath);
}

void fn_vWriteDegeIni( void )
{
	char szBuffer[128];

	// These values should never change, but write them anyway in case the user messes up the config
	WritePrivateProfileString("General", "ProgressiveScanlineOrder", szTrue, szDegePath);
	WritePrivateProfileString("General", "EnumerateRefreshRates", szTrue, szDegePath);
	WritePrivateProfileString("General", "ScalingMode", "stretched_ar", szDegePath);
	WritePrivateProfileString("General", "KeepWindowAspectRatio", szTrue, szDegePath);
	WritePrivateProfileString("Glide", "VideoCard", "voodoo_2", szDegePath);
	WritePrivateProfileString("Glide", "OnboardRAM", "12", szDegePath);
	WritePrivateProfileString("Glide", "MemorySizeOfTMU", "4096", szDegePath);
	WritePrivateProfileString("Glide", "NumberOfTMUs", "2", szDegePath);
	WritePrivateProfileString("Glide", "EnableGlideGammaRamp", szTrue, szDegePath);
	WritePrivateProfileString("Glide", "EnableInactiveAppState", szFalse, szDegePath);

	// Display mode & refresh rate
	sprintf_s(szBuffer, sizeof(szBuffer), "h:%i, v:%i, refrate:%i",
	          g_stCurrentMode.dwWidth, g_stCurrentMode.dwHeight, g_eRefRate);
	WritePrivateProfileString("Glide", "Resolution", szBuffer, szDegePath);

	// Force VSync
	char const *szVsync = g_bForceVsync ? szTrue : szFalse;
	WritePrivateProfileString("Glide", "ForceVerticalSync", szVsync, szDegePath);

	// Fullscreen mode
	char const *szFullScreen = g_bFullscreen ? szTrue : szFalse;
	WritePrivateProfileString("General", "FullScreenMode", szFullScreen, szDegePath);
}

void fn_vCleanUpGogMess( void )
{
	for ( DWORD i = 0; i < ARRAYSIZE(a_szFilesToDelete); i++ )
	{
		char szFilePath[MAX_PATH];
		sprintf_s(szFilePath, MAX_PATH, ".\\%s", a_szFilesToDelete[i]);

		DeleteFile(szFilePath);
	}
}

void CFG_fn_vRead( void )
{
	fn_vReadUbiIni();
	fn_vReadDegeIni();
}

void CFG_fn_vWrite( void )
{
	fn_vWriteUbiIni();
	fn_vWriteDegeIni();
}

void CFG_fn_vVerify( void )
{
	if ( GetFileAttributes(".\\goglog.ini") != INVALID_FILE_ATTRIBUTES )
	{
		// Delete unnecessary GOG/nGlide files
		fn_vCleanUpGogMess();
	}

	if ( GetFileAttributes(szUbiPath) == INVALID_FILE_ATTRIBUTES )
		g_eMissingFiles |= e_VE_UbiMissing;

	if ( GetFileAttributes(szDegePath) == INVALID_FILE_ATTRIBUTES )
		g_eMissingFiles |= e_VE_DegeMissing | e_VE_FixError;

	if ( GetFileAttributes(".\\DLL\\GliVd1Vf.dll") == INVALID_FILE_ATTRIBUTES )
		g_eMissingFiles |= e_VE_GlideMissing | e_VE_GameError;

	if ( GetFileAttributes(".\\DLL\\GliFixVf.dll") == INVALID_FILE_ATTRIBUTES )
		g_eMissingFiles |= e_VE_FixMissing | e_VE_FixError;

	if ( GetFileAttributes(".\\dinput.dll") == INVALID_FILE_ATTRIBUTES )
		g_eMissingFiles |= e_VE_DinputMissing | e_VE_FixError;
}
