#include "framework.h"
#include "tweaks.h"
#include "config.h"

char *szModsFolder = ".\\Mods";

MOD_DATA TWK_aMods[MAX_MODS] = { 0 };
int TWK_lModCount = 0;

BOOL TWK_fn_bLoadSingleMod( char *szDllName )
{
	char szFilePath[MAX_PATH];
	sprintf_s(szFilePath, MAX_PATH, "%s\\%s", szModsFolder, szDllName);

	HMODULE hMod = LoadLibrary(szFilePath);

	if ( hMod )
	{
		MOD_DATA mod = { hMod, NULL };
		TWK_aMods[TWK_lModCount++] = mod;
		return TRUE;
	}

	return FALSE;
}

void TWK_fn_vLoadAllMods( void )
{
	char szFileName[MAX_PATH];
	WIN32_FIND_DATA ffd = { 0 };
	int lMods = 0;

	// Make sure the mods folder exists
	CreateDirectory(szModsFolder, NULL);

	sprintf_s(szFileName, MAX_PATH, "%s\\%s", szModsFolder, "*.dll");

	HANDLE hFind = FindFirstFile(szFileName, &ffd);

	if ( hFind != INVALID_HANDLE_VALUE )
	{
		do
		{
			TWK_fn_bLoadSingleMod(ffd.cFileName);
		}
		while ( FindNextFile(hFind, &ffd) != 0 );
		FindClose(hFind);
	}

	TWK_lModCount = lMods;
}

void TWK_fn_vInitTweaks( void )
{
	if ( CFG_lTweaks & TWK_MODLOADER )
	{
		TWK_fn_vLoadAllMods();
	}
}
