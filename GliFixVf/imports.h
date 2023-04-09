#pragma once

#include "framework.h"
#include "devinfo.h"
#include "defs.h"


void IMP_fn_vLoadGliLibrary( void );


/*
 * GLI Library imports
 */

/****  COMMON  ****/
extern long (*Vd_GLI_DRV_lGetDllInfo)( char const *szName, void *pData );
extern long (*Vd_GLI_DRV_lSetCommonData)( char const *szName, void *pData );
extern long (*Vd_GLI_DRV_lSetCommonFct)( char const *szName, tdfn_CommonFct pData );

/****  INIT  ****/
extern void (*Vd_GLI_DRV_vFlipDevice)( int lWaitFrames );
extern void (*Vd_GLI_DRV_vFlipDeviceWithSyncro)( void );
extern void (*Vd_GLI_DRV_vClearDevice)( BOOL bZBuffer, BOOL bColorBuffer, unsigned long ulColor );
extern void (*Vd_GLI_DRV_xInitDriver)( HWND hWnd, BOOL bFullscreen, long lWidth, long lHeight, int lBpp );
extern void (*Vd_GLI_DRV_vCloseDriver)( void );
extern void (*Vd_GLI_DRV_vClearZBufferRegion)( long lXStart, long lXEnd, long lYStart, long lYEnd );
extern long (*Vd_GLI_DRV_hChangeMode)( BOOL bFullscreen, long lWidth, long lHeight, long lBpp );
extern BOOL (*Vd_GLI_DRV_bLockDevice)( void **pp_vVirtualScreen, long *p_lPitch );
extern BOOL (*Vd_GLI_DRV_bUnlockDevice)( void );
extern BOOL (*Vd_GLI_DRV_bWindowedModeIsOptimized)( void );
extern void (*Vd_GLI_DRV_vOptimizedWindowedMode)( void );
extern void (*Vd_GLI_DRV_vNonOptimizedWindowedMode)( void );
extern BOOL (*Vd_GLI_DRV_bPrepareForGliWindowed)( HWND hWnd );
extern void (*Vd_GLI_DRV_vPrepareForGliFullScreen)( HWND hWnd );
extern void (*Vd_GLI_DRV_vActivateGli)( HWND hWnd, BOOL bActivate );
extern void (*Vd_GLI_DRV_vReadaptDisplay)( void );
extern long (*Vd_GLI_DRV_fn_lGetAllDisplayConfig)( tdfn_lAddDisplayInfo p_fn_lAddDisplayInfo );
extern long (*Vd_GLI_DRV_fnl_EnumModes)( char *szDriverName, char *szDeviceName );

/****  HDWTEX  ****/
extern void (*Vd_GLI_DRV_vDownLoadTextures)( long bRestore, long lTextureMode, BOOL bReloading );
extern void (*Vd_GLI_DRV_vUnLoadTextures)( void );
extern long (*Vd_GLI_DRV_lGetSizeOfTexture)( GLI_tdstTexture *p_stTexture );

/****  DOMAT  ****/
extern void (*Vd_GLI_DRV_vDoOpaqueTextureSelection)( GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT );
extern void (*Vd_GLI_DRV_vWrite16bBitmapToBackBuffer)(
	void *p_vSourceBuffer,
	long lWidth,
	long lHeight,
	long lDestLeft,
	long lDestTop,
	long lDestRight,
	long lDestBottom
);
extern void (*Vd_GLI_DRV_vAddBlackPolygon)( long lLeft, long lTop, long lRight, long lBottom );
extern void (*Vd_GLI_DRV_vNoBlackPolygon)( void );
extern void (*Vd_GLI_DRV_vComputeFogEffect)( GLI_tdstInternalGlobalValuesFor3dEngine *p_stBG );
extern BOOL (*Vd_GLI_DRV_bBeginScene)( void );
extern BOOL (*Vd_GLI_DRV_bEndScene)( void );

/****  ACCES  ****/
extern void (*Vd_GLI_DRV_vSendSpriteToClip)(
	GLI_tdstAligned2DVector *a4_st2DVertex,
	float xZ,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
extern void (*Vd_GLI_DRV_vSendSpriteToClipWithUV)(
	GLI_tdstAligned2DVector *a4_st2DVertex,
	float *a8_stUVVertex,
	float xZ,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
extern void (*Vd_GLI_DRV_vSendSpriteToClipWithColors)(
	GLI_tdstAligned2DVector *a4_st2DVertex,
	void *_Colors,
	float xZ,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
extern void (*Vd_GLI_DRV_vSendSingleLineToClip)(
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
extern void (*Vd_GLI_DRV_xClearViewingList)( void );
extern void (*Vd_GLI_DRV_xSendListToViewport)( GLD_tdstViewportAttributes *p_stVpt );
extern void (*Vd_GLI_DRV_vSetZClip)( float xZClip, GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT );
extern void (*Vd_GLI_DRV_vSetClipWindow)(
	float fXMin,
	float fXMax,
	float fYMin,
	float fYMax,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
extern void (*Vd_GLI_DRV_xSendElementTIToClip_TRIANGLES)(
	GEO_tdstElementIndexedTriangles *p_stLocalElementIndexedTriangle,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
extern void (*Vd_GLI_DRV_xSendSingleTriangleToClip_TRIANGLES)(
	GLI_tdstAligned2DVector *a3_st2DVertex,
	ACP_tdst2DUVValues *a3_stUV,
	GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT
);
