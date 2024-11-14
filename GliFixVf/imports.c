#include "framework.h"
#include "imports.h"
#include "glide2types.h"


/*
 * GLI Library imports
 */

/****  COMMON  ****/
long (*Vd_GLI_DRV_lGetDllInfo)( char const *szName, void *pData );
long (*Vd_GLI_DRV_lSetCommonData)( char const *szName, void *pData );
long (*Vd_GLI_DRV_lSetCommonFct)( char const *szName, tdfn_CommonFct pData );

/****  INIT  ****/
void (*Vd_GLI_DRV_vFlipDevice)( int lWaitFrames );
void (*Vd_GLI_DRV_vFlipDeviceWithSyncro)( void );
void (*Vd_GLI_DRV_vClearDevice)( BOOL bZBuffer, BOOL bColorBuffer, unsigned long ulColor );
void (*Vd_GLI_DRV_xInitDriver)( HWND hWnd, BOOL bFullscreen, long lWidth, long lHeight, int lBpp );
void (*Vd_GLI_DRV_vCloseDriver)( void );
void (*Vd_GLI_DRV_vClearZBufferRegion)( long lXStart, long lXEnd, long lYStart, long lYEnd );
long (*Vd_GLI_DRV_hChangeMode)( BOOL bFullscreen, long lWidth, long lHeight, long lBpp );
BOOL (*Vd_GLI_DRV_bLockDevice)( void **pp_vVirtualScreen, long *p_lPitch );
BOOL (*Vd_GLI_DRV_bUnlockDevice)( void );
BOOL (*Vd_GLI_DRV_bWindowedModeIsOptimized)( void );
void (*Vd_GLI_DRV_vOptimizedWindowedMode)( void );
void (*Vd_GLI_DRV_vNonOptimizedWindowedMode)( void );
BOOL (*Vd_GLI_DRV_bPrepareForGliWindowed)( HWND hWnd );
void (*Vd_GLI_DRV_vPrepareForGliFullScreen)( HWND hWnd );
void (*Vd_GLI_DRV_vActivateGli)( HWND hWnd, BOOL bActivate );
void (*Vd_GLI_DRV_vReadaptDisplay)( void );
long (*Vd_GLI_DRV_fn_lGetAllDisplayConfig)( tdfn_lAddDisplayInfo p_fn_lAddDisplayInfo );
long (*Vd_GLI_DRV_fnl_EnumModes)( char *szDriverName, char *szDeviceName );

/****  HDWTEX  ****/
void (*Vd_GLI_DRV_vDownLoadTextures)( long bRestore, long lTextureMode, BOOL bReloading );
void (*Vd_GLI_DRV_vUnLoadTextures)( void );
long (*Vd_GLI_DRV_lGetSizeOfTexture)( GLI_tdstTexture *p_stTexture );

/****  DOMAT  ****/
void (*Vd_GLI_DRV_vDoOpaqueTextureSelection)( GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT );
void (*Vd_GLI_DRV_vWrite16bBitmapToBackBuffer)(
	void *p_vSourceBuffer,
	long lWidth,
	long lHeight,
	long lDestLeft,
	long lDestTop,
	long lDestRight,
	long lDestBottom
);
void (*Vd_GLI_DRV_vAddBlackPolygon)( long lLeft, long lTop, long lRight, long lBottom );
void (*Vd_GLI_DRV_vNoBlackPolygon)( void );
void (*Vd_GLI_DRV_vComputeFogEffect)( GLI_tdstInternalGlobalValuesFor3dEngine *p_stBG );
BOOL (*Vd_GLI_DRV_bBeginScene)( void );
BOOL (*Vd_GLI_DRV_bEndScene)( void );

