#pragma once

#include "framework.h"
#include "devinfo.h"


void IMP_fn_vLoadGliLibrary( void );


/*
 * GLI Library imports
 */

extern BOOL (*Vd_GLI_DRV_lGetDllInfo)( const char *, void * );
extern BOOL (*Vd_GLI_DRV_fn_lGetAllDisplayConfig)( tdfnGliSet );

extern BOOL (*Vd_GLI_DRV_lSetCommonData)( const char *, void * );
extern BOOL (*Vd_GLI_DRV_lSetCommonFct)( const char *, tdfnCommonFct );

extern BOOL (*Vd_GLI_DRV_fnl_EnumModes)( char *, char * );

extern DWORD (*Vd_GLI_DRV_xInitDriver)( HWND, BOOL, int, int, int );
extern void (*Vd_GLI_DRV_vCloseDriver)();
extern BOOL (*Vd_GLI_DRV_bBeginScene)();
extern BOOL (*Vd_GLI_DRV_bEndScene)();
extern BOOL (*Vd_GLI_DRV_bLockDevice)( DWORD *, DWORD * );
extern BOOL (*Vd_GLI_DRV_bUnlockDevice)();
extern void (*Vd_GLI_DRV_vClearDevice)( int, int, int );
extern void (*Vd_GLI_DRV_vFlipDevice)();
extern void (*Vd_GLI_DRV_vFlipDeviceWithSyncro)();
extern void (*Vd_GLI_DRV_vDownLoadTextures)( int, int, int );
extern void (*Vd_GLI_DRV_vUnLoadTextures)();
extern int (*Vd_GLI_DRV_lGetSizeOfTexture)( void * );
extern void (*Vd_GLI_DRV_vDoOpaqueTextureSelection)( int );
extern int (*Vd_GLI_DRV_hChangeMode)( BOOL, int, int, int );
extern BOOL (*Vd_GLI_DRV_bWindowedModeIsOptimized)();
extern void (*Vd_GLI_DRV_vOptimizedWindowedMode)();
extern void (*Vd_GLI_DRV_vNonOptimizedWindowedMode)();
extern BOOL (*Vd_GLI_DRV_bPrepareForGliWindowed)( HWND );
extern void (*Vd_GLI_DRV_vPrepareForGliFullScreen)( HWND );
extern void (*Vd_GLI_DRV_vActivateGli)( HWND, BOOL );
extern void (*Vd_GLI_DRV_vReadaptDisplay)();
extern void (*Vd_GLI_DRV_vAddBlackPolygon)( int, int, int, int );
extern void (*Vd_GLI_DRV_vNoBlackPolygon)();
extern void (*Vd_GLI_DRV_vSetZClip)( float, int );
extern void (*Vd_GLI_DRV_vSetClipWindow)( float, int, int, int, int );
extern void (*Vd_GLI_DRV_vSendSingleLineToClip)( int, int, int, int, int );
extern void (*Vd_GLI_DRV_vSendSpriteToClip)( int, int, int );
extern void (*Vd_GLI_DRV_vSendSpriteToClipWithColors)( int, int, int, int );
extern void (*Vd_GLI_DRV_vSendSpriteToClipWithUV)( int, int, int, int );
extern int (*Vd_GLI_DRV_xSendElementTIToClip_TRIANGLES)( int, int );
extern int (*Vd_GLI_DRV_xSendSingleTriangleToClip_TRIANGLES)( int, int, int );
extern int (*Vd_GLI_DRV_xClearViewingList)();
extern int (*Vd_GLI_DRV_xSendListToViewport)();
extern void (*Vd_GLI_DRV_vClearZBufferRegion)();
extern void (*Vd_GLI_DRV_vComputeFogEffect)( int );
extern void (*Vd_GLI_DRV_vWrite16bBitmapToBackBuffer)( int, int, int, int, int, int, int );
