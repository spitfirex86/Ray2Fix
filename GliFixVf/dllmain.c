#include "framework.h"
#include "imports.h"
#include "fix.h"
#include "config.h"


void fn_vInitDll( void )
{
	CFG_fn_vInitGlobals();
	IMP_fn_vLoadGliLibrary();

	if ( CFG_bIsMainModuleR2 )
	{
		/* Loaded in the game, patch all */
		FIX_fn_vAttachHooks();
	}
	else
	{
		/* Probably loaded in GxSetup/GliDetect, only patch mode enum */
		FIX_fn_vRemoveModeEnum();
	}
}

void fn_vDeInitDll( void )
{
	if ( CFG_bIsMainModuleR2 )
	{
		FIX_fn_vDetachHooks();
	}
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
	switch ( dwReason )
	{
	case DLL_PROCESS_ATTACH:
		fn_vInitDll();
		break;

	case DLL_PROCESS_DETACH:
		fn_vDeInitDll();
		break;
	}

	return TRUE;
}
