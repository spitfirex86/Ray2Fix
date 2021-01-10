#include "framework.h"
#include "imports.h"

//
// GLI LIBRARY IMPORTS
//

BOOL (*Vd_GLI_DRV_lGetDllInfo)( const char *, void * ) = NULL;
BOOL (*Vd_GLI_DRV_fn_lGetAllDisplayConfig)( GliSet ) = NULL;

BOOL (*Vd_GLI_DRV_lSetCommonData)( const char *, void * ) = NULL;
BOOL (*Vd_GLI_DRV_lSetCommonFct)( const char *, CommonFct ) = NULL;

BOOL (*Vd_GLI_DRV_fnl_EnumModes)( char *, char * ) = NULL;

DWORD (*Vd_GLI_DRV_xInitDriver)( HWND, BOOL, int, int, int ) = NULL;
void (*Vd_GLI_DRV_vCloseDriver)() = NULL;
BOOL (*Vd_GLI_DRV_bBeginScene)() = NULL;
BOOL (*Vd_GLI_DRV_bEndScene)() = NULL;
BOOL (*Vd_GLI_DRV_bLockDevice)( DWORD *, DWORD * ) = NULL;
BOOL (*Vd_GLI_DRV_bUnlockDevice)() = NULL;
void (*Vd_GLI_DRV_vClearDevice)( int, int, int ) = NULL;
void (*Vd_GLI_DRV_vFlipDevice)() = NULL;
void (*Vd_GLI_DRV_vFlipDeviceWithSyncro)();
void (*Vd_GLI_DRV_vDownLoadTextures)( int, int, int ) = NULL;
void (*Vd_GLI_DRV_vUnLoadTextures)() = NULL;
int (*Vd_GLI_DRV_lGetSizeOfTexture)( void * ) = NULL;
void (*Vd_GLI_DRV_vDoOpaqueTextureSelection)( int ) = NULL;
int (*Vd_GLI_DRV_hChangeMode)( BOOL, int, int, int ) = NULL;
BOOL (*Vd_GLI_DRV_bWindowedModeIsOptimized)() = NULL;
void (*Vd_GLI_DRV_vOptimizedWindowedMode)() = NULL;
void (*Vd_GLI_DRV_vNonOptimizedWindowedMode)() = NULL;
BOOL (*Vd_GLI_DRV_bPrepareForGliWindowed)( HWND ) = NULL;
void (*Vd_GLI_DRV_vPrepareForGliFullScreen)( HWND ) = NULL;
void (*Vd_GLI_DRV_vActivateGli)( HWND, BOOL ) = NULL;
void (*Vd_GLI_DRV_vReadaptDisplay)() = NULL;
void (*Vd_GLI_DRV_vAddBlackPolygon)( int, int, int, int ) = NULL;
void (*Vd_GLI_DRV_vNoBlackPolygon)() = NULL;
void (*Vd_GLI_DRV_vSetZClip)( float, int ) = NULL;
void (*Vd_GLI_DRV_vSetClipWindow)( float, int, int, int, int ) = NULL;
void (*Vd_GLI_DRV_vSendSingleLineToClip)( int, int, int, int, int ) = NULL;
void (*Vd_GLI_DRV_vSendSpriteToClip)( int, int, int ) = NULL;
void (*Vd_GLI_DRV_vSendSpriteToClipWithColors)( int, int, int, int ) = NULL;
void (*Vd_GLI_DRV_vSendSpriteToClipWithUV)( int, int, int, int ) = NULL;
int (*Vd_GLI_DRV_xSendElementTIToClip_TRIANGLES)( int, int ) = NULL;
int (*Vd_GLI_DRV_xSendSingleTriangleToClip_TRIANGLES)( int, int, int ) = NULL;
int (*Vd_GLI_DRV_xClearViewingList)() = NULL;
int (*Vd_GLI_DRV_xSendListToViewport)() = NULL;
void (*Vd_GLI_DRV_vClearZBufferRegion)() = NULL;
void (*Vd_GLI_DRV_vComputeFogEffect)( int ) = NULL;
void (*Vd_GLI_DRV_vWrite16bBitmapToBackBuffer)( int, int, int, int, int, int, int ) = NULL;

HMODULE hGliVd = NULL;

void *fn_lpGetFn( LPCSTR szName )
{
	return (void*)GetProcAddress(hGliVd, szName);
}

