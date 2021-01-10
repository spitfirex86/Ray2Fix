#pragma once
#include "framework.h"

typedef enum tagDM_FLAGS
{
	DMF_NONE = 0,
	DMF_SAFE = 1 << 0,
	DMF_BEST = 1 << 1
} DM_FLAGS;

typedef enum tagREFRATE
{
	RR_HALF = 30,
	RR_FULL = 60
} REFRATE;

typedef struct tagDISP_MODE
{
	DWORD dwWidth;
	DWORD dwHeight;
	DM_FLAGS dmfFlags;
} DISP_MODE;

//
// GLOBAL VARS
//

#define MAX_MODES 64

extern DISP_MODE g_aDispModes[MAX_MODES];

//
// FUNCTIONS
//

void EnumResolutions( void );
