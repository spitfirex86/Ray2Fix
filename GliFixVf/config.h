#pragma once
#include "framework.h"
#include "shared.h"

typedef struct tagDISP_MODE
{
	DWORD dwWidth;
	DWORD dwHeight;
} DISP_MODE;

//
// GLOBAL VARS
//

extern DISP_MODE CFG_dmDispMode;

extern BOOL CFG_bIsMainModuleR2;
extern BOOL CFG_bIsFixEnabled;

extern BOOL CFG_bTainted;
extern TWEAKS CFG_lTweaks;

//
// FUNCTIONS
//

void CFG_vInitGlobals( void );
BOOL CFG_bOpenConfigTool( void );
BOOL CFG_bDetermineMainModule( void );
