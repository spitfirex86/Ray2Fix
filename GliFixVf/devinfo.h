#pragma once
#include "framework.h"

//
// RENDERER INFO
//

extern const char *GLI_szName;
extern const char *GLI_szDesc;
extern const char *GLI_szVersion;
extern const int GLI_lCode;

//
// DEVICE CAPABILITIES
//

// Device capability flags
typedef enum tagDC_DEVICE
{
	DF_HARDWARE_ACCEL = 1 << 0,
	DF_AGP_MEM = 1 << 1,
	DF_SYS_MEM = 1 << 2,
	DF_WINDOWED = 1 << 3,
	DF_ALPHA_TEST = 1 << 4,
	DF_READJUST_VIEWPORT = 1 << 5
	// ...
} DC_DEVICE;

// Texture capability flags
typedef enum tagDC_TEXTURE
{
	TF_PALETTIZED = 1 << 0,
	TF_NONSQUARE = 1 << 1,
	TF_MIPMAP = 1 << 2,
	TF_MIRROR = 1 << 3
	// ...
} DC_TEXTURE;

// Device capabilites
typedef struct tagDEV_CAPS
{
	char _char0; // ?
	DC_DEVICE DeviceFlags;
	DC_TEXTURE TextureFlags;
	BYTE _gapC[8]; // ?
	DWORD TextureMaxSize;
	DWORD TextureMem;
	DWORD VideoMemLocal;
	DWORD VideoMemNonLocal;
	BYTE _byte24; // ?
	float RefreshRate;
} DEV_CAPS;


//
// FUNCTION POINTER TYPES
//

// GLI property set function, used in GLI_DRV_fn_lGetAllDisplayConfig
typedef int (*GliSet)(DWORD display, DWORD device, DWORD mode, const char *szAction, const void *value);

// Common function, used in GLI_DRV_lSetCommonFct
typedef int (*CommonFct)();

//
// GLISET ACTIONS
//

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
