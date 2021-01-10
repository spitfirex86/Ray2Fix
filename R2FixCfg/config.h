#pragma once
#include "framework.h"
#include "../GliFixVf/shared.h"
#include "display.h"

typedef enum tagVERIFY_ERR
{
	VE_OK = 0,
	VE_GAME_ERROR = 1 << 0,
	VE_FIX_ERROR = 1 << 1,
	VE_UBI_MISSING = 1 << 2,
	VE_DEGE_MISSING = 1 << 3,
	VE_GLIDE_MISSING = 1 << 4,
	VE_FIX_MISSING = 1 << 5,
	VE_DINPUT_MISSING = 1 << 6
} VERIFY_ERR;

//
// GLOBAL VARS
//

extern BOOL g_bFixState;
extern BOOL g_bFixPrevState;

extern TWEAKS g_lTweaks;

extern DISP_MODE g_dmCurrentMode;
extern REFRATE g_lRefRate;
extern BOOL g_bForceVsync;
extern BOOL g_bFullscreen;

extern VERIFY_ERR g_veMissingFiles;

//
// FUNCTIONS
//

void ReadConfig( void );
void WriteConfig( void );
void VerifyFiles( void );
