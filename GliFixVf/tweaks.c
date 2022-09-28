#include "framework.h"
#include "tweaks.h"
#include "config.h"
#include "log.h"

char *szModsFolder = ".\\Mods";

MOD_DATA TWK_aMods[MAX_MODS] = { 0 };
int TWK_lModCount = 0;

BOOL TWK_fn_bLoadSingleMod( char *szDllName )
{
	char szFilePath[MAX_PATH];
	sprintf_s(szFilePath, MAX_PATH, "%s\\%s", szModsFolder, szDllName);

	LOG_fn_vPrintf("Loading mod '%s'", szDllName);
	LOG_fn_vPrintf("Path to mod: '%s'", szFilePath);

	HMODULE hMod = LoadLibrary(szFilePath);

	if ( hMod )
	{
		MOD_DATA mod = { hMod, NULL };
		TWK_aMods[TWK_lModCount++] = mod;
		return TRUE;
	}
	else
	{
		DWORD dwError = GetLastError();
		LOG_fn_vPrintf("Could not load mod DLL '%s'! Error code 0x%08x", szDllName, dwError);
	}

	return FALSE;
}

void TWK_fn_vLoadAllMods( void )
{
	char szFileName[MAX_PATH];
	WIN32_FIND_DATA ffd = { 0 };
	int lMods = 0;

	LOG_fn_vPrintf("Loading mods...");

	// Make sure the mods folder exists
	CreateDirectory(szModsFolder, NULL);

	sprintf_s(szFileName, MAX_PATH, "%s\\%s", szModsFolder, "*.dll");

	HANDLE hFind = FindFirstFile(szFileName, &ffd);

	if ( hFind != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( TWK_fn_bLoadSingleMod(ffd.cFileName) )
				lMods++;
		}
		while ( FindNextFile(hFind, &ffd) != 0 );
		FindClose(hFind);
	}
	else
	{
		DWORD dwError = GetLastError();
		LOG_fn_vPrintf("Could not find any DLL files to load! Error code 0x%08x", dwError);
	}

	TWK_lModCount = lMods;
	LOG_fn_vPrintf("Finished loading %d mods.", lMods);
}

void TWK_fn_vInitTweaks( void )
{
	if ( CFG_lTweaks & TWK_MODLOADER )
	{
		TWK_fn_vLoadAllMods();
	}
}
