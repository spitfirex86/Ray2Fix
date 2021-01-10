#pragma once
#include "framework.h"

//
// DIRECTINPUT DECLARATIONS
//

#define DIENUM_STOP			0
#define DIENUM_CONTINUE		1

typedef BOOL (CALLBACK *DIEnumDevicesCallback)( LPVOID lpddi, LPVOID pvRef );

//
// DETOURS
//

void FIX_vAttachHooks( void );
void FIX_vDetachHooks( void );

void FIX_vPatchFramerate( void );
void FIX_vRemoveModeEnum( void );
