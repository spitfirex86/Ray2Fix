#pragma once
#include "framework.h"


typedef enum tdeAnalogAction
{
	#define M_DefineAction(Id, szName, szCfg_X, szCfg_Y) Id,
	#include "def_analog.h"

	E_NbAnalogAction
}
tdeAnalogAction;

typedef enum tdeButtonAction
{
	#define M_DefineAction(Id, szName, szCfg) Id,
	#include "def_button.h"

	E_NbButtonAction
}
tdeButtonAction;

typedef enum tdePadInput
{
	#define M_DefineInput(Id, lCtrl, bAnalog, szCfg) Id,
	#include "def_pad.h"

	E_NbPadInput
}
tdePadInput;


typedef struct tdstPadConfig
{
	int lCtrlId;
	HWND hCtrl;

	BOOL bIsAnalog;
	int lAction;

	char szName[40];
}
tdstPadConfig;

typedef struct tdstAction
{
	char szName[40];
	char szConfigString[80];
}
tdstAction;

typedef struct tdstAnalogAction
{
	char szName[40];
	char szConfigString_X[80];
	char szConfigString_Y[80];
}
tdstAnalogAction;
