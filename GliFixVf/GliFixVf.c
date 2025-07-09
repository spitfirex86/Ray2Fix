#include "framework.h"
#include "GliFixVf.h"
#include "config.h"
#include "devinfo.h"
#include "imports.h"
#include "glide2types.h"

#include <ACP_Ray2.h>

#define SYNCROTEST


tdstGliCaps *g_p_stCaps = NULL;

int (*GLI_MDRV_lAddDisplayMode)( BOOL bFullscreen, int x, int y, int lBitDepth ) = NULL;
int (*GLI_MDRV_lComputeWaitFrameForSmoothSynchro)( int ) = NULL;
int (*GLI_MDRV_xIsGliInit)( void ) = NULL;
void (*GLI_MDRV_vSerialProjection)( GLI_tdstCamera *p_stCam, long lNbOfVertex, GLI_tdstAligned3DVector *p_stSource, GLI_tdstAligned2DVector *p_stDest ) = NULL;


GrVertex CurrentDestXYZ[4];

#define GLI_C_3dfxBugConstant   65536.0f * 8.0f

#define GLI_M_Correct3DFXBug2( a )	\
{									\
	a.x -= GLI_C_3dfxBugConstant;	\
	a.y -= GLI_C_3dfxBugConstant;	\
}

void GLI_fn_vCorrect3DFXBug1( GrVertex *a )
{
	a->x += GLI_C_3dfxBugConstant;
	a->y += GLI_C_3dfxBugConstant;
}

#define GLI_M_Restore3DFXColor( P, ulColor )									\
{																				\
	P.b = (float) ((ulColor) & 0xFF);											\
	P.g = (float) (((ulColor) >> 8) & 0xFF);									\
	P.r = (float) (((ulColor) >> 16) & 0xFF);									\
	P.a = (float) (((ulColor) >> 24) & 0xFF);									\
	P.tmuvtx[*Vd_GLI_GLIDE1_xTmuNumber].sow *= P.oow * p_stGlobaleMT->fMulV;	\
	P.tmuvtx[*Vd_GLI_GLIDE1_xTmuNumber].tow *= P.oow * p_stGlobaleMT->fMulU;	\
}


#define M_CopyString(dst, src) strcpy_s(dst, strlen(src)+1, src)

/* copies renderer info (first listbox in GxSetup) to specified memory address */
long GLI_DRV_lGetDllInfo( char const *szName, void *pData )
{
	if ( !strcmp(szName, "Name") )
		M_CopyString(pData, GLI_szName);
	else if ( !strcmp(szName, "Description") )
		M_CopyString(pData, GLI_szDesc);
	else if ( !strcmp(szName, "Version") )
		M_CopyString(pData, GLI_szVersion);
	else if ( !strcmp(szName, "Code") )
		*(DWORD *)pData = GLI_lCode;
	else
		return 0;

	return 1;
}

long GLI_DRV_lSetCommonData( char const *szName, void *pData )
{
	if ( !strcmp(szName, "GliCaps") )
		g_p_stCaps = pData;

	return Vd_GLI_DRV_lSetCommonData(szName, pData);
}

#ifdef SYNCROTEST

long fakeComputeFrame( long a )
{
	return 1;
}

short *GLD_gs_wTimerForSmoothSyncro = 0x49FC50;
unsigned long *GLI_g_ulFrameLength = 0x4A7218;

