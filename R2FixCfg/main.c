#include "framework.h"
#include "resource.h"
#include "dialogs.h"
#include "config.h"
#include "main.h"

HINSTANCE g_hInst;
BOOL g_bUnsavedChanges = FALSE;

char szAppName[80];

HWND hTC;
HWND hAdvanced;
HWND hCurrentTab;

HWND a_hTabs[NUM_TABS];

TAB_INFO a_tiTabs[NUM_TABS] = {
	[TAB_GENERAL] = { IDD_GENERAL, "General", GeneralDialogProc },
	[TAB_PAD] = { IDD_PAD, "Gamepad", PadDialogProc },
	[TAB_TWEAKS] = { IDD_TWEAKS, "Tweaks", TweaksDialogProc }
};

void CreateTabDialogs( HWND hParent, HWND hTabCtrl )
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

	for ( int i = 0; i < NUM_TABS; i++ )
	{
		TAB_INFO *lpTab = &a_tiTabs[i];

		a_hTabs[i] = CreateDialog(g_hInst, MAKEINTRESOURCE(lpTab->lDlgId), hParent, lpTab->lpDlgProc);

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

BOOL AskSaveBeforeClose( HWND hWnd )
{
	char szPrompt[250];
	LoadString(g_hInst, IDS_ASKTOSAVE, szPrompt, sizeof(szPrompt));

	int result = MessageBox(hWnd, szPrompt, szAppName, MB_YESNOCANCEL | MB_ICONASTERISK);

	switch ( result )
	{
	case IDYES:
		// Save and quit
		WriteConfig();
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

BOOL CALLBACK MainDialogProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
	)
{
	HICON hIcon;
	int nIndex;
	BOOL bAdvToggle;

	switch ( uMsg )
	{
	case WM_INITDIALOG:
		// Set caption and taskbar icon
		hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_GLI_ICON));
		SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

		hTC = GetDlgItem(hWnd, IDC_TAB1);
		CreateTabDialogs(hWnd, hTC);

		hAdvanced = GetDlgItem(hWnd, IDC_ADVANCED);
		break;

	case WM_SHOWWINDOW:
		if ( wParam == TRUE && lParam == 0 )
		{
			HWND hBtn = GetDlgItem(hWnd, IDOK);
			SetFocus(hBtn);
		}
		break;

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
			break;

		default: return FALSE;
		}
		break;

	case WM_COMMAND:
		switch ( LOWORD(wParam) )
		{
		case ID_DEBUG:
			// TODO: debug stuff
			break;

		case IDC_ADVANCED:
			bAdvToggle = Button_GetCheck(hAdvanced);
			for ( int i = 0; i < NUM_TABS; i++ )
			{
				SendMessage(a_hTabs[i], WM_ADVTOGGLE, bAdvToggle, 0);
			}
			break;

		case IDOK:
			SetFocus(NULL);
			WriteConfig();
			g_bUnsavedChanges = FALSE;
			// fall-through
		case IDCANCEL:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		default: return FALSE;
		}
		break;

	case WM_CLOSE:
		if ( g_bUnsavedChanges )
		{
			if ( AskSaveBeforeClose(hWnd) )
				DestroyWindow(hWnd);
		}
		else
		{
			DestroyWindow(hWnd);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default: return FALSE;
	}

	return TRUE;
}

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	INT nCmdShow
	)
{
	HWND hDlg;
	MSG msg;
	g_hInst = hInstance;

	LoadRcString(IDS_APPNAME, szAppName);

	HANDLE hMutex = CreateMutex(NULL, TRUE, "Ray2FixCfgSI");

	if ( hMutex == NULL )
		return 1;

	if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		// If another instance already exists, find the window and activate it
		HWND hWndExisting = FindWindow(NULL, szAppName);
		if ( hWndExisting )
		{
			SetForegroundWindow(hWndExisting);
		}

		return 0;
	}

	VerifyFiles();
	ReadConfig();
	EnumResolutions();

	hDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_MAIN), NULL, MainDialogProc);

	if ( hDlg == NULL )
		return 1;

	SetWindowText(hDlg, szAppName);
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
