#include "framework.h"
#include "imports.h"
#include "fix.h"
#include "config.h"
#include "tweaks.h"
#include "log.h"

void fn_vInitDll( void )
{
	LOG_fn_vInit();
	LOG_fn_vPrintf("Ray2Fix %s initialising...", GLI_szVersion);
	CFG_vInitGlobals();
	IMP_vLoadGliLibrary();

	if ( CFG_bIsMainModuleR2 )
	{
		// Game is running, fix stuff
		LOG_fn_vPrintf("Applying fixes...");
		FIX_vPatchFramerate();
		FIX_vAttachHooks();

		if ( CFG_lTweaks )
		{
		LOG_fn_vPrintf("Applying tweaks...");

			TWK_fn_vInitTweaks();
		}
	}
	else
	{
		// Most likely GxSetup/GliDetect
		FIX_vRemoveModeEnum();
	}

	LOG_fn_vPrintf("Ray2Fix %s library initialised.", GLI_szVersion);
}

void fn_vDeInitDll( void )
{
	if ( CFG_bIsMainModuleR2 )
	{
		FIX_vDetachHooks();
	}
}

BOOL APIENTRY DllMain(
	HMODULE hModule,
	DWORD dwReason,
	LPVOID lpReserved
	)
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
