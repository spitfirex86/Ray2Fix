#include "framework.h"
#include "imports.h"
#include "fix.h"
#include "config.h"


void fn_vInitDll( void )
{
	CFG_fn_vInitGlobals();
	IMP_fn_vLoadGliLibrary();

	if ( CFG_bIsMainModuleR2 ) /* retail RII */
	{
		/* Loaded in the game, patch all */
		FIX_fn_vAttachHooks();
	}
	else if ( !_stricmp(CFG_ModuleName, "MainWinR.exe") ) /* most likely some other flavor of RII */
	{
		if ( !CFG_DEBUG_lWaitFrame ) /* alt framerate fix */
			CFG_DEBUG_lWaitFrame = 1;

		if ( !_stricmp(CFG_ModuleDate, "1999-08-18") ) /* Demo 1 */
		{
			FIX_fn_vAttachHooksMin((void *)0x502250, (void *)0x401A10);
		}
		else if ( !_stricmp(CFG_ModuleDate, "1999-09-04") ) /* Demo 2 */
		{
			FIX_fn_vAttachHooksMin((void *)0x5129E0, (void *)0x401A10);
		}
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
	else if ( !_stricmp(CFG_ModuleName, "MainWinR.exe") )
	{
		FIX_fn_vDetachHooksMin();
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
