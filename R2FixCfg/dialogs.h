#pragma once
#include "framework.h"

typedef enum tagTAB_ID
{
	TAB_GENERAL,
	TAB_TWEAKS,
	NUM_TABS
} TAB_ID;

typedef struct tagTAB_INFO
{
	int lDlgId;
	char szTabName[50];
	DLGPROC lpDlgProc;
} TAB_INFO;

BOOL CALLBACK GeneralDialogProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK TweaksDialogProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
