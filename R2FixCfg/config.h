#pragma once

#include "framework.h"
#include "display.h"


typedef enum tdeErrorState_
{
	e_ES_Ok			= 0,
	e_ES_Warning	= 1 << 0,
	e_ES_GameError	= 1 << 1,
	e_ES_FixError	= 1 << 2,
}
tdeErrorState;

typedef enum tdeVerifyErr_
{
	e_VE_Ok				= 0,
	e_VE_FilesMissing	= 1 << 0,
	e_VE_UbiMissing		= 1 << 1,
	e_VE_DegeMissing	= 1 << 2,
	e_VE_GlideMissing	= 1 << 3,
	e_VE_FixMissing		= 1 << 4,
	e_VE_DinputMissing	= 1 << 5,
	e_VE_XidiMissing	= 1 << 6,
	e_VE_XidiModified	= 1 << 7,
}
tdeVerifyErr;


/*
 * Global Vars
 */

extern BOOL g_bFixState;
extern BOOL g_bFixPrevState;

extern tdstDisplayMode g_stCurrentMode;
extern tdeRefRate g_eRefRate;
extern BOOL g_bForceVsync;
extern BOOL g_bFullscreen;

extern tdeErrorState g_eError;
extern tdeVerifyErr g_eErrorDetails;


/*
 * Functions
 */

void CFG_fn_vRead( void );
void CFG_fn_vWrite( void );
void CFG_fn_vVerify( void );
