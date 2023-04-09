#pragma once

#include "framework.h"
#include "devinfo.h"
#include "defs.h"


#define NAKED __declspec(naked)
#define JMP(p_fn) do { __asm { jmp p_fn } } while(0)


/*
 * Exported functions
 */

EXPORT long GLI_DRV_lGetDllInfo( char const *szName, void *pData );
EXPORT long GLI_DRV_lSetCommonData( char const *szName, void *pData );
EXPORT long GLI_DRV_lSetCommonFct( char const *szName, tdfn_CommonFct pData );
EXPORT long GLI_DRV_fn_lGetAllDisplayConfig( tdfn_lAddDisplayInfo p_fn_lAddDisplayInfo );
EXPORT long GLI_DRV_fnl_EnumModes( char *szDriverName, char *szDeviceName );
EXPORT void GLI_DRV_xInitDriver( HWND hWnd, BOOL bFullscreen, long lWidth, long lHeight, int lBpp );
EXPORT void GLI_DRV_vFlipDeviceWithSyncro( void );


/*
 * Redirected exports
 */

/****  INIT  ****/
EXPORT void GLI_DRV_vFlipDevice( int lWaitFrames );
EXPORT void GLI_DRV_vClearDevice( BOOL bZBuffer, BOOL bColorBuffer, unsigned long ulColor );
EXPORT void GLI_DRV_vCloseDriver( void );
EXPORT void GLI_DRV_vClearZBufferRegion( long lXStart, long lXEnd, long lYStart, long lYEnd );
EXPORT long GLI_DRV_hChangeMode( BOOL bFullscreen, long lWidth, long lHeight, long lBpp );
EXPORT BOOL GLI_DRV_bLockDevice( void **pp_vVirtualScreen, long *p_lPitch );
EXPORT BOOL GLI_DRV_bUnlockDevice( void );
EXPORT BOOL GLI_DRV_bWindowedModeIsOptimized( void );
EXPORT void GLI_DRV_vOptimizedWindowedMode( void );
EXPORT void GLI_DRV_vNonOptimizedWindowedMode( void );
EXPORT BOOL GLI_DRV_bPrepareForGliWindowed( HWND hWnd );
EXPORT void GLI_DRV_vPrepareForGliFullScreen( HWND hWnd );
EXPORT void GLI_DRV_vActivateGli( HWND hWnd, BOOL bActivate );
EXPORT void GLI_DRV_vReadaptDisplay( void );
/*
GLI_DRV_vGetStats()
GLI_DRV_vWaitRetrace()
GLI_DRV_lRequest()
*/

/****  HDWTEX  ****/
EXPORT void GLI_DRV_vDownLoadTextures( long bRestore, long lTextureMode, BOOL bReloading );
EXPORT void GLI_DRV_vUnLoadTextures( void );
EXPORT long GLI_DRV_lGetSizeOfTexture( GLI_tdstTexture *p_stTexture );

/****  DOMAT  ****/
EXPORT void GLI_DRV_vDoOpaqueTextureSelection( GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT );
EXPORT void GLI_DRV_vWrite16bBitmapToBackBuffer(
	void *p_vSourceBuffer,
	long lWidth,
	long lHeight,
	long lDestLeft,
	long lDestTop,
	long lDestRight,
	long lDestBottom
);
EXPORT void GLI_DRV_vAddBlackPolygon( long lLeft, long lTop, long lRight, long lBottom );
EXPORT void GLI_DRV_vNoBlackPolygon( void );
EXPORT void GLI_DRV_vComputeFogEffect( GLI_tdstInternalGlobalValuesFor3dEngine *p_stBG );
EXPORT BOOL GLI_DRV_bBeginScene( void );
EXPORT BOOL GLI_DRV_bEndScene( void );

/****  ACCES  ****/
EXPORT void GLI_DRV_vSendSpriteToClip(
	GLI_tdstAligned2DVector *a4_st2DVertex,
	float xZ,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
EXPORT void GLI_DRV_vSendSpriteToClipWithUV(
	GLI_tdstAligned2DVector *a4_st2DVertex,
	float *a8_stUVVertex,
	float xZ,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
EXPORT void GLI_DRV_vSendSpriteToClipWithColors(
	GLI_tdstAligned2DVector *a4_st2DVertex,
	void *_Colors,
	float xZ,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
EXPORT void GLI_DRV_vSendSingleLineToClip(
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
EXPORT void GLI_DRV_xClearViewingList( void );
EXPORT void GLI_DRV_xSendListToViewport( GLD_tdstViewportAttributes *p_stVpt );
EXPORT void GLI_DRV_vSetZClip( float xZClip, GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT );
EXPORT void GLI_DRV_vSetClipWindow(
	float fXMin,
	float fXMax,
	float fYMin,
	float fYMax,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
EXPORT void GLI_DRV_xSendElementTIToClip_TRIANGLES(
	GEO_tdstElementIndexedTriangles *p_stLocalElementIndexedTriangle,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
EXPORT void GLI_DRV_xSendSingleTriangleToClip_TRIANGLES(
	GLI_tdstAligned2DVector *a3_st2DVertex,
	ACP_tdst2DUVValues *a3_stUV,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
