#include "framework.h"
#include "GliFixVf.h"
#include "config.h"
#include "devinfo.h"
#include "imports.h"

// AddDisplayMode function pointer
int (*fnAddDiplayMode)( BOOL bFullscreen, int x, int y, int lBitDepth ) = NULL;

void vCopyString( char *dst, const char *src )
{
	size_t len = strlen(src) + 1;
	strcpy_s(dst, len, src);
}

//
// EXPORTED FUNCTIONS
//

// Copies renderer info (first listbox in GxSetup) to specified memory address
EXPORT BOOL GLI_DRV_lGetDllInfo( const char *szType, void *lpDst )
{
	if ( !strcmp(szType, "Name") )
	{
		vCopyString(lpDst, GLI_szName);
	}
	else if ( !strcmp(szType, "Description") )
	{
		vCopyString(lpDst, GLI_szDesc);
	}
	else if ( !strcmp(szType, "Version") )
	{
		vCopyString(lpDst, GLI_szVersion);
	}
	else if ( !strcmp(szType, "Code") )
	{
		*(DWORD*)lpDst = GLI_lCode;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

// Sets capability flags and populates displays, devices and resolutions
EXPORT BOOL GLI_DRV_fn_lGetAllDisplayConfig( GliSet gliSet )
{
	int disp, dev, mode;
	DEV_CAPS caps = { 0 };

	gliSet(0, 0, 0, GS_BMP, 0);

	// display info
	disp = gliSet(0, 0, 0, GS_ADD_DISPLAY, NULL);
	gliSet(disp, 0, 0, GS_DISPLAY_NAME, "Default");
	//(disp, 0, 0, GS_DISPLAY_DESC, "Display Description");

	// device info
	dev = gliSet(disp, 0, 0, GS_ADD_DEVICE, NULL);
	gliSet(disp, dev, 0, GS_DEVICE_NAME, "Default");
	//gliSet(disp, dev, 0, GS_DEVICE_DESC, "Device Description");

	// display mode (resolution)
	mode = gliSet(disp, dev, 0, GS_ADD_MODE, NULL);
	gliSet(disp, dev, mode, GS_MODE_FULLSCREEN, 1);
	gliSet(disp, dev, mode, GS_MODE_BITDEPTH, 16);
	gliSet(disp, dev, mode, GS_MODE_WIDTH, CFG_lDispWidth);
	gliSet(disp, dev, mode, GS_MODE_HEIGHT, CFG_lDispHeight);

	gliSet(disp, dev, 0, "dev_caps", &caps);

	// HACK: start R2FixCfg instead of GxSetup/GliDetect
	if ( CFG_bIsFixEnabled )
	{
		// If fix is enabled, always open R2FixCfg
		if ( CFG_bOpenConfigTool() )
			ExitProcess(0);
	}
	else
	{
		// If disabled, ask first
		int lResult = MessageBox(
			NULL, "Ray2Fix is installed but not enabled.\n Do you want to open Ray2Fix Settings?",
			GLI_szName, MB_YESNO | MB_ICONASTERISK);

		if ( lResult == IDYES )
		{
			if ( !CFG_bOpenConfigTool() )
			{
				MessageBox(NULL, "Cannot open Ray2Fix Settings, please update or reinstall Ray2Fix.",
				           GLI_szName, MB_OK | MB_ICONERROR);
			}

			ExitProcess(0);
		}
	}

	return TRUE;
}

DEV_CAPS *gliCaps;

EXPORT BOOL GLI_DRV_lSetCommonData( const char *szName, void *value )
{
	if ( !strcmp(szName, "GliCaps") )
	{
		gliCaps = value;
	}
	return Vd_GLI_DRV_lSetCommonData(szName, value);
}

EXPORT BOOL GLI_DRV_lSetCommonFct( const char *szName, CommonFct lpFn )
{
	// needed for EnumModes
	if ( fnAddDiplayMode == NULL && !strcmp(szName, "AddDisplayMode") )
	{
		fnAddDiplayMode = lpFn;
	}

	return Vd_GLI_DRV_lSetCommonFct(szName, lpFn);
}

EXPORT BOOL GLI_DRV_fnl_EnumModes( char *szDrvDspName, char *szDevName )
{
	fnAddDiplayMode(TRUE, CFG_lDispWidth, CFG_lDispHeight, 16);

	return TRUE;
}

//
// REDIRECTED EXPORTS
//

EXPORT NAKED DWORD GLI_DRV_xInitDriver( HWND hWnd, BOOL bFullscreen, int xRight, int yBottom, int lBitDepth )
{
	JUMP(Vd_GLI_DRV_xInitDriver);
}

EXPORT NAKED void GLI_DRV_vCloseDriver()
{
	JUMP(Vd_GLI_DRV_vCloseDriver);
}

EXPORT NAKED BOOL GLI_DRV_bBeginScene()
{
	JUMP(Vd_GLI_DRV_bBeginScene);
}

EXPORT NAKED BOOL GLI_DRV_bEndScene()
{
	JUMP(Vd_GLI_DRV_bEndScene);
}

EXPORT NAKED BOOL GLI_DRV_bLockDevice( DWORD *a1, DWORD *a2 )
{
	JUMP(Vd_GLI_DRV_bLockDevice);
}

EXPORT NAKED BOOL GLI_DRV_bUnlockDevice()
{
	JUMP(Vd_GLI_DRV_bUnlockDevice);
}

EXPORT NAKED void GLI_DRV_vClearDevice( int a1, int a2, int a3 )
{
	JUMP(Vd_GLI_DRV_vClearDevice);
}

EXPORT NAKED void GLI_DRV_vFlipDevice( int waitFrames )
{
	JUMP(Vd_GLI_DRV_vFlipDevice);
}

EXPORT NAKED void GLI_DRV_vFlipDeviceWithSyncro()
{
	JUMP(Vd_GLI_DRV_vFlipDeviceWithSyncro);
}

// TODO: remove this, testing alternative framerate fix
/*
EXPORT void GLI_DRV_vFlipDeviceWithSyncro()
{
	GLI_DRV_vFlipDevice(1);
}
*/

EXPORT NAKED void GLI_DRV_vDownLoadTextures( int a1, int a2, int a3 )
{
	JUMP(Vd_GLI_DRV_vDownLoadTextures);
}

EXPORT NAKED void GLI_DRV_vUnLoadTextures()
{
	JUMP(Vd_GLI_DRV_vUnLoadTextures);
}

EXPORT NAKED int GLI_DRV_lGetSizeOfTexture( void *a1 )
{
	JUMP(Vd_GLI_DRV_lGetSizeOfTexture);
}

EXPORT NAKED void GLI_DRV_vDoOpaqueTextureSelection( int a1 )
{
	JUMP(Vd_GLI_DRV_vDoOpaqueTextureSelection);
}

EXPORT NAKED HANDLE GLI_DRV_hChangeMode( BOOL bFullscreen, int xRight, int yBottom, int bitDepth )
{
	JUMP(Vd_GLI_DRV_hChangeMode);
}

EXPORT NAKED BOOL GLI_DRV_bWindowedModeIsOptimized()
{
	JUMP(Vd_GLI_DRV_bWindowedModeIsOptimized);
}

EXPORT NAKED void GLI_DRV_vOptimizedWindowedMode()
{
	JUMP(Vd_GLI_DRV_vOptimizedWindowedMode);
}

EXPORT NAKED void GLI_DRV_vNonOptimizedWindowedMode()
{
	JUMP(Vd_GLI_DRV_vNonOptimizedWindowedMode);
}

EXPORT NAKED BOOL GLI_DRV_bPrepareForGliWindowed( HWND hWnd )
{
	JUMP(Vd_GLI_DRV_bPrepareForGliWindowed);
}

EXPORT NAKED void GLI_DRV_vPrepareForGliFullScreen( HWND hWnd )
{
	JUMP(Vd_GLI_DRV_vPrepareForGliFullScreen);
}

EXPORT NAKED void GLI_DRV_vActivateGli( HWND hWnd, BOOL a2 )
{
	JUMP(Vd_GLI_DRV_vActivateGli);
}

EXPORT NAKED void GLI_DRV_vReadaptDisplay()
{
	JUMP(Vd_GLI_DRV_vReadaptDisplay);
}

EXPORT NAKED void GLI_DRV_vAddBlackPolygon( int a1, int a2, int a3, int a4 )
{
	JUMP(Vd_GLI_DRV_vAddBlackPolygon);
}

EXPORT NAKED void GLI_DRV_vNoBlackPolygon()
{
	JUMP(Vd_GLI_DRV_vNoBlackPolygon);
}

EXPORT NAKED void GLI_DRV_vSetZClip( float a1, int a2 )
{
	JUMP(Vd_GLI_DRV_vSetZClip);
}

EXPORT NAKED void GLI_DRV_vSetClipWindow( float a1, int a2, int a3, int a4, int a5 )
{
	JUMP(Vd_GLI_DRV_vSetClipWindow);
}

EXPORT NAKED void GLI_DRV_vSendSingleLineToClip( int a1, int a2, int a3, int a4, int a5 )
{
	JUMP(Vd_GLI_DRV_vSendSingleLineToClip);
}

EXPORT NAKED void GLI_DRV_vSendSpriteToClip( int a1, int a2, int a3 )
{
	JUMP(Vd_GLI_DRV_vSendSpriteToClip);
}

EXPORT NAKED void GLI_DRV_vSendSpriteToClipWithColors( int a1, int a2, int a3, int a4 )
{
	JUMP(Vd_GLI_DRV_vSendSpriteToClipWithColors);
}

EXPORT NAKED void GLI_DRV_vSendSpriteToClipWithUV( int a1, int a2, int a3, int a4 )
{
	JUMP(Vd_GLI_DRV_vSendSpriteToClipWithUV);
}

EXPORT NAKED int GLI_DRV_xSendElementTIToClip_TRIANGLES( int a1, int a2 )
{
	JUMP(Vd_GLI_DRV_xSendElementTIToClip_TRIANGLES);
}

EXPORT NAKED int GLI_DRV_xSendSingleTriangleToClip_TRIANGLES( int a1, int a2, int a3 )
{
	JUMP(Vd_GLI_DRV_xSendSingleTriangleToClip_TRIANGLES);
}

EXPORT NAKED DWORD GLI_DRV_xClearViewingList()
{
	JUMP(Vd_GLI_DRV_xClearViewingList);
}

EXPORT NAKED DWORD GLI_DRV_xSendListToViewport()
{
	JUMP(Vd_GLI_DRV_xSendListToViewport);
}

EXPORT NAKED void GLI_DRV_vClearZBufferRegion()
{
	JUMP(Vd_GLI_DRV_vClearZBufferRegion);
}

EXPORT NAKED void GLI_DRV_vComputeFogEffect( int a1 )
{
	JUMP(Vd_GLI_DRV_vComputeFogEffect);
}

EXPORT NAKED void GLI_DRV_vWrite16bBitmapToBackBuffer( int a1, int a2, int a3, int a4, int a5, int a6, int a7 )
{
	JUMP(Vd_GLI_DRV_vWrite16bBitmapToBackBuffer);
}
