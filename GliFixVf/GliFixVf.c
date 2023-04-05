#include "framework.h"
#include "GliFixVf.h"
#include "config.h"
#include "devinfo.h"
#include "imports.h"


tdstDevCaps *g_p_stCaps = NULL;

int (*p_fn_lAddDiplayMode)( BOOL bFullscreen, int x, int y, int lBitDepth ) = NULL;
int (*p_fn_lComputeWaitFrameForSmoothSynchro)( int ) = NULL;
int (*p_fn_xIsGliInit)( void ) = NULL;


void fn_vCopyString( char *dst, const char *src )
{
	size_t len = strlen(src) + 1;
	strcpy_s(dst, len, src);
}


/*
 * Exported functions
 */

// Copies renderer info (first listbox in GxSetup) to specified memory address
BOOL GLI_DRV_lGetDllInfo( const char *szType, void *lpDst )
{
	if ( !strcmp(szType, "Name") )
	{
		fn_vCopyString(lpDst, GLI_szName);
	}
	else if ( !strcmp(szType, "Description") )
	{
		fn_vCopyString(lpDst, GLI_szDesc);
	}
	else if ( !strcmp(szType, "Version") )
	{
		fn_vCopyString(lpDst, GLI_szVersion);
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
BOOL GLI_DRV_fn_lGetAllDisplayConfig( tdfnGliSet p_fn_vGliSet )
{
	int disp, dev, mode;
	tdstDevCaps stCaps = { 0 };

	p_fn_vGliSet(0, 0, 0, GS_BMP, 0);

	// display info
	disp = p_fn_vGliSet(0, 0, 0, GS_ADD_DISPLAY, NULL);
	p_fn_vGliSet(disp, 0, 0, GS_DISPLAY_NAME, "Default");
	p_fn_vGliSet(disp, 0, 0, GS_DISPLAY_DESC, "Default");

	// device info
	dev = p_fn_vGliSet(disp, 0, 0, GS_ADD_DEVICE, NULL);
	p_fn_vGliSet(disp, dev, 0, GS_DEVICE_NAME, "Default");
	p_fn_vGliSet(disp, dev, 0, GS_DEVICE_DESC, "Default");

	// display mode (resolution)
	mode = p_fn_vGliSet(disp, dev, 0, GS_ADD_MODE, NULL);
	p_fn_vGliSet(disp, dev, mode, GS_MODE_FULLSCREEN, 1);
	p_fn_vGliSet(disp, dev, mode, GS_MODE_BITDEPTH, 16);
	p_fn_vGliSet(disp, dev, mode, GS_MODE_WIDTH, CFG_stDispMode.dwWidth);
	p_fn_vGliSet(disp, dev, mode, GS_MODE_HEIGHT, CFG_stDispMode.dwHeight);

	p_fn_vGliSet(disp, dev, 0, "dev_caps", &stCaps);


	/* If fix is enabled, always open R2FixCfg,
	   if fix is disabled, ask first */
	if ( !CFG_bIsFixEnabled )
	{
		int lResult = MessageBox(
			NULL, "Ray2Fix is installed but not enabled.\nDo you want to open Ray2Fix Settings?",
			GLI_szName, MB_YESNO | MB_ICONASTERISK);

		if ( lResult != IDYES )
			return TRUE;
	}

	if ( CFG_fn_bOpenConfigTool() )
		ExitProcess(0);

	MessageBox(NULL, "Cannot open Ray2Fix Settings, please update or reinstall Ray2Fix.",
			   GLI_szName, MB_OK | MB_ICONERROR);

	return TRUE;
}

BOOL GLI_DRV_lSetCommonData( const char *szName, void *value )
{
	if ( !strcmp(szName, "GliCaps") )
	{
		g_p_stCaps = value;
	}
	return Vd_GLI_DRV_lSetCommonData(szName, value);
}

BOOL GLI_DRV_lSetCommonFct( const char *szName, tdfnCommonFct pFct )
{
	if ( !strcmp(szName, "AddDisplayMode") ) /* for GLI_DRV_fnl_EnumModes */
		p_fn_lAddDiplayMode = pFct;
	else if ( !strcmp(szName, "IsGliInit") ) /* for GLI_DRV_vFlipDeviceWithSyncro */
		p_fn_xIsGliInit = pFct;
	else if ( !strcmp(szName, "ComputeWaitFrameForSmoothSynchro") )
		p_fn_lComputeWaitFrameForSmoothSynchro = pFct;

	return Vd_GLI_DRV_lSetCommonFct(szName, pFct);
}

BOOL GLI_DRV_fnl_EnumModes( char *szDrvDspName, char *szDevName )
{
	p_fn_lAddDiplayMode(TRUE, CFG_stDispMode.dwWidth, CFG_stDispMode.dwHeight, 16);

	return TRUE;
}

DWORD GLI_DRV_xInitDriver( HWND hWnd, BOOL bFullscreen, int xRight, int yBottom, int lBitDepth )
{
	DWORD dwResult = Vd_GLI_DRV_xInitDriver(hWnd, bFullscreen, xRight, yBottom, lBitDepth);

	// HACK: make sure the window has a title bar and a border
	// Sometimes the game and/or dgVoodoo bugs out for no reason and displays the game
	// without a title bar, making it impossible to move or resize the game window.
	int lStyle = GetWindowLong(hWnd, GWL_STYLE);
	SetWindowLong(hWnd, GWL_STYLE, lStyle | WS_OVERLAPPEDWINDOW);

	// HACK: Refresh rate fix for >60Hz monitors
	g_p_stCaps->xRefreshRate = CFG_bHalfRefRate ? 30.0f : 60.0f;

	return dwResult;
}

void GLI_DRV_vFlipDeviceWithSyncro()
{
	if ( !p_fn_xIsGliInit() )
		return;
	
	int lWaitFrame = p_fn_lComputeWaitFrameForSmoothSynchro(0);
	GLI_DRV_vFlipDevice(lWaitFrame);
}


/*
 * Redirected exports
 */

NAKED void GLI_DRV_vCloseDriver()
{
	JMP(Vd_GLI_DRV_vCloseDriver);
}

NAKED BOOL GLI_DRV_bBeginScene()
{
	JMP(Vd_GLI_DRV_bBeginScene);
}

NAKED BOOL GLI_DRV_bEndScene()
{
	JMP(Vd_GLI_DRV_bEndScene);
}

NAKED BOOL GLI_DRV_bLockDevice( DWORD *a1, DWORD *a2 )
{
	JMP(Vd_GLI_DRV_bLockDevice);
}

NAKED BOOL GLI_DRV_bUnlockDevice()
{
	JMP(Vd_GLI_DRV_bUnlockDevice);
}

NAKED void GLI_DRV_vClearDevice( int a1, int a2, int a3 )
{
	JMP(Vd_GLI_DRV_vClearDevice);
}

NAKED void GLI_DRV_vFlipDevice( int lWaitFrames )
{
	JMP(Vd_GLI_DRV_vFlipDevice);
}

/*
NAKED void GLI_DRV_vFlipDeviceWithSyncro()
{
	JMP(Vd_GLI_DRV_vFlipDeviceWithSyncro);
}
*/

NAKED void GLI_DRV_vDownLoadTextures( int a1, int a2, int a3 )
{
	JMP(Vd_GLI_DRV_vDownLoadTextures);
}

NAKED void GLI_DRV_vUnLoadTextures()
{
	JMP(Vd_GLI_DRV_vUnLoadTextures);
}

NAKED int GLI_DRV_lGetSizeOfTexture( void *a1 )
{
	JMP(Vd_GLI_DRV_lGetSizeOfTexture);
}

NAKED void GLI_DRV_vDoOpaqueTextureSelection( int a1 )
{
	JMP(Vd_GLI_DRV_vDoOpaqueTextureSelection);
}

NAKED HANDLE GLI_DRV_hChangeMode( BOOL bFullscreen, int xRight, int yBottom, int bitDepth )
{
	JMP(Vd_GLI_DRV_hChangeMode);
}

NAKED BOOL GLI_DRV_bWindowedModeIsOptimized()
{
	JMP(Vd_GLI_DRV_bWindowedModeIsOptimized);
}

NAKED void GLI_DRV_vOptimizedWindowedMode()
{
	JMP(Vd_GLI_DRV_vOptimizedWindowedMode);
}

NAKED void GLI_DRV_vNonOptimizedWindowedMode()
{
	JMP(Vd_GLI_DRV_vNonOptimizedWindowedMode);
}

NAKED BOOL GLI_DRV_bPrepareForGliWindowed( HWND hWnd )
{
	JMP(Vd_GLI_DRV_bPrepareForGliWindowed);
}

NAKED void GLI_DRV_vPrepareForGliFullScreen( HWND hWnd )
{
	JMP(Vd_GLI_DRV_vPrepareForGliFullScreen);
}

NAKED void GLI_DRV_vActivateGli( HWND hWnd, BOOL a2 )
{
	JMP(Vd_GLI_DRV_vActivateGli);
}

NAKED void GLI_DRV_vReadaptDisplay()
{
	JMP(Vd_GLI_DRV_vReadaptDisplay);
}

NAKED void GLI_DRV_vAddBlackPolygon( int a1, int a2, int a3, int a4 )
{
	JMP(Vd_GLI_DRV_vAddBlackPolygon);
}

NAKED void GLI_DRV_vNoBlackPolygon()
{
	JMP(Vd_GLI_DRV_vNoBlackPolygon);
}

NAKED void GLI_DRV_vSetZClip( float a1, int a2 )
{
	JMP(Vd_GLI_DRV_vSetZClip);
}

NAKED void GLI_DRV_vSetClipWindow( float a1, int a2, int a3, int a4, int a5 )
{
	JMP(Vd_GLI_DRV_vSetClipWindow);
}

NAKED void GLI_DRV_vSendSingleLineToClip( int a1, int a2, int a3, int a4, int a5 )
{
	JMP(Vd_GLI_DRV_vSendSingleLineToClip);
}

NAKED void GLI_DRV_vSendSpriteToClip( int a1, int a2, int a3 )
{
	JMP(Vd_GLI_DRV_vSendSpriteToClip);
}

NAKED void GLI_DRV_vSendSpriteToClipWithColors( int a1, int a2, int a3, int a4 )
{
	JMP(Vd_GLI_DRV_vSendSpriteToClipWithColors);
}

NAKED void GLI_DRV_vSendSpriteToClipWithUV( int a1, int a2, int a3, int a4 )
{
	JMP(Vd_GLI_DRV_vSendSpriteToClipWithUV);
}

NAKED int GLI_DRV_xSendElementTIToClip_TRIANGLES( int a1, int a2 )
{
	JMP(Vd_GLI_DRV_xSendElementTIToClip_TRIANGLES);
}

NAKED int GLI_DRV_xSendSingleTriangleToClip_TRIANGLES( int a1, int a2, int a3 )
{
	JMP(Vd_GLI_DRV_xSendSingleTriangleToClip_TRIANGLES);
}

NAKED DWORD GLI_DRV_xClearViewingList()
{
	JMP(Vd_GLI_DRV_xClearViewingList);
}

NAKED DWORD GLI_DRV_xSendListToViewport()
{
	JMP(Vd_GLI_DRV_xSendListToViewport);
}

NAKED void GLI_DRV_vClearZBufferRegion()
{
	JMP(Vd_GLI_DRV_vClearZBufferRegion);
}

NAKED void GLI_DRV_vComputeFogEffect( int a1 )
{
	JMP(Vd_GLI_DRV_vComputeFogEffect);
}

NAKED void GLI_DRV_vWrite16bBitmapToBackBuffer( int a1, int a2, int a3, int a4, int a5, int a6, int a7 )
{
	JMP(Vd_GLI_DRV_vWrite16bBitmapToBackBuffer);
}
