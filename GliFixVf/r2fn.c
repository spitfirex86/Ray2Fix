#include "framework.h"
#include "r2fn.h"


/*
 * Function pointers
 */

DIEnumDevicesCallback R2_fn_InputEnum = 0x495170;
BOOL (*R2_fn_SuspendGame)() = 0x4016E0;

char * (*R2_fn_szGetStringFromTextOrStringParam)( void *param ) = 0x4829D0;
