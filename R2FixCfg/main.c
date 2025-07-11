#include "framework.h"
#include "resource.h"
#include "dialogs.h"
#include "config.h"
#include "main.h"
#include "../gitver.h"


HINSTANCE g_hInst;
BOOL g_bUnsavedChanges = FALSE;

char g_szAppName[80];

HWND g_hMainWnd = NULL;

HWND hTC;
HWND hAdvanced;
HWND hCurrentTab;

HWND a_hTabs[e_NbTab];

tdstTabInfo a_stTabs[e_NbTab] = {
	[e_TAB_General] = { IDD_GENERAL, "General", DLG_fn_bProc_General },
	[e_TAB_Pad] = { IDD_PAD, "Gamepad", DLG_fn_bProc_Pad },
};

HWND hToolTip = NULL;


void fn_vCreateTabDialogs( HWND hParent, HWND hTabCtrl )
{
	RECT rcTabArea, rcHeaderArea;
	GetClientRect(hTabCtrl, &rcTabArea);
	TabCtrl_AdjustRect(hTabCtrl, FALSE, &rcTabArea);
	TabCtrl_GetItemRect(hTabCtrl, 0, &rcHeaderArea);

	POINT pt = { 0 };
	ClientToScreen(hTabCtrl, &pt);
	ScreenToClient(hParent, &pt);

	TCITEM tci = { 0 };
	tci.mask = TCIF_TEXT;

	for ( int i = 0; i < e_NbTab; i++ )
	{
		tdstTabInfo *lpTab = &a_stTabs[i];

		a_hTabs[i] = CreateDialog(g_hInst, MAKEINTRESOURCE(lpTab->lDlgId), hParent, lpTab->pfnDlgProc);

		tci.pszText = lpTab->szTabName;
		TabCtrl_InsertItem(hTabCtrl, i, &tci);

		SetWindowPos(
			a_hTabs[i], NULL,
			rcHeaderArea.left + pt.x + 3,
			rcHeaderArea.bottom + pt.y + 3,
			rcTabArea.right - rcTabArea.left,
			rcTabArea.bottom - rcHeaderArea.bottom - 3,
			SWP_NOACTIVATE | SWP_NOZORDER);
	}

	hCurrentTab = a_hTabs[0];
	ShowWindow(hCurrentTab, SW_SHOW);
}

BOOL fn_bAskSaveBeforeClose( HWND hWnd )
{
	char szPrompt[250];
	LoadString(g_hInst, IDS_ASKTOSAVE, szPrompt, sizeof(szPrompt));

	int result = MessageBox(hWnd, szPrompt, g_szAppName, MB_YESNOCANCEL | MB_ICONASTERISK);

	switch ( result )
	{
	case IDYES:
		// Save and quit
		CFG_fn_vWrite();
		g_bUnsavedChanges = FALSE;
		return TRUE;

	case IDNO:
		// Discard and quit
		return TRUE;

	default:
		// Cancelled or closed, do nothing
		return FALSE;
	}
}

void fn_vCreateToolTip( HWND hWnd )
{
	HWND hwndTip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX | TTS_CLOSE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hWnd, NULL, 
		g_hInst, NULL);

	if ( hwndTip )
	{
		SendMessage(hwndTip, TTM_SETMAXTIPWIDTH, 0, 400);
		SendMessage(hwndTip, TTM_SETDELAYTIME, TTDT_INITIAL, 250);
		hToolTip = hwndTip;
	}
}

void fn_vRegisterToolTip( HWND hWnd, HWND hItem, int nIDString )
{
	char szText[256];
	LoadRcString(nIDString, szText);

	TOOLINFO toolInfo = { 0 };
	toolInfo.cbSize = sizeof(toolInfo);
	toolInfo.hwnd = hWnd;
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId = (UINT_PTR)hItem;
	toolInfo.lpszText = szText;
	SendMessage(hToolTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
}

BOOL CALLBACK fn_bProc_Main( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	HICON hIcon;
	int nIndex;
	BOOL bAdvToggle;

	switch ( uMsg )
	{
	case WM_INITDIALOG:
		g_hMainWnd = hWnd;
		// Set caption and taskbar icon
		hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_GLI_ICON));
		SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

		fn_vCreateToolTip(hWnd);

		hTC = GetDlgItem(hWnd, IDC_TAB1);
		fn_vCreateTabDialogs(hWnd, hTC);

		hAdvanced = GetDlgItem(hWnd, IDC_ADVANCED);

		HWND hVerNum = GetDlgItem(hWnd, IDC_VERNUM);
		SetWindowText(hVerNum, "Ray2Fix " C_GIT_VER);

		return TRUE;

	case WM_SHOWWINDOW:
		if ( wParam == TRUE && lParam == 0 )
		{
			HWND hBtn = GetDlgItem(hWnd, IDOK);
			SetFocus(hBtn);
		}
		return TRUE;

	case WM_NOTIFY:
		switch ( ((LPNMHDR)lParam)->code )
		{
		case TCN_SELCHANGE:
			nIndex = TabCtrl_GetCurSel(hTC);
			if ( nIndex > -1 )
			{
				ShowWindow(hCurrentTab, SW_HIDE);
				hCurrentTab = a_hTabs[nIndex];
				ShowWindow(hCurrentTab, SW_SHOW);
			}
			return TRUE;
		}
		break;

	case WM_COMMAND:
		switch ( LOWORD(wParam) )
		{
		case ID_DEBUG:
			// TODO: debug stuff
			return TRUE;

		case IDC_ADVANCED:
			bAdvToggle = Button_GetCheck(hAdvanced);
			for ( int i = 0; i < e_NbTab; i++ )
			{
				SendMessage(a_hTabs[i], WM_ADVTOGGLE, bAdvToggle, 0);
			}
			return TRUE;

		case IDOK:
			SetFocus(NULL);
			CFG_fn_vWrite();
			g_bUnsavedChanges = FALSE;
			// fall-through
		case IDCANCEL:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		if ( g_bUnsavedChanges )
		{
			if ( fn_bAskSaveBeforeClose(hWnd) )
				DestroyWindow(hWnd);
		}
		else
		{
			DestroyWindow(hWnd);
		}
		return TRUE;

	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;
	}

	return FALSE;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow )
{
	HWND hDlg;
	MSG msg;
	g_hInst = hInstance;

	LoadRcString(IDS_APPNAME, g_szAppName);

	HANDLE hMutex = CreateMutex(NULL, TRUE, "Ray2FixCfgSI");

	if ( hMutex == NULL )
		return 1;

	if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		// If another instance already exists, find the window and activate it
		HWND hWndExisting = FindWindow(NULL, g_szAppName);
		if ( hWndExisting )
		{
			SetForegroundWindow(hWndExisting);
		}

		return 0;
	}

	CFG_fn_vVerify();
	CFG_fn_vRead();
	DSP_fn_vEnumResolutions();

	hDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_MAIN), NULL, fn_bProc_Main);

	if ( hDlg == NULL )
		return 1;

	SetWindowText(hDlg, g_szAppName);
	ShowWindow(hDlg, nCmdShow);

	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCEL));

	while ( GetMessage(&msg, NULL, 0, 0) > 0 )
	{
		if ( !TranslateAccelerator(hDlg, hAccel, &msg) && !IsDialogMessage(hDlg, &msg) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
	return 0;
}
