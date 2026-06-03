#include "framework.h"
#include "resource.h"
#include "dialogs.h"
#include "main.h"
#include "pad.h"


static HWND hThis;

static HWND hBackendSDL;
static HWND hBackendXinput;

static HWND hAdvGroup;
static HWND hXidiLog;


static void fn_vToggleAdvanced( BOOL bVisible )
{
	int nCmdShow = bVisible ? SW_SHOW : SW_HIDE;
	ShowWindow(hAdvGroup, nCmdShow);
	ShowWindow(hXidiLog, nCmdShow);
}

BOOL CALLBACK DLG_fn_bProc_Input( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
	case WM_INITDIALOG:
		hThis = hWnd;
		hBackendSDL = GetDlgItem(hWnd, IDC_R_SDL);
		hBackendXinput = GetDlgItem(hWnd, IDC_R_XINPUT);
		hAdvGroup = GetDlgItem(hWnd, IDC_ADVGROUP);
		hXidiLog = GetDlgItem(hWnd, IDC_XIDILOG);

		Button_SetCheck(hBackendSDL, g_bUsingSDLBackend);
		Button_SetCheck(hBackendXinput, !g_bUsingSDLBackend);
		Button_SetCheck(hXidiLog, g_bXidiLogging);

		return TRUE;

	case WM_ADVTOGGLE:
		fn_vToggleAdvanced((BOOL)wParam);
		return TRUE;

	case WM_COMMAND:
		switch ( LOWORD(wParam) )
		{
		case IDC_R_SDL:
			g_bUsingSDLBackend = TRUE;
			Button_SetCheck(hBackendSDL, BST_CHECKED);
			Button_SetCheck(hBackendXinput, BST_UNCHECKED);
			g_bUnsavedChanges = TRUE;
			return TRUE;

		case IDC_R_XINPUT:
			g_bUsingSDLBackend = FALSE;
			Button_SetCheck(hBackendSDL, BST_UNCHECKED);
			Button_SetCheck(hBackendXinput, BST_CHECKED);
			g_bUnsavedChanges = TRUE;
			return TRUE;

		case IDC_XIDILOG:
			g_bXidiLogging = Button_GetCheck(hXidiLog);
			g_bUnsavedChanges = TRUE;
			return TRUE;
		}
		break;
	}

	return FALSE;
}
