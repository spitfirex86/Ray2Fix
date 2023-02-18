#pragma once

#include "framework.h"
#include "fix.h"


typedef struct tdstJFFTXT_
{
	char *lpText;
	float X;
	float Y;
	float size;
	BYTE crAlpha;
	BYTE _byte11;
	BYTE highlight;
	BYTE options;
	DWORD _dword14;
	BYTE crHighlight;
}
tdstJFFTXT;

typedef struct tdstVector3D_
{
	float X;
	float Y;
	float Z;
}
tdstVector3D;


/*
 * Variables
 */

extern void *R2_lpGlobalContext;


/*
 * Function pointers
 */

extern DIEnumDevicesCallback R2_fn_InputEnum;
extern BOOL (*R2_fn_SuspendGame)();

extern char* (*R2_fn_p_szGetLevelName)( void );
extern void (*R2_fn_vEngine)( void );
extern void (*R2_JFFTXT_vAffiche)( void *lpContext );

extern void (*R2_JFFTXT_vDrawString)( void *lpContext, tdstJFFTXT *lpTextStruct );
extern void (*R2_GLI_vDraw2DSpriteWithPercent)( void *lpContext, float x, float y, float u, float v, void *lpTextureData );

extern char* (*R2_fn_szGetStringFromTextOrStringParam)( void *param );

extern HWND (*R2_GetWindowHandle)( void );
