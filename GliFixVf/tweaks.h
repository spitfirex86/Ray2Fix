#pragma once

#include "framework.h"
#include "shared.h" /* tdeTweaks defined here */


#define C_MaxMods 40

typedef struct tdstModData_
{
	HMODULE hDll;
	void *reserved;
}
tdstModData;


extern tdstModData TWK_a_stMods[C_MaxMods];
extern int TWK_lModCount;

void TWK_fn_vLoadAllMods( void );
void TWK_fn_vInitTweaks( void );
