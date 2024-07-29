#pragma once

#include "framework.h"
#include "fix.h"


/*
 * Function pointers
 */

extern DIEnumDevicesCallback R2_fn_InputEnum;
extern BOOL (*R2_fn_SuspendGame)();

extern char * (*R2_fn_szGetStringFromTextOrStringParam)( void *param );
