#include "framework.h"
#include <detours.h>
#include <math.h>
#include "fix.h"
#include "imports.h"
#include "r2fn.h"
#include "config.h"


char *szConfigMenu = "/C:Ray2Fix Config";
char szVersionString[50];


/*
 * Detours
 */

BOOL CALLBACK FIX_fn_InputEnum( void *lpddi, void *pvRef )
{
	// Call original function
	R2_fn_InputEnum(lpddi, pvRef);

	// HACK: Input device enumeration fix
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

char * FIX_fn_szGetStringFromTextOrStringParam( void *param )
{
	// Call original function
	char *result = R2_fn_szGetStringFromTextOrStringParam(param);

	// HACK: Replace "Site Rayman2" in main menu with a shortcut to R2FixCfg
	if ( !_stricmp(R2_fn_p_szGetLevelName(), "menu") )
	{
		// Part 1: Replace menu text.
		if ( !strcmp(result, "/C:Site Rayman2")
			|| !strcmp(result, "/C:Web de Rayman2")
			|| !strcmp(result, "/C:Sito di Rayman2")
			|| !strcmp(result, "/C:Rayman2 - Webseite") )
		{
			//return szConfigMenu;
			return szVersionString;
		}

		// Part 2: Open R2FixCfg with ShellExecute.
		if ( !strcmp(result, "http://www.rayman2.com/") )
		{
			if ( CFG_fn_bOpenConfigTool() )
				SendMessage(R2_GetWindowHandle(), WM_CLOSE, 0, 0);
		}
	}

	return result;
}

void FIX_GLI_xAdjustCameraToViewport2(GLD_tdstDeviceAttributes *p_stDev, GLD_tdstViewportAttributes *p_stVpt, GLI_tdstCamera *p_stCam)
{
	// HACK: Calculate new FOV based on selected aspect ratio
	float fOldFOV = *((float*)p_stCam + 25);
	float fNewFOV = 2.0f * atan(tan(fOldFOV / 2.0f) * 0.75f * CFG_fAspectRatio);
	*((float*)p_stCam + 25) = fNewFOV;

	// Call original function
	R2_GLI_xAdjustCameraToViewport2(p_stDev, p_stVpt, p_stCam);
}

/*
 * Functions
 */

void fn_vPreAttachHooks( void )
{
	//sprintf_s(szVersionString, sizeof(szVersionString), "/O200:%s v%s", GLI_szName, GLI_szVersion);
	snprintf(szVersionString, sizeof(szVersionString), "/C:%s v%s", GLI_szName, GLI_szVersion);
}

void FIX_fn_vAttachHooks( void )
{
	fn_vPreAttachHooks();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach((PVOID*)&R2_fn_InputEnum, (PVOID)FIX_fn_InputEnum);
	DetourAttach((PVOID*)&R2_fn_SuspendGame, (PVOID)FIX_fn_SuspendGame);
	DetourAttach((PVOID*)&R2_fn_szGetStringFromTextOrStringParam, (PVOID)FIX_fn_szGetStringFromTextOrStringParam);
	DetourAttach((PVOID*)&R2_GLI_xAdjustCameraToViewport2, (PVOID)FIX_GLI_xAdjustCameraToViewport2);

	DetourTransactionCommit();
}

void FIX_fn_vDetachHooks( void )
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourDetach((PVOID*)&R2_fn_InputEnum, (PVOID)FIX_fn_InputEnum);
	DetourDetach((PVOID*)&R2_fn_SuspendGame, (PVOID)FIX_fn_SuspendGame);
	DetourDetach((PVOID*)&R2_fn_szGetStringFromTextOrStringParam, (PVOID)FIX_fn_szGetStringFromTextOrStringParam);
	DetourDetach((PVOID*)&R2_GLI_xAdjustCameraToViewport2, (PVOID)FIX_GLI_xAdjustCameraToViewport2);

	DetourTransactionCommit();
}

void FIX_fn_vPatchFramerate( void )
{
	return; /* no longer necessary */

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

void FIX_fn_vRemoveModeEnum( void )
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
