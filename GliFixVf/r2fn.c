#include "framework.h"
#include "r2fn.h"


/*
 * Variables
 */

void *R2_lpGlobalContext = 0x5004D4;


/*
 * Function pointers
 */

DIEnumDevicesCallback R2_fn_InputEnum = 0x495170;
BOOL (*R2_fn_SuspendGame)() = 0x4016E0;

char* (*R2_fn_p_szGetLevelName)( void ) = 0x404DA0;
void (*R2_fn_vEngine)( void ) = 0x40ADA0;
void (*R2_JFFTXT_vAffiche)( void *lpContext ) = 0x465C10;

void (*R2_JFFTXT_vDrawString)( void *lpContext, tdstJFFTXT *lpTextStruct ) = 0x4660B0;
void (*R2_GLI_vDraw2DSpriteWithPercent)(void *lpContext, float x, float y, float u, float v, void *lpTextureData) = 0x429670;

char* (*R2_fn_szGetStringFromTextOrStringParam)( void *param ) = 0x4829D0;

HWND (*R2_GetWindowHandle)(void) = 0x401400;
