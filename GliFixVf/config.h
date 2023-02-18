#pragma once

#include "framework.h"
#include "shared.h"


typedef struct tdstDisplayMode_
{
	DWORD dwWidth;
	DWORD dwHeight;
}
tdstDisplayMode;


/*
 * Global Vars
 */

extern tdstDisplayMode CFG_stDispMode;
extern BOOL CFG_bHalfRefRate;

extern BOOL CFG_bIsMainModuleR2;
extern BOOL CFG_bIsFixEnabled;

extern BOOL CFG_bTainted;
extern tdeTweaks CFG_eTweaks;


/*
 * Functions
 */

void CFG_fn_vInitGlobals( void );
BOOL CFG_fn_bOpenConfigTool( void );
BOOL CFG_fn_bDetermineMainModule( void );