/****  ACCES  ****/
void (*Vd_GLI_DRV_vSendSpriteToClip)(
	GLI_tdstAligned2DVector *a4_st2DVertex,
	float xZ,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
void (*Vd_GLI_DRV_vSendSpriteToClipWithUV)(
	GLI_tdstAligned2DVector *a4_st2DVertex,
	float *a8_stUVVertex,
	float xZ,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
void (*Vd_GLI_DRV_vSendSpriteToClipWithColors)(
	GLI_tdstAligned2DVector *a4_st2DVertex,
	void *_Colors,
	float xZ,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
void (*Vd_GLI_DRV_vSendSingleLineToClip)(
	GLD_tdstViewportAttributes *p_stVpt,
	GLI_tdstAligned3DVector *p_stVertex1,
	GLI_tdstAligned2DVector *p_st2DVertex1,
	GLI_tdstAligned3DVector *p_stVertex2,
	GLI_tdstAligned2DVector *p_st2DVertex2,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT,
	long lDrawModeMask,
	GEO_tdstColor *p_stColor
);

/****  CLIP TRIANGLES  ****/
void (*Vd_GLI_DRV_xClearViewingList)( void );
void (*Vd_GLI_DRV_xSendListToViewport)( GLD_tdstViewportAttributes *p_stVpt );
void (*Vd_GLI_DRV_vSetZClip)( float xZClip, GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT );
void (*Vd_GLI_DRV_vSetClipWindow)(
	float fXMin,
	float fXMax,
	float fYMin,
	float fYMax,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
void (*Vd_GLI_DRV_xSendElementTIToClip_TRIANGLES)(
	GEO_tdstElementIndexedTriangles *p_stLocalElementIndexedTriangle,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
void (*Vd_GLI_DRV_xSendSingleTriangleToClip_TRIANGLES)(
	GLI_tdstAligned2DVector *a3_st2DVertex,
	ACP_tdst2DUVValues *a3_stUV,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);


/*
 * Code starts here
 */


char IMP_cWhatBuildWeUsing = 0;

HMODULE hGliVd = NULL;


/** in glide2types.h **/
FxI32 *Vd_GLI_GLIDE1_xTmuNumber = 0;
void (FX_CALL *grAlphaBlendFunction)( FxI32 rgb_sf, FxI32 rgb_df, FxI32 alpha_sf, FxI32 alpha_df ) = NULL;
void (FX_CALL *grAlphaCombine)( FxI32 function, FxI32 factor, FxI32 local, FxI32 other, FxBool invert ) = NULL;
void (FX_CALL *guColorCombineFunction)( FxI32 fnc ) = NULL;
void (FX_CALL *grDrawLine)( const GrVertex *v1, const GrVertex *v2 ) = NULL;

void fn_vLoadGlide2Lib( void )
{
	HMODULE hGlide2 = LoadLibrary("glide2x.dll");
	if ( !hGlide2 )
	{
		MessageBox(NULL, "Cannot load Glide2x library.", "Error", MB_OK | MB_ICONERROR);
		ExitProcess(1);
	}

	grAlphaBlendFunction = (void *)GetProcAddress(hGlide2, "_grAlphaBlendFunction@16");
	grAlphaCombine = (void *)GetProcAddress(hGlide2, "_grAlphaCombine@20");
	guColorCombineFunction = (void *)GetProcAddress(hGlide2, "_guColorCombineFunction@4");
	grDrawLine = (void *)GetProcAddress(hGlide2, "_grDrawLine@8");
}


void *fn_lpGetFn( LPCSTR szName )
{
	return (void*)GetProcAddress(hGliVd, szName);
}

void IMP_fn_vLoadGliLibrary( void )
{
	IMP_cWhatBuildWeUsing = 'f';
	hGliVd = LoadLibrary(".\\DLL\\GliVd1vf.dll");

	if ( !hGliVd )
	{
		IMP_cWhatBuildWeUsing = 'r';
		hGliVd = LoadLibrary(".\\DLL\\GliVd1vr.dll");
	}

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

	if ( IMP_cWhatBuildWeUsing == 'f' )
	{
		unsigned char *hGliBase = (unsigned char *)hGliVd;
		Vd_GLI_GLIDE1_xTmuNumber = (void *)(hGliBase + 0xB704);

		fn_vLoadGlide2Lib();
	}
}
