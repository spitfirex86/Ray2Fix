#pragma once

#include "framework.h"


/*
 * DirectInput
 */

#define DIENUM_STOP			0
#define DIENUM_CONTINUE		1

typedef BOOL (CALLBACK *DIEnumDevicesCallback)( LPVOID lpddi, LPVOID pvRef );


/*
 * Detours
 */

void FIX_fn_vAttachHooks( void );
void FIX_fn_vDetachHooks( void );

void FIX_fn_vPatchFramerate( void );
void FIX_fn_vRemoveModeEnum( void );