long FIX_GLD_fn_lComputeWaitFrameForSmoothSynchro( long _a1 )
{
#define C_ulFrameRateHistorySize 5
	static long gas_ulFrameRateHistory[] =
	{ 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0 };
	static unsigned long    ulTotalFrameRate = 0;
	static unsigned long    ulCurrentFrame = 0;
	TMR_tdstTimerCount            stDeltaTime;
	unsigned long           ulTramePassed;
	unsigned long           ulTrameReturn = 1;

	TMR_fn_wReadTimer(*GLD_gs_wTimerForSmoothSyncro, &stDeltaTime);

	/* Compute duration passed for engine + rendering (rounded up) */
	ulTramePassed = (stDeltaTime.ulLowPart + ((*GLI_g_ulFrameLength * 95) / 100)) / *GLI_g_ulFrameLength;

	/* Too slow. We don't do anything */
	if ( ulTramePassed > 4 )
		return 1;

	/* Compute number of frames to wait for */
	if ( ulTramePassed << C_ulFrameRateHistorySize < ulTotalFrameRate )
		ulTrameReturn =
		1
		+ ((ulTotalFrameRate + (1 << C_ulFrameRateHistorySize) - 1) >> C_ulFrameRateHistorySize)
		- ulTramePassed;

	/* Update frame history */
	ulTotalFrameRate -= gas_ulFrameRateHistory[ulCurrentFrame];
	ulTotalFrameRate += ulTramePassed;
	gas_ulFrameRateHistory[ulCurrentFrame] = ulTramePassed;
	ulCurrentFrame++;
	ulCurrentFrame &= (1 << C_ulFrameRateHistorySize) - 1;

	return (long)ulTrameReturn;
}

long fakeComputeFrame2( long a )
{
	int *dword_4A722C = (int*)0x4A722C;
	int *dword_4A7230 = (int*)0x4A7230;

	//long lFrame = fakeComputeFrame(0);
	long lFrame = GLI_MDRV_lComputeWaitFrameForSmoothSynchro(a);
#ifdef DEBUGCONSOLE
	printf_s(".");
	if ( lFrame > 1 )
		printf_s("\n== Skipping frames: %u, 0x4A722C = %d, 0x4A7230 = %d\n", lFrame, *dword_4A722C, *dword_4A7230);
#endif // DEBUGCONSOLE

	return lFrame;
}

#endif // SYNCROTEST

long GLI_DRV_lSetCommonFct( char const *szName, tdfn_CommonFct pData )
{
	if ( !strcmp(szName, "AddDisplayMode") ) /* for GLI_DRV_fnl_EnumModes */
		GLI_MDRV_lAddDisplayMode = pData;
	else if ( !strcmp(szName, "IsGliInit") ) /* for GLI_DRV_vFlipDeviceWithSyncro */
		GLI_MDRV_xIsGliInit = pData;
	else if ( !strcmp(szName, "ComputeWaitFrameForSmoothSynchro") )
	{
		GLI_MDRV_lComputeWaitFrameForSmoothSynchro = pData;
#ifdef SYNCROTEST
		return Vd_GLI_DRV_lSetCommonFct(szName, fakeComputeFrame);
		//return Vd_GLI_DRV_lSetCommonFct(szName, fakeComputeFrame2);
#endif // SYNCROTEST
	}
	else if ( !strcmp(szName, "SerialProjection") ) /* for GLI_DRV_vSendSingleLineToClip */
		GLI_MDRV_vSerialProjection = pData;

	return Vd_GLI_DRV_lSetCommonFct(szName, pData);
}

