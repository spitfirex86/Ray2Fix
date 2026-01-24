#include "framework.h"
#include "r2fn.h"


/*
 * Function pointers
 */

DIEnumDevicesCallback R2_fn_InputEnum = OFFSET(0x495170);
BOOL (*R2_fn_bAppliOnDeActivate)( BOOL bIsFullScreen ) = OFFSET(0x4016E0);
BOOL (*R2_fn_bAppliOnActivate)( void ) = OFFSET(0x401790);

char * (*R2_fn_szGetStringFromTextOrStringParam)( void *param ) = OFFSET(0x4829D0);

AI_tdstNodeInterpret * (*R2_fn_p_stSPOSuperimpoed)( HIE_tdstSuperObject * p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree ) = OFFSET(0x4710C0);
