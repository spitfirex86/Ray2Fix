#pragma once

#include "framework.h"
#include "../GliFixVf/shared.h"
#include "display.h"


typedef enum tdeVerifyErr_
{
	e_VE_Ok = 0,
	e_VE_GameError = 1 << 0,
	e_VE_FixError = 1 << 1,
	e_VE_UbiMissing = 1 << 2,
	e_VE_DegeMissing = 1 << 3,
	e_VE_GlideMissing = 1 << 4,
	e_VE_FixMissing = 1 << 5,
	e_VE_DinputMissing = 1 << 6
}
tdeVerifyErr;


/*
 * Global Vars
 */

extern BOOL g_bFixState;
extern BOOL g_bFixPrevState;

extern tdeTweaks g_eTweaks;

extern tdstDisplayMode g_stCurrentMode;
extern tdeRefRate g_eRefRate;
extern BOOL g_bForceVsync;
extern BOOL g_bFullscreen;

extern tdeVerifyErr g_eMissingFiles;


/*
 * Functions
 */

void CFG_fn_vRead( void );
void CFG_fn_vWrite( void );
void CFG_fn_vVerify( void );
