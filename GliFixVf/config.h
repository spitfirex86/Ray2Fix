#pragma once

#include "framework.h"


typedef struct tdstDisplayMode_
{
	DWORD dwWidth;
	DWORD dwHeight;
}
tdstDisplayMode;


/*
 * Global Vars
 */

extern tdstDisplayMode CFG_stActualDispMode;
extern float CFG_xActualRatio;

extern tdstDisplayMode CFG_stDispMode;
extern BOOL CFG_bHalfRefRate;
extern int CFG_DEBUG_lWaitFrame;
extern int CFG_DEBUG_lScreenshotLag;

extern BOOL CFG_bIsMainModuleR2;
extern BOOL CFG_bIsFixEnabled;

extern BOOL CFG_bPatchWidescreen;
extern BOOL CFG_bIsWidescreen;

extern char CFG_szModuleName[MAX_PATH];
extern char CFG_szModuleDate[20];


/*
 * Functions
 */

void CFG_fn_vInitGlobals( void );
BOOL CFG_fn_bOpenConfigTool( void );
BOOL CFG_fn_bDetermineMainModule( void );
