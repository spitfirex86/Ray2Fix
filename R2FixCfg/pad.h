#pragma once
#include "framework.h"


#define C_Pad_InvalidAction (-1)
#define C_Pad_MaxName 40
#define C_Pad_MaxCfgString 80


typedef enum tdeAnalogAction_
{
	#define M_DefineAction(Id, szName, szCfg_X, szCfg_Y) Id,
	#include "def_analog.h"

	E_NbAnalogAction
}
tdeAnalogAction;

typedef enum tdeButtonAction_
{
	#define M_DefineAction(Id, szName, szCfg) Id,
	#include "def_button.h"

	E_NbButtonAction
}
tdeButtonAction;

typedef enum tdePadInput_
{
	#define M_DefineInput(Id, lCtrl, bAnalog, szCfg) Id,
	#include "def_pad.h"

	E_NbPadInput
}
tdePadInput;


typedef struct tdstAnalogAction_
{
	int lId;
	char szName[C_Pad_MaxName];
	char szConfigString_X[C_Pad_MaxCfgString];
	char szConfigString_Y[C_Pad_MaxCfgString];
}
tdstAnalogAction;

typedef struct tdstAction_
{
	int lId;
	char szName[C_Pad_MaxName];
	char szConfigString[C_Pad_MaxCfgString];
}
tdstAction;

typedef struct tdstPadConfig_
{
	int lId;
	int lAction;
	BOOL bIsAnalog;
	char szName[C_Pad_MaxName];
}
tdstPadConfig;

typedef struct tdstPadDlgItem_
{
	int lId;
	int lCtrlId;
	HWND hCtrl;
	BOOL bIsAnalog;
}
tdstPadDlgItem;


extern char const *g_szXidiPath;

extern tdstAnalogAction g_a_stAnalogAction[E_NbAnalogAction];
extern tdstAction g_a_stButtonAction[E_NbButtonAction];
extern tdstPadConfig g_a_stPadInput[E_NbPadInput];


void PAD_fn_vRead( void );
void PAD_fn_vWrite( void );
