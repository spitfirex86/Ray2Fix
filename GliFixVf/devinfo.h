#pragma once

#include "framework.h"


/*
 * Renderer Info
 */

extern char const *GLI_szName;
extern char const *GLI_szDesc;
extern char const *GLI_szVersion;
extern int const GLI_lCode;


/*
 * Device Capabilities
 */

#define C_DI_DllBmp				"dll_bmp"
#define C_DI_DevCaps			"dev_caps"

#define C_DI_DisplayAdd			"dispadd"
#define C_DI_DisplayName		"dispname"
#define C_DI_DisplayDesc		"dispdesc"

#define C_DI_DeviceAdd			"dev_add"
#define C_DI_DeviceName			"dev_name"
#define C_DI_DeviceDesc			"dev_desc"

#define C_DI_ModeAdd			"modeadd"
#define C_DI_ModeFullscreen		"modefs"
#define C_DI_ModeBpp			"modebpp"
#define C_DI_ModeWidth			"modew"
#define C_DI_ModeHeight			"modeh"


typedef enum tdeDriverCaps
{
	E_DRV_IsHardware = 1 << 0,
	E_DRV_CanUseAGP = 1 << 1,
	E_DRV_CanUseSystemMemory = 1 << 2,
	E_DRV_CanBeWindowed = 1 << 3,
	E_DRV_SupportAlphaTest = 1 << 4,
	E_DRV_ReadjustViewport = 1 << 5
	// ...
}
tdeDriverCaps;

typedef enum tdeTextureCaps
{
	E_TEX_Support8P = 1 << 0,
	E_TEX_SupportNonSquare = 1 << 1,
	E_TEX_SupportMipMap = 1 << 2,
	E_TEX_SupportMirror = 1 << 3
	// ...
}
tdeTextureCaps;

typedef struct tdstGliCaps
{
	unsigned long ulSize;
	tdeDriverCaps ulDriverCaps;
	tdeTextureCaps ulTextureCaps;
	unsigned long ulSurfaceCaps;
	unsigned long ulMemoryCaps;
	unsigned long ulTextureMaxSize;
	unsigned long ulTotalTextureMemory;
	unsigned long ulTotalVideoMemory; /* mem local */
	unsigned long ulTotalAGPMemory; /* mem non-local */
	unsigned char ucPrimarySurfacePixelFormat;
	float xRefreshRate;
}
tdstGliCaps;


/* GLI property set function, used in GLI_DRV_fn_lGetAllDisplayConfig */
typedef int (*tdfn_lAddDisplayInfo)( DWORD lDisplay, DWORD lDevice, DWORD lMode, const char *szName, long Value );

/* Common function, used in GLI_DRV_lSetCommonFct */
typedef int (*tdfn_CommonFct)();