/* sets capability flags and populates displays, devices and resolutions */
long GLI_DRV_fn_lGetAllDisplayConfig( tdfn_lAddDisplayInfo p_fn_lAddDisplayInfo )
{
	long lDisp, lDev, lMode;
	tdstGliCaps stCaps = { 0 };

	if ( CFG_eBackend == E_GL_DirectX )
	{
		/* defer to DX6 lib */
		Vd_GLI_DRV_fn_lGetAllDisplayConfig(p_fn_lAddDisplayInfo);
		p_fn_lAddDisplayInfo(0, 0, 0, C_DI_DllBmp, 0);
	}
	else
	{
		p_fn_lAddDisplayInfo(0, 0, 0, C_DI_DllBmp, 0);

		/* display info */
		lDisp = p_fn_lAddDisplayInfo(0, 0, 0, C_DI_DisplayAdd, 0);
		p_fn_lAddDisplayInfo(lDisp, 0, 0, C_DI_DisplayName, (long)"display");
		p_fn_lAddDisplayInfo(lDisp, 0, 0, C_DI_DisplayDesc, (long)"Default");

		/* device info */
		lDev = p_fn_lAddDisplayInfo(lDisp, 0, 0, C_DI_DeviceAdd, 0);
		p_fn_lAddDisplayInfo(lDisp, lDev, 0, C_DI_DeviceName, (long)"Direct3D HAL");
		p_fn_lAddDisplayInfo(lDisp, lDev, 0, C_DI_DeviceDesc, (long)"Default");

		/* display mode (resolution) */
		lMode = p_fn_lAddDisplayInfo(lDisp, lDev, 0, C_DI_ModeAdd, 0);
		p_fn_lAddDisplayInfo(lDisp, lDev, lMode, C_DI_ModeFullscreen, 1);
		p_fn_lAddDisplayInfo(lDisp, lDev, lMode, C_DI_ModeBpp, 16);
		p_fn_lAddDisplayInfo(lDisp, lDev, lMode, C_DI_ModeWidth, CFG_stDispMode.dwWidth);
		p_fn_lAddDisplayInfo(lDisp, lDev, lMode, C_DI_ModeHeight, CFG_stDispMode.dwHeight);

		p_fn_lAddDisplayInfo(lDisp, lDev, 0, C_DI_DevCaps, (long)&stCaps);
	}

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

long GLI_DRV_fnl_EnumModes( char *szDriverName, char *szDeviceName )
{
	if ( CFG_eBackend == E_GL_DirectX )
	{
		/* defer to DX6 lib */
		Vd_GLI_DRV_fnl_EnumModes(szDriverName, szDeviceName);
		GLI_MDRV_lAddDisplayMode(TRUE, CFG_stDispMode.dwWidth, CFG_stDispMode.dwHeight, 16);
	}
	else
	{
		GLI_MDRV_lAddDisplayMode(TRUE, CFG_stDispMode.dwWidth, CFG_stDispMode.dwHeight, 16);
	}

	return TRUE;
}

void GLI_DRV_xInitDriver( HWND hWnd, BOOL bFullscreen, long lWidth, long lHeight, int lBpp )
{
	Vd_GLI_DRV_xInitDriver(hWnd, bFullscreen, lWidth, lHeight, lBpp);

	/* HACK: make sure the window has a title bar and a border
	   Sometimes the game and/or dgVoodoo bugs out for no reason and displays the game
	   without a title bar, making it impossible to move or resize the game window. */
	int lStyle = GetWindowLong(hWnd, GWL_STYLE) | WS_OVERLAPPEDWINDOW;
	int lExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
	SetWindowLong(hWnd, GWL_STYLE, lStyle);

	char szTitle[64];
	sprintf_s(szTitle, sizeof(szTitle), "Rayman II    [%s %s]", GLI_szName, GLI_szVersion);
	SetWindowText(hWnd, szTitle);

	RECT stWinRect = { 0, 0, CFG_stActualDispMode.dwWidth, CFG_stActualDispMode.dwHeight };
	AdjustWindowRectEx(&stWinRect, lStyle, FALSE, lExStyle);
	SetWindowPos(hWnd, NULL, 0, 0, stWinRect.right - stWinRect.left, stWinRect.bottom - stWinRect.top, SWP_NOMOVE | SWP_NOZORDER);

	/* HACK: Refresh rate fix for >60Hz monitors */
	g_p_stCaps->xRefreshRate = CFG_bHalfRefRate ? 30.0f : 60.0f;
	//g_p_stCaps->ulDriverCaps |= 0x40;
}

void GLI_DRV_vFlipDeviceWithSyncro( void )
{
	if ( !GLI_MDRV_xIsGliInit() )
		return;

#ifdef SYNCROTEST
	Vd_GLI_DRV_vFlipDeviceWithSyncro();
	//GLI_DRV_vFlipDevice(fakeComputeFrame2(0));

	return;
#else

	int lWaitFrame = ( CFG_DEBUG_lWaitFrame > 0 )
		? CFG_DEBUG_lWaitFrame
		: GLI_MDRV_lComputeWaitFrameForSmoothSynchro(0);
	GLI_DRV_vFlipDevice(lWaitFrame);
#endif // SYNCROTEST
}

void GLI_DRV_vSendSingleLineToClip(
	GLD_tdstViewportAttributes *p_stVpt,
	GLI_tdstAligned3DVector *p_stVertex1,
	GLI_tdstAligned2DVector *p_st2DVertex1,
	GLI_tdstAligned3DVector *p_stVertex2,
	GLI_tdstAligned2DVector *p_st2DVertex2,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT,
	long lDrawModeMask,
	GEO_tdstColor *p_stColor
)
{
	if ( CFG_eBackend != E_GL_Glide || IMP_cWhatBuildWeUsing != 'f' ) /* not glide or not final = ignore */
	{
		Vd_GLI_DRV_vSendSingleLineToClip(p_stVpt, p_stVertex1, p_st2DVertex1, p_stVertex2, p_st2DVertex2, p_stGlobaleMT, lDrawModeMask, p_stColor);
		return;
	}

	/* final has an annoying bug that has to be fixed... (for mod support) */

	GLI_tdstAligned2DVector stP1, stP2;
	GLI_tdstAligned3DVector st3DP1, st3DP2;
	float fFactor;

	grAlphaCombine(GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_ONE, GR_COMBINE_LOCAL_ITERATED, GR_COMBINE_OTHER_ITERATED, FXFALSE);
	grAlphaBlendFunction(GR_BLEND_ONE, GR_BLEND_ZERO, GR_BLEND_ONE, GR_BLEND_ZERO);
	guColorCombineFunction(GR_COLORCOMBINE_ITRGB);

	/* Z clipping */
	if ( p_st2DVertex1->xOoZ < p_st2DVertex2->xOoZ )
	{
		stP1 = *p_st2DVertex1;
		st3DP1 = *p_stVertex1;
		stP2 = *p_st2DVertex2;
		st3DP2 = *p_stVertex2;
	}
	else
	{
		stP2 = *p_st2DVertex1;
		st3DP2 = *p_stVertex1;
		stP1 = *p_st2DVertex2;
		st3DP1 = *p_stVertex2;
	}

	if ( (*(unsigned long *)&stP2.xOoZ & 0x80000000) )
		return;

	if ( (*(unsigned long *)&stP1.xOoZ & 0x80000000) )
	{
		fFactor = (p_stGlobaleMT->p_stCurrentCamera->xNear - st3DP2.xZ) / (st3DP1.xZ - st3DP2.xZ);
		st3DP1.xX = st3DP2.xX + (st3DP1.xX - st3DP2.xX) * fFactor;
		st3DP1.xY = st3DP2.xY + (st3DP1.xY - st3DP2.xY) * fFactor;
		st3DP1.xZ = p_stGlobaleMT->fZClipping;
		GLI_MDRV_vSerialProjection(p_stGlobaleMT->p_stCurrentCamera, 1, &st3DP1, &stP1);
	}

	/* X clipping */
	if ( stP1.xX < stP2.xX )
	{
		p_st2DVertex1 = &stP1;
		p_stVertex1 = &st3DP1;
		p_st2DVertex2 = &stP2;
		p_stVertex2 = &st3DP2;
	}
	else
	{
		p_st2DVertex1 = &stP2;
		p_stVertex1 = &st3DP2;
		p_st2DVertex2 = &stP1;
		p_stVertex2 = &st3DP1;
	}

	if ( (p_st2DVertex2->xX < p_stGlobaleMT->fXMinClipping) || (p_st2DVertex1->xX > p_stGlobaleMT->fXMaxClipping) )
		return;

	if ( p_st2DVertex1->xX < p_stGlobaleMT->fXMinClipping )
	{
		fFactor = (p_st2DVertex2->xX - p_stGlobaleMT->fXMinClipping) / (p_st2DVertex2->xX - p_st2DVertex1->xX);
		p_st2DVertex1->xY = p_st2DVertex2->xY - (p_st2DVertex2->xY - p_st2DVertex1->xY) * fFactor;
		p_stVertex1->xZ = p_stVertex2->xZ - (p_stVertex2->xY - p_stVertex1->xY) * fFactor;
		p_st2DVertex1->xX = p_stGlobaleMT->fXMinClipping;
	}

	if ( p_st2DVertex2->xX > p_stGlobaleMT->fXMaxClipping )
	{
		fFactor = (p_st2DVertex1->xX - p_stGlobaleMT->fXMaxClipping) / (p_st2DVertex2->xX - p_st2DVertex1->xX);
		p_st2DVertex2->xY = p_st2DVertex1->xY - (p_st2DVertex2->xY - p_st2DVertex1->xY) * fFactor;
		p_stVertex2->xZ = p_stVertex1->xZ - (p_stVertex2->xY - p_stVertex1->xY) * fFactor;
		p_st2DVertex2->xX = p_stGlobaleMT->fXMaxClipping;
	}

	/* Y clipping */
	if ( stP1.xY < stP2.xY )
	{
		p_st2DVertex1 = &stP1;
		p_stVertex1 = &st3DP1;
		p_st2DVertex2 = &stP2;
		p_stVertex2 = &st3DP2;
	}
	else
	{
		p_st2DVertex1 = &stP2;
		p_stVertex1 = &st3DP2;
		p_st2DVertex2 = &stP1;
		p_stVertex2 = &st3DP1;
	}

	if ( (p_st2DVertex2->xY < p_stGlobaleMT->fYMinClipping) || (p_st2DVertex1->xY > p_stGlobaleMT->fYMaxClipping) )
		return;

	if ( p_st2DVertex1->xY < p_stGlobaleMT->fYMinClipping )
	{
		fFactor = (p_st2DVertex2->xY - p_stGlobaleMT->fYMinClipping) / (p_st2DVertex2->xY - p_st2DVertex1->xY);
		p_st2DVertex1->xX = p_st2DVertex2->xX - (p_st2DVertex2->xX - p_st2DVertex1->xX) * fFactor;
		p_stVertex1->xZ = p_stVertex2->xZ - (p_stVertex2->xZ - p_stVertex1->xZ) * fFactor;
		p_st2DVertex1->xY = p_stGlobaleMT->fYMinClipping;
	}

	if ( p_st2DVertex2->xY > p_stGlobaleMT->fYMaxClipping )
	{
		fFactor = (p_st2DVertex1->xY - p_stGlobaleMT->fYMaxClipping) / (p_st2DVertex2->xY - p_st2DVertex1->xY);
		p_st2DVertex2->xX = p_st2DVertex1->xX - (p_st2DVertex2->xX - p_st2DVertex1->xX) * fFactor;
		p_stVertex2->xZ = p_stVertex1->xZ - (p_stVertex2->xZ - p_stVertex1->xZ) * fFactor;
		p_st2DVertex2->xY = p_stGlobaleMT->fYMaxClipping;
	}

	/* GLI_M_InitLine */
	CurrentDestXYZ[0].x = p_st2DVertex1->xX;
	CurrentDestXYZ[0].y = p_st2DVertex1->xY;
	CurrentDestXYZ[1].x = p_st2DVertex2->xX;
	CurrentDestXYZ[1].y = p_st2DVertex2->xY;
	CurrentDestXYZ[0].oow = p_stVertex1->xZ;
	CurrentDestXYZ[1].oow = p_stVertex2->xZ;
	//*(unsigned long *)&CurrentDestXYZ[0].r = p_st2DVertex1->ulPackedColor;
	//*(unsigned long *)&CurrentDestXYZ[1].r = p_st2DVertex2->ulPackedColor;

	/* GLI_DrawLine */
	GLI_fn_vCorrect3DFXBug1(&CurrentDestXYZ[0]);
	GLI_fn_vCorrect3DFXBug1(&CurrentDestXYZ[1]);
	GLI_M_Correct3DFXBug2(CurrentDestXYZ[0]);
	GLI_M_Correct3DFXBug2(CurrentDestXYZ[1]);
	GLI_M_Restore3DFXColor(CurrentDestXYZ[0], p_st2DVertex1->ulPackedColor);
	GLI_M_Restore3DFXColor(CurrentDestXYZ[1], p_st2DVertex2->ulPackedColor);

	grDrawLine(&CurrentDestXYZ[0], &CurrentDestXYZ[1]);
}


/*
 * Redirected exports
 */


/****  INIT  ****/

NAKED void GLI_DRV_vFlipDevice( int lWaitFrames )
{
	JMP(Vd_GLI_DRV_vFlipDevice);
}

NAKED void GLI_DRV_vClearDevice( BOOL bZBuffer, BOOL bColorBuffer, unsigned long ulColor )
{
	JMP(Vd_GLI_DRV_vClearDevice);
}

/*
NAKED void GLI_DRV_vFlipDeviceWithSyncro( void )
{
	JMP(Vd_GLI_DRV_vFlipDeviceWithSyncro);
}
*/

NAKED void GLI_DRV_vCloseDriver( void )
{
	JMP(Vd_GLI_DRV_vCloseDriver);
}

NAKED void GLI_DRV_vClearZBufferRegion( long lXStart, long lXEnd, long lYStart, long lYEnd )
{
	JMP(Vd_GLI_DRV_vClearZBufferRegion);
}

NAKED long GLI_DRV_hChangeMode( BOOL bFullscreen, long lWidth, long lHeight, long lBpp )
{
	JMP(Vd_GLI_DRV_hChangeMode);
}

NAKED BOOL GLI_DRV_bLockDevice( void **pp_vVirtualScreen, long *p_lPitch )
{
	JMP(Vd_GLI_DRV_bLockDevice);
}

NAKED BOOL GLI_DRV_bUnlockDevice( void )
{
	JMP(Vd_GLI_DRV_bUnlockDevice);
}

NAKED BOOL GLI_DRV_bWindowedModeIsOptimized( void )
{
	JMP(Vd_GLI_DRV_bWindowedModeIsOptimized);
}

NAKED void GLI_DRV_vOptimizedWindowedMode( void )
{
	JMP(Vd_GLI_DRV_vOptimizedWindowedMode);
}

NAKED void GLI_DRV_vNonOptimizedWindowedMode( void )
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

NAKED void GLI_DRV_vActivateGli( HWND hWnd, BOOL bActivate )
{
	JMP(Vd_GLI_DRV_vActivateGli);
}

NAKED void GLI_DRV_vReadaptDisplay( void )
{
	JMP(Vd_GLI_DRV_vReadaptDisplay);
}


/****  HDWTEX  ****/

NAKED void GLI_DRV_vDownLoadTextures( long bRestore, long lTextureMode, BOOL bReloading )
{
	JMP(Vd_GLI_DRV_vDownLoadTextures);
}

NAKED void GLI_DRV_vUnLoadTextures( void )
{
	JMP(Vd_GLI_DRV_vUnLoadTextures);
}

NAKED long GLI_DRV_lGetSizeOfTexture( GLI_tdstTexture *p_stTexture )
{
	JMP(Vd_GLI_DRV_lGetSizeOfTexture);
}


/****  DOMAT  ****/

NAKED void GLI_DRV_vDoOpaqueTextureSelection( GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT )
{
	JMP(Vd_GLI_DRV_vDoOpaqueTextureSelection);
}

NAKED void GLI_DRV_vWrite16bBitmapToBackBuffer(
	void *p_vSourceBuffer,
	long lWidth,
	long lHeight,
	long lDestLeft,
	long lDestTop,
	long lDestRight,
	long lDestBottom
	)
{
	JMP(Vd_GLI_DRV_vWrite16bBitmapToBackBuffer);
}

NAKED void GLI_DRV_vAddBlackPolygon( long lLeft, long lTop, long lRight, long lBottom )
{
	JMP(Vd_GLI_DRV_vAddBlackPolygon);
}

NAKED void GLI_DRV_vNoBlackPolygon( void )
{
	JMP(Vd_GLI_DRV_vNoBlackPolygon);
}

NAKED void GLI_DRV_vComputeFogEffect( GLI_tdstInternalGlobalValuesFor3dEngine *p_stBG )
{
	JMP(Vd_GLI_DRV_vComputeFogEffect);
}

NAKED BOOL GLI_DRV_bBeginScene( void )
{
	JMP(Vd_GLI_DRV_bBeginScene);
}

NAKED BOOL GLI_DRV_bEndScene( void )
{
	JMP(Vd_GLI_DRV_bEndScene);
}


/****  ACCES  ****/

NAKED void GLI_DRV_vSendSpriteToClip(
	GLI_tdstAligned2DVector *a4_st2DVertex,
	float xZ,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
	)
{
	JMP(Vd_GLI_DRV_vSendSpriteToClip);
}

NAKED void GLI_DRV_vSendSpriteToClipWithUV(
	GLI_tdstAligned2DVector *a4_st2DVertex,
	float *a8_stUVVertex,
	float xZ,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
	)
{
	JMP(Vd_GLI_DRV_vSendSpriteToClipWithUV);
}

NAKED void GLI_DRV_vSendSpriteToClipWithColors(
	GLI_tdstAligned2DVector *a4_st2DVertex,
	void *_Colors,
	float xZ,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
	)
{
	JMP(Vd_GLI_DRV_vSendSpriteToClipWithColors);
}

/*
NAKED void GLI_DRV_vSendSingleLineToClip(
	GLD_tdstViewportAttributes *p_stVpt,
	GLI_tdstAligned3DVector *p_stVertex1,
	GLI_tdstAligned2DVector *p_st2DVertex1,
	GLI_tdstAligned3DVector *p_stVertex2,
	GLI_tdstAligned2DVector *p_st2DVertex2,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT,
	long lDrawModeMask,
	GEO_tdstColor *p_stColor
	)
{
	JMP(Vd_GLI_DRV_vSendSingleLineToClip);
}
*/

/****  CLIP TRIANGLES  ****/

NAKED void GLI_DRV_xClearViewingList( void )
{
	JMP(Vd_GLI_DRV_xClearViewingList);
}

NAKED void GLI_DRV_xSendListToViewport( GLD_tdstViewportAttributes *p_stVpt )
{
	JMP(Vd_GLI_DRV_xSendListToViewport);
}

NAKED void GLI_DRV_vSetZClip( float xZClip, GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT )
{
	JMP(Vd_GLI_DRV_vSetZClip);
}

NAKED void GLI_DRV_vSetClipWindow(
	float fXMin,
	float fXMax,
	float fYMin,
	float fYMax,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
	)
{
	JMP(Vd_GLI_DRV_vSetClipWindow);
}

NAKED void GLI_DRV_xSendElementTIToClip_TRIANGLES(
	GEO_tdstElementIndexedTriangles *p_stLocalElementIndexedTriangle,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
	)
{
	JMP(Vd_GLI_DRV_xSendElementTIToClip_TRIANGLES);
}

NAKED void GLI_DRV_xSendSingleTriangleToClip_TRIANGLES(
	GLI_tdstAligned2DVector *a3_st2DVertex,
	ACP_tdst2DUVValues *a3_stUV,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
	)
{
	JMP(Vd_GLI_DRV_xSendSingleTriangleToClip_TRIANGLES);
}
