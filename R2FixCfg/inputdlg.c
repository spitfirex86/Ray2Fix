#include "framework.h"
#include "resource.h"
#include "dialogs.h"
#include "main.h"
#include "pad.h"


static HWND hThis;

static HWND hBackendSDL;
static HWND hBackendXinput;

static HWND hDeadzoneLT;
static HWND hDeadzoneRT;
static HWND hAdvGroup;
static HWND hXidiLog;


#define Trackbar_GetPos(hwndCtl) ((int)SNDMSG((hwndCtl), TBM_GETPOS, 0, 0))
#define Trackbar_SetPos(hwndCtl, pos) ((void)SNDMSG((hwndCtl), TBM_SETPOS, TRUE, (LPARAM)(pos)))
#define Trackbar_SetRange(hwndCtl, uwMin, uwMax) ((void)SNDMSG((hwndCtl), TBM_SETRANGE, TRUE, MAKELPARAM((uwMin), (uwMax))))
#define Trackbar_SetPageSize(hwndCtl, size) ((int)SNDMSG((hwndCtl), TBM_SETPAGESIZE, 0, (LPARAM)(size)))
#define Trackbar_SetTicFreq(hwndCtl, freq) ((void)SNDMSG((hwndCtl), TBM_SETTICFREQ, (WPARAM)(freq), 0))

static void fn_vToggleAdvanced( BOOL bVisible )
{
	int nCmdShow = bVisible ? SW_SHOW : SW_HIDE;
	ShowWindow(hAdvGroup, nCmdShow);
	ShowWindow(hXidiLog, nCmdShow);
}

static void fn_vInitTrackbar( HWND hTB, long lInitPos )
{
	Trackbar_SetRange(hTB, 0, 45);
	Trackbar_SetPageSize(hTB, 5);
	Trackbar_SetTicFreq(hTB, 5);
	Trackbar_SetPos(hTB, lInitPos);
}

static BOOL fn_bHandleTrackbar( HWND hCtrl, HWND hTB, long *plValueOut )
{
	if ( hCtrl == hTB )
	{
		long lPos = Trackbar_GetPos(hTB);
		*plValueOut = lPos;
		g_bUnsavedChanges = TRUE;
		return TRUE;
	}
	return FALSE;
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
		hDeadzoneLT = GetDlgItem(hWnd, IDC_DEADZONE_LT);
		hDeadzoneRT = GetDlgItem(hWnd, IDC_DEADZONE_RT);

		Button_SetCheck(hBackendSDL, g_bUsingSDLBackend);
		Button_SetCheck(hBackendXinput, !g_bUsingSDLBackend);
		Button_SetCheck(hXidiLog, g_bXidiLogging);

		fn_vInitTrackbar(hDeadzoneLT, g_lDeadzoneLT);
		fn_vInitTrackbar(hDeadzoneRT, g_lDeadzoneRT);

		return TRUE;

	case WM_ADVTOGGLE:
		fn_vToggleAdvanced((BOOL)wParam);
		return TRUE;

	case WM_HSCROLL:
			return fn_bHandleTrackbar((HWND)lParam, hDeadzoneLT, &g_lDeadzoneLT)
				|| fn_bHandleTrackbar((HWND)lParam, hDeadzoneRT, &g_lDeadzoneRT);

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
