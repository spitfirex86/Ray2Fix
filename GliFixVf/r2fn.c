#include "framework.h"
#include "r2fn.h"


/*
 * Function pointers
 */

DIEnumDevicesCallback R2_fn_InputEnum = 0x495170;
BOOL (*R2_fn_SuspendGame)() = 0x4016E0;

char * (*R2_fn_p_szGetLevelName)( void ) = 0x404DA0;
void (*R2_fn_vEngine)( void ) = 0x40ADA0;

char* (*R2_fn_szGetStringFromTextOrStringParam)( void *param ) = 0x4829D0;

HWND (*R2_GetWindowHandle)(void) = 0x401400;

void (*R2_GLI_xAdjustCameraToViewport2)(GLD_tdstDeviceAttributes* p_stDev, GLD_tdstViewportAttributes* p_stVpt, GLI_tdstCamera* p_stCam) = 0x422AB0;