#pragma once
#include "framework.h"
// TWEAKS enum defined here
#include "shared.h"

#define MAX_MODS 40

typedef struct tagMOD_DATA
{
	HMODULE hDll;
	void *reserved;
} MOD_DATA;

extern MOD_DATA TWK_aMods[MAX_MODS];
extern int TWK_lModCount;

void TWK_fn_vLoadAllMods( void );
void TWK_fn_vInitTweaks( void );
