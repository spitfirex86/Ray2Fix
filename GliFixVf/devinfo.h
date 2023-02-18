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

// Device capability flags
typedef enum tdeDeviceFlags_
{
	e_DF_HardwareAccel = 1 << 0,
	e_DF_AgpMem = 1 << 1,
	e_DF_SysMem = 1 << 2,
	e_DF_Windowed = 1 << 3,
	e_DF_AlphaTest = 1 << 4,
	e_DF_ReadjustViewport = 1 << 5
	// ...
}
tdeDeviceFlags;

// Texture capability flags
typedef enum tdeTextureFlags_
{
	e_TF_Palettized = 1 << 0,
	e_TF_NonSquare = 1 << 1,
	e_TF_MipMap = 1 << 2,
	e_TF_Mirror = 1 << 3
	// ...
}
tdeTextureFlags;

// Device capabilites
typedef struct tdstDevCaps_
{
	char _char0; // ?
	tdeDeviceFlags eDeviceFlags;
	tdeTextureFlags eTextureFlags;
	BYTE _gapC[8]; // ?
	DWORD ulTextureMaxSize;
	DWORD ulTextureMem;
	DWORD ulVideoMemLocal;
	DWORD ulVideoMemNonLocal;
	BYTE _byte24; // ?
	float xRefreshRate;
}
tdstDevCaps;


// GLI property set function, used in GLI_DRV_fn_lGetAllDisplayConfig
typedef int (*tdfnGliSet)( DWORD display, DWORD device, DWORD mode, const char *szAction, const void *value );

// Common function, used in GLI_DRV_lSetCommonFct
typedef int (*tdfnCommonFct)();


/*
 * GliSet Actions
 */

extern const char *GS_BMP;
extern const char *GS_ADD_DISPLAY;
extern const char *GS_DISPLAY_NAME;
extern const char *GS_DISPLAY_DESC;

extern const char *GS_ADD_DEVICE;
extern const char *GS_DEVICE_NAME;
extern const char *GS_DEVICE_DESC;

extern const char *GS_ADD_MODE;
extern const char *GS_MODE_FULLSCREEN;
extern const char *GS_MODE_BITDEPTH;
extern const char *GS_MODE_WIDTH;
extern const char *GS_MODE_HEIGHT;
