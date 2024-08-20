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


/*
 * Special widescreen exports
 */

EXPORT BOOL GLI_FIX_bIsWidescreen( void );
EXPORT float GLI_FIX_xGetActualRatio( void );
EXPORT void GLI_FIX_vGetActualDisplayMode( unsigned long *p_ulWidthOut, unsigned long *p_ulHeightOut );
