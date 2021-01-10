#pragma once
#include "framework.h"
#include "shared.h"

//
// GLOBAL VARS
//

extern int CFG_lDispWidth;
extern int CFG_lDispHeight;

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
