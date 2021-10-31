#include "framework.h"
#include "config.h"

//
// GLOBAL VARS
//

BOOL g_bFixState = FALSE;
BOOL g_bFixPrevState = FALSE;

TWEAKS g_lTweaks = TWK_NO_TWEAKS;

DISP_MODE g_dmCurrentMode = { 0 };
REFRATE g_lRefRate = RR_FULL;
BOOL g_bForceVsync = FALSE;
BOOL g_bFullscreen = FALSE;

VERIFY_ERR g_veMissingFiles = VE_OK;

//
// STRINGS
//

char *szDegePath = ".\\dgVoodoo.conf";
char *szUbiPath = ".\\Ubi.ini";

char *szUbiR2 = "Rayman2";
char *szUbiFix = "Ray2Fix";
char *szDegeGlide = "Glide";
char *szDegeGeneral = "General";

char *szTrue = "true";
char *szFalse = "false";

char *a_szFilesToDelete[] = {
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

//
// FUNCTIONS
//

void ReadUbiIni( void )
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
		g_dmCurrentMode.dwWidth = dwWidth;
		g_dmCurrentMode.dwHeight = dwHeight;
	}

	// Tweaks
	g_lTweaks = GetPrivateProfileInt(szUbiFix, "Tweaks", 0, szUbiPath);
}

void ReadDegeIni( void )
{
	char szBuffer[128];

	DWORD dwRefRate = 0;

	// Refresh rate
	GetPrivateProfileString(szDegeGlide, "Resolution", NULL, szBuffer, sizeof(szBuffer), szDegePath);
	char *lpLastComma = strrchr(szBuffer, ',');

	if ( lpLastComma != NULL )
	{
		int nParsed = sscanf_s(++lpLastComma, " refrate:%d", &dwRefRate);

		if ( nParsed == 1 && dwRefRate > 0 )
			g_lRefRate = dwRefRate;
	}

	// Force VSync
	GetPrivateProfileString(szDegeGlide, "ForceVerticalSync", NULL, szBuffer, sizeof(szBuffer), szDegePath);
	if ( !strcmp(szBuffer, szTrue) )
		g_bForceVsync = TRUE;

	// Fullscreen mode
	GetPrivateProfileString(szDegeGeneral, "FullScreenMode", NULL, szBuffer, sizeof(szBuffer), szDegePath);
	if ( !strcmp(szBuffer, szTrue) )
		g_bFullscreen = TRUE;
}

void WriteUbiIni( void )
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
	sprintf_s(szBuffer, sizeof(szBuffer), "1 - %i x %i x 16", g_dmCurrentMode.dwWidth, g_dmCurrentMode.dwHeight);
	WritePrivateProfileString(szUbiR2, "GLI_Mode", szBuffer, szUbiPath);

	// Tweaks
	sprintf_s(szBuffer, sizeof(szBuffer), "%i", g_lTweaks);
	WritePrivateProfileString(szUbiFix, "Tweaks", szBuffer, szUbiPath);

	// Refresh rate
	sprintf_s(szBuffer, sizeof(szBuffer), "%i", (g_lRefRate == RR_HALF));
	WritePrivateProfileString(szUbiFix, "HalfRefRate", szBuffer, szUbiPath);
}

void WriteDegeIni( void )
{
	char szBuffer[128];

	// These values should never change, but write them anyway in case the user messes up the config
	WritePrivateProfileString(szDegeGeneral, "ProgressiveScanlineOrder", szTrue, szDegePath);
	WritePrivateProfileString(szDegeGeneral, "EnumerateRefreshRates", szTrue, szDegePath);
	WritePrivateProfileString(szDegeGeneral, "ScalingMode", "stretched_ar", szDegePath);
	WritePrivateProfileString(szDegeGeneral, "KeepWindowAspectRatio", szTrue, szDegePath);
	WritePrivateProfileString(szDegeGlide, "VideoCard", "voodoo_2", szDegePath);
	WritePrivateProfileString(szDegeGlide, "OnboardRAM", "12", szDegePath);
	WritePrivateProfileString(szDegeGlide, "MemorySizeOfTMU", "4096", szDegePath);
	WritePrivateProfileString(szDegeGlide, "NumberOfTMUs", "2", szDegePath);
	WritePrivateProfileString(szDegeGlide, "EnableGlideGammaRamp", szTrue, szDegePath);
	WritePrivateProfileString(szDegeGlide, "EnableInactiveAppState", szFalse, szDegePath);

	// Display mode & refresh rate
	sprintf_s(szBuffer, sizeof(szBuffer), "h:%i, v:%i, refrate:%i",
	          g_dmCurrentMode.dwWidth, g_dmCurrentMode.dwHeight, g_lRefRate);
	WritePrivateProfileString(szDegeGlide, "Resolution", szBuffer, szDegePath);

	// Force VSync
	char *szVsync = g_bForceVsync ? szTrue : szFalse;
	WritePrivateProfileString(szDegeGlide, "ForceVerticalSync", szVsync, szDegePath);

	// Fullscreen mode
	char *szFullScreen = g_bFullscreen ? szTrue : szFalse;
	WritePrivateProfileString(szDegeGeneral, "FullScreenMode", szFullScreen, szDegePath);
}

void CleanUpGogMess( void )
{
	char szFindQuery[MAX_PATH];
	WIN32_FIND_DATA ffd = { 0 };

	for ( DWORD i = 0; i < ARRAYSIZE(a_szFilesToDelete); i++ )
	{
		char szFilePath[MAX_PATH];
		sprintf_s(szFilePath, MAX_PATH, ".\\%s", a_szFilesToDelete[i]);

		DeleteFile(szFilePath);
	}

	sprintf_s(szFindQuery, MAX_PATH, ".\\%s", "goggame-*.*");
	HANDLE hFind = FindFirstFile(szFindQuery, &ffd);

	if ( hFind != INVALID_HANDLE_VALUE )
	{
		do
		{
			char szFilePath[MAX_PATH];
			sprintf_s(szFilePath, MAX_PATH, ".\\%s", ffd.cFileName);

			DeleteFile(szFilePath);
		}
		while ( FindNextFile(hFind, &ffd) != 0 );
		FindClose(hFind);
	}
}

void ReadConfig( void )
{
	ReadUbiIni();
	ReadDegeIni();
}

void WriteConfig( void )
{
	WriteUbiIni();
	WriteDegeIni();
}

void VerifyFiles( void )
{
	if ( GetFileAttributes(".\\goglog.ini") != INVALID_FILE_ATTRIBUTES )
	{
		// Delete unnecessary GOG/nGlide files
		CleanUpGogMess();
	}

	if ( GetFileAttributes(szUbiPath) == INVALID_FILE_ATTRIBUTES )
		g_veMissingFiles |= VE_UBI_MISSING;

	if ( GetFileAttributes(szDegePath) == INVALID_FILE_ATTRIBUTES )
		g_veMissingFiles |= VE_DEGE_MISSING | VE_FIX_ERROR;

	if ( GetFileAttributes(".\\DLL\\GliVd1Vf.dll") == INVALID_FILE_ATTRIBUTES )
		g_veMissingFiles |= VE_GLIDE_MISSING | VE_GAME_ERROR;

	if ( GetFileAttributes(".\\DLL\\GliFixVf.dll") == INVALID_FILE_ATTRIBUTES )
		g_veMissingFiles |= VE_FIX_MISSING | VE_FIX_ERROR;

	if ( GetFileAttributes(".\\dinput.dll") == INVALID_FILE_ATTRIBUTES )
		g_veMissingFiles |= VE_DINPUT_MISSING | VE_FIX_ERROR;
}
