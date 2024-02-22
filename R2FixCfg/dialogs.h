#pragma once

#include "framework.h"


typedef enum tdeTabID_
{
	e_TAB_General,
	e_TAB_Pad,
	e_NbTab
}
tdeTabID;

typedef struct tdstTabInfo_
{
	int lDlgId;
	char szTabName[50];
	DLGPROC pfnDlgProc;
}
tdstTabInfo;


BOOL CALLBACK DLG_fn_bProc_General( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK DLG_fn_bProc_Pad( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
