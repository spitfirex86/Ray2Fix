#pragma once

#include "framework.h"
#include "fix.h"


/*
 * Function pointers
 */

extern DIEnumDevicesCallback R2_fn_InputEnum;
extern BOOL (*R2_fn_SuspendGame)();

extern char * (*R2_fn_p_szGetLevelName)( void );
extern void (*R2_fn_vEngine)( void );

extern char* (*R2_fn_szGetStringFromTextOrStringParam)( void *param );

extern HWND (*R2_GetWindowHandle)( void );