void IMP_vLoadGliLibrary( void )
{
	hGliVd = LoadLibrary(".\\DLL\\GliVd1vf.dll");

	if ( !hGliVd )
	{
		MessageBox(NULL, "Cannot load GLI library.", "Error", MB_OK | MB_ICONERROR);
		ExitProcess(1);
	}

	Vd_GLI_DRV_lGetDllInfo = fn_lpGetFn("GLI_DRV_lGetDllInfo");
	Vd_GLI_DRV_fn_lGetAllDisplayConfig = fn_lpGetFn("GLI_DRV_fn_lGetAllDisplayConfig");
	Vd_GLI_DRV_lSetCommonData = fn_lpGetFn("GLI_DRV_lSetCommonData");
	Vd_GLI_DRV_lSetCommonFct = fn_lpGetFn("GLI_DRV_lSetCommonFct");
	Vd_GLI_DRV_fnl_EnumModes = fn_lpGetFn("GLI_DRV_fnl_EnumModes");
	Vd_GLI_DRV_xInitDriver = fn_lpGetFn("GLI_DRV_xInitDriver");
	Vd_GLI_DRV_vCloseDriver = fn_lpGetFn("GLI_DRV_vCloseDriver");
	Vd_GLI_DRV_bBeginScene = fn_lpGetFn("GLI_DRV_bBeginScene");
	Vd_GLI_DRV_bEndScene = fn_lpGetFn("GLI_DRV_bEndScene");
	Vd_GLI_DRV_bLockDevice = fn_lpGetFn("GLI_DRV_bLockDevice");
	Vd_GLI_DRV_bUnlockDevice = fn_lpGetFn("GLI_DRV_bUnlockDevice");
	Vd_GLI_DRV_vClearDevice = fn_lpGetFn("GLI_DRV_vClearDevice");
	Vd_GLI_DRV_vFlipDevice = fn_lpGetFn("GLI_DRV_vFlipDevice");
	Vd_GLI_DRV_vFlipDeviceWithSyncro = fn_lpGetFn("GLI_DRV_vFlipDeviceWithSyncro");
	Vd_GLI_DRV_vDownLoadTextures = fn_lpGetFn("GLI_DRV_vDownLoadTextures");
	Vd_GLI_DRV_vUnLoadTextures = fn_lpGetFn("GLI_DRV_vUnLoadTextures");
	Vd_GLI_DRV_lGetSizeOfTexture = fn_lpGetFn("GLI_DRV_lGetSizeOfTexture");
	Vd_GLI_DRV_vDoOpaqueTextureSelection = fn_lpGetFn("GLI_DRV_vDoOpaqueTextureSelection");
	Vd_GLI_DRV_hChangeMode = fn_lpGetFn("GLI_DRV_hChangeMode");
	Vd_GLI_DRV_bWindowedModeIsOptimized = fn_lpGetFn("GLI_DRV_bWindowedModeIsOptimized");
	Vd_GLI_DRV_vOptimizedWindowedMode = fn_lpGetFn("GLI_DRV_vOptimizedWindowedMode");
	Vd_GLI_DRV_vNonOptimizedWindowedMode = fn_lpGetFn("GLI_DRV_vNonOptimizedWindowedMode");
	Vd_GLI_DRV_bPrepareForGliWindowed = fn_lpGetFn("GLI_DRV_bPrepareForGliWindowed");
	Vd_GLI_DRV_vPrepareForGliFullScreen = fn_lpGetFn("GLI_DRV_vPrepareForGliFullScreen");
	Vd_GLI_DRV_vActivateGli = fn_lpGetFn("GLI_DRV_vActivateGli");
	Vd_GLI_DRV_vReadaptDisplay = fn_lpGetFn("GLI_DRV_vReadaptDisplay");
	Vd_GLI_DRV_vAddBlackPolygon = fn_lpGetFn("GLI_DRV_vAddBlackPolygon");
	Vd_GLI_DRV_vNoBlackPolygon = fn_lpGetFn("GLI_DRV_vNoBlackPolygon");
	Vd_GLI_DRV_vSetZClip = fn_lpGetFn("GLI_DRV_vSetZClip");
	Vd_GLI_DRV_vSetClipWindow = fn_lpGetFn("GLI_DRV_vSetClipWindow");
	Vd_GLI_DRV_vSendSingleLineToClip = fn_lpGetFn("GLI_DRV_vSendSingleLineToClip");
	Vd_GLI_DRV_vSendSpriteToClip = fn_lpGetFn("GLI_DRV_vSendSpriteToClip");
	Vd_GLI_DRV_vSendSpriteToClipWithColors = fn_lpGetFn("GLI_DRV_vSendSpriteToClipWithColors");
	Vd_GLI_DRV_vSendSpriteToClipWithUV = fn_lpGetFn("GLI_DRV_vSendSpriteToClipWithUV");
	Vd_GLI_DRV_xSendElementTIToClip_TRIANGLES = fn_lpGetFn("GLI_DRV_xSendElementTIToClip_TRIANGLES");
	Vd_GLI_DRV_xSendSingleTriangleToClip_TRIANGLES = fn_lpGetFn("GLI_DRV_xSendSingleTriangleToClip_TRIANGLES");
	Vd_GLI_DRV_xClearViewingList = fn_lpGetFn("GLI_DRV_xClearViewingList");
	Vd_GLI_DRV_xSendListToViewport = fn_lpGetFn("GLI_DRV_xSendListToViewport");
	Vd_GLI_DRV_vClearZBufferRegion = fn_lpGetFn("GLI_DRV_vClearZBufferRegion");
	Vd_GLI_DRV_vComputeFogEffect = fn_lpGetFn("GLI_DRV_vComputeFogEffect");
	Vd_GLI_DRV_vWrite16bBitmapToBackBuffer = fn_lpGetFn("GLI_DRV_vWrite16bBitmapToBackBuffer");
}
