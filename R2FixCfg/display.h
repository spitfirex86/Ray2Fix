#pragma once

#include "framework.h"


typedef enum tdeDisplayModeFlags_
{
	e_DMF_None = 0,
	e_DMF_Safe = 1 << 0,
	e_DMF_Best = 1 << 1,
	e_DMF_Custom = 1 << 2,
	e_DMF_Widescreen = 1 << 3
}
tdeDisplayModeFlags;

typedef enum tdeRefRate_
{
	e_RR_Half = 30,
	e_RR_Full = 60
}
tdeRefRate;

typedef struct tdstDisplayMode_
{
	DWORD dwWidth;
	DWORD dwHeight;
	tdeDisplayModeFlags eFlags;
}
tdstDisplayMode;


/*
 * Global Vars
 */

#define C_MaxModes 64

extern tdstDisplayMode g_a_stDispModes[C_MaxModes];


/*
 * Functions
 */

void DSP_fn_vEnumResolutions( void );
