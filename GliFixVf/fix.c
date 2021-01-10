#include "framework.h"
#include <detours.h>
#include "fix.h"
#include "imports.h"
#include "r2fn.h"
#include "config.h"

char *szConfigMenu = "/C:Ray2Fix Config";

char szVersionString[50];
JFFTXT txtVersion = {
	NULL,
	5, 980, 7, 160,
	0, 0, 0, 0, 0
};

//
// DETOURS
//

BOOL CALLBACK FIX_fn_InputEnum( void *lpddi, void *pvRef )
{
	// Call original function
	R2_fn_InputEnum(lpddi, pvRef);

	// HACK: Input defice enumeration fix
	// Rayman2 returns DIENUM_STOP when it encounters a device type it cannot handle.
	// This usually causes it to skip enumeration of gamepads on modern PCs.
	// Here we fix this by always returning DIENUM_CONTINUE.
	return DIENUM_CONTINUE;
}

BOOL FIX_fn_SuspendGame()
{
	// HACK: Disable suspended game state
	// Disregard whatever the game's trying to do here and always return TRUE.
	// This prevents the game from suspending itself when the focus is lost.
	return TRUE;
}

char* FIX_fn_szGetStringFromTextOrStringParam( void *param )
{
	// Call original function
	char *result = R2_fn_szGetStringFromTextOrStringParam(param);

	// HACK: Replace "Site Rayman2" in main menu with a shortcut to R2FixCfg
	if ( !_stricmp(R2_fn_p_szGetLevelName(), "menu") )
	{
		// Part 1: Replace menu text.
		if ( !strcmp(result, "/C:Site Rayman2") )
		{
			return szConfigMenu;
		}

		// Part 2: Open R2FixCfg with ShellExecute.
		if ( !strcmp(result, "http://www.rayman2.com/") )
		{
			if ( CFG_bOpenConfigTool() )
				SendMessage(R2_GetWindowHandle(), WM_CLOSE, 0, 0);
		}
	}

	return result;
}

void FIX_JFFTXT_vAffiche( void *lpContext )
{
	R2_JFFTXT_vDrawString(lpContext, &txtVersion);

	// Call original function
	R2_JFFTXT_vAffiche(lpContext);
}

//
// FUNCTIONS
//

void fn_vPreAttachHooks( void )
{
	if ( CFG_bTainted )
	{
		sprintf_s(szVersionString, sizeof(szVersionString), "/O200:%s v%s", GLI_szName, GLI_szVersion);
		txtVersion.lpText = szVersionString;
	}
}

void FIX_vAttachHooks( void )
{
	fn_vPreAttachHooks();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach((PVOID*)&R2_fn_InputEnum, (PVOID)FIX_fn_InputEnum);
	DetourAttach((PVOID*)&R2_fn_SuspendGame, (PVOID)FIX_fn_SuspendGame);
	DetourAttach((PVOID*)&R2_fn_szGetStringFromTextOrStringParam, (PVOID)FIX_fn_szGetStringFromTextOrStringParam);
	
	if ( CFG_bTainted )
	{
		DetourAttach((PVOID*)&R2_JFFTXT_vAffiche, (PVOID)FIX_JFFTXT_vAffiche);
	}

	DetourTransactionCommit();
}

void FIX_vDetachHooks( void )
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourDetach((PVOID*)&R2_fn_InputEnum, (PVOID)FIX_fn_InputEnum);
	DetourDetach((PVOID*)&R2_fn_SuspendGame, (PVOID)FIX_fn_SuspendGame);
	DetourDetach((PVOID*)&R2_fn_szGetStringFromTextOrStringParam, (PVOID)FIX_fn_szGetStringFromTextOrStringParam);

	if ( CFG_bTainted )
	{
		DetourDetach((PVOID*)&R2_JFFTXT_vAffiche, (PVOID)FIX_JFFTXT_vAffiche);
	}

	DetourTransactionCommit();
}

void FIX_vPatchFramerate( void )
{
	typedef struct
	{
		char _code1[54];
		WORD nop1;
		char _code2[8];
		WORD nop2;
	} FlipDeviceCode;

	FlipDeviceCode *lpCode = DetourCodeFromPointer((PVOID)Vd_GLI_DRV_vFlipDeviceWithSyncro, NULL);

	DWORD dwOldProtect;
	DWORD dwNewProtect = PAGE_EXECUTE_READWRITE;

	VirtualProtect(lpCode, sizeof(FlipDeviceCode), dwNewProtect, &dwOldProtect);

	lpCode->nop1 = lpCode->nop2 = (WORD)0x9090;

	VirtualProtect(lpCode, sizeof(FlipDeviceCode), dwOldProtect, &dwNewProtect);
}

void FIX_vRemoveModeEnum( void )
{
	typedef struct
	{
		char _code1[251];
		char nop1[30];
	} DisplayConfigCode;

	DisplayConfigCode *lpCode = DetourCodeFromPointer((PVOID)Vd_GLI_DRV_fn_lGetAllDisplayConfig, NULL);

	DWORD dwOldProtect;
	DWORD dwNewProtect = PAGE_EXECUTE_READWRITE;

	VirtualProtect(lpCode, sizeof(DisplayConfigCode), dwNewProtect, &dwOldProtect);

	memset(lpCode->nop1, 0x90, sizeof(lpCode->nop1));

	VirtualProtect(lpCode, sizeof(DisplayConfigCode), dwOldProtect, &dwNewProtect);
}
