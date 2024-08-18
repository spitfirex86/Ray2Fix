#pragma once

#include "framework.h"
#include "fix.h"

#include <ACP_Ray2.h>


/*
 * Function pointers
 */

extern DIEnumDevicesCallback R2_fn_InputEnum;
extern BOOL (*R2_fn_SuspendGame)();

extern char * (*R2_fn_szGetStringFromTextOrStringParam)( void *param );

extern AI_tdstNodeInterpret * (*R2_fn_p_stSPOSuperimpoed)( HIE_tdstSuperObject * p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree );
