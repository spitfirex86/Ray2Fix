#include "framework.h"
#include "resource.h"
#include "dialogs.h"
#include "display.h"
#include "config.h"
#include "main.h"

HWND hThis;

HWND hStatus;
HWND hToggle;
HWND hResolution;
HWND hFsModeWin;
HWND hFsModeFs;

HWND hAdvGroup;
HWND hVsync;
HWND hRefRateLabel;
HWND hRefRate;

void SetDisplayMode( HWND hCB )
{
	int idx = ComboBox_GetCurSel(hCB);
	DISP_MODE *lpMode = (DISP_MODE*)ComboBox_GetItemData(hCB, idx);

	if ( lpMode != NULL )
	{
		g_dmCurrentMode.dwWidth = lpMode->dwWidth;
		g_dmCurrentMode.dwHeight = lpMode->dwHeight;
	}
}

void UpdateStatus( HWND hEdit, HWND hButton )
{
	char szStatusLine[512];
	char szMsgTemplate[140];
	char szCurrentStatus[60];

	UINT uTemplateId = (g_bFixState == g_bFixPrevState) ? IDS_FIXSTATUS : IDS_STATUSCHANGED;
	UINT uCurrentStatusId = (g_bFixState) ? IDS_U_ENABLED : IDS_U_DISABLED;

	LoadRcString(uTemplateId, szMsgTemplate);
	LoadRcString(uCurrentStatusId, szCurrentStatus);

	int nChars = 0;

	if ( g_veMissingFiles == VE_OK )
	{
		nChars += sprintf_s(szStatusLine, sizeof(szStatusLine), szMsgTemplate, szCurrentStatus);
		nChars += LoadStringAtChar(IDS_VE_OK, szStatusLine, nChars);
	}
	else
	{
		nChars += LoadStringAtChar(IDS_VE_HEADER, szStatusLine, nChars);

		if ( g_veMissingFiles & VE_DEGE_MISSING )
			nChars += LoadStringAtChar(IDS_VE_DEGE, szStatusLine, nChars);

		if ( g_veMissingFiles & VE_FIX_MISSING )
			nChars += LoadStringAtChar(IDS_VE_FIX, szStatusLine, nChars);

		if ( g_veMissingFiles & VE_DINPUT_MISSING )
			nChars += LoadStringAtChar(IDS_VE_DINPUT, szStatusLine, nChars);

		if ( g_veMissingFiles & VE_GLIDE_MISSING )
			nChars += LoadStringAtChar(IDS_VE_GLIDE, szStatusLine, nChars);

		if ( g_veMissingFiles & VE_FIX_ERROR )
			nChars += LoadStringAtChar(IDS_VE_REINSTALLFIX, szStatusLine, nChars);

		if ( g_veMissingFiles & VE_GAME_ERROR )
			nChars += LoadStringAtChar(IDS_VE_REINSTALLR2, szStatusLine, nChars);

	}

	Edit_SetText(hEdit, szStatusLine);
	Button_SetCheck(hButton, g_bFixState);
}

void ToggleAdvanced( BOOL bVisible )
{
	int nCmdShow = bVisible ? SW_SHOW : SW_HIDE;

	ShowWindow(hAdvGroup, nCmdShow);
	ShowWindow(hVsync, nCmdShow);
	ShowWindow(hRefRateLabel, nCmdShow);
	ShowWindow(hRefRate, nCmdShow);
}

void PopulateDisplayModes( HWND hCB )
{
	int idxSelected = -1;

	for ( int i = 0; i < MAX_MODES; i++ )
	{
		DISP_MODE *lpMode = &g_aDispModes[i];

		if ( lpMode->dwWidth == 0 )
			break;

		char szItemText[60];
		sprintf_s(szItemText, sizeof(szItemText), "%d x %d ", lpMode->dwWidth, lpMode->dwHeight);

		if ( lpMode->dmfFlags & DMF_SAFE )
			strcat_s(szItemText, sizeof(szItemText), "*");

		if ( lpMode->dmfFlags & DMF_BEST )
			strcat_s(szItemText, sizeof(szItemText), "+");

		int idx = ComboBox_AddString(hCB, szItemText);
		ComboBox_SetItemData(hCB, idx, lpMode);

		if ( lpMode->dmfFlags )
			ComboBox_SetCurSel(hCB, idx);

		if ( lpMode->dwWidth == g_dmCurrentMode.dwWidth
			&& lpMode->dwHeight == g_dmCurrentMode.dwHeight )
		{
			idxSelected = idx;
		}
	}

	if ( idxSelected >= 0 )
		ComboBox_SetCurSel(hCB, idxSelected);

	SetDisplayMode(hResolution);
}

void PopulateRefRates( HWND hCB )
{
	int lFull = ComboBox_AddString(hCB, "60");
	ComboBox_SetItemData(hCB, lFull, RR_FULL);

	int lHalf = ComboBox_AddString(hCB, "30");
	ComboBox_SetItemData(hCB, lHalf, RR_HALF);

	if ( g_lRefRate == RR_HALF )
	{
		ComboBox_SetCurSel(hCB, lHalf);
	}
	else
	{
		ComboBox_SetCurSel(hCB, lFull);
	}
}

BOOL CALLBACK GeneralDialogProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
	)
{
	switch ( uMsg )
	{
	case WM_INITDIALOG:
		hThis = hWnd;
		hStatus = GetDlgItem(hWnd, IDC_STATUSLINE);
		hToggle = GetDlgItem(hWnd, IDC_MAINTOGGLE);
		hResolution = GetDlgItem(hWnd, IDC_RESOLUTION);
		hFsModeWin = GetDlgItem(hWnd, IDC_FSMODE_WND);
		hFsModeFs = GetDlgItem(hWnd, IDC_FSMODE_FS);

		hAdvGroup = GetDlgItem(hWnd, IDC_ADVGROUP);
		hVsync = GetDlgItem(hWnd, IDC_VSYNC);
		hRefRateLabel = GetDlgItem(hWnd, IDC_REFRATE_LABEL);
		hRefRate = GetDlgItem(hWnd, IDC_REFRATE);

		UpdateStatus(hStatus, hToggle);
		PopulateDisplayModes(hResolution);
		PopulateRefRates(hRefRate);

		Button_SetCheck(hVsync, g_bForceVsync);
		Button_SetCheck(hFsModeWin, !g_bFullscreen);
		Button_SetCheck(hFsModeFs, g_bFullscreen);

		break;

	case WM_ADVTOGGLE:
		ToggleAdvanced((BOOL)wParam);
		break;

	case WM_COMMAND:
		switch ( LOWORD(wParam) )
		{
		case IDC_MAINTOGGLE:
			g_bFixState = Button_GetCheck(hToggle);
			UpdateStatus(hStatus, hToggle);

			g_bUnsavedChanges = TRUE;
			break;

		case IDC_RESOLUTION:
			if ( HIWORD(wParam) == CBN_SELCHANGE )
			{
				SetDisplayMode(hResolution);
				g_bUnsavedChanges = TRUE;
				break;
			}
			return FALSE;

		case IDC_FSMODE_WND:
			g_bFullscreen = FALSE;
			Button_SetCheck(hFsModeWin, BST_CHECKED);
			Button_SetCheck(hFsModeFs, BST_UNCHECKED);
			break;

		case IDC_FSMODE_FS:
			g_bFullscreen = TRUE;
			Button_SetCheck(hFsModeWin, BST_UNCHECKED);
			Button_SetCheck(hFsModeFs, BST_CHECKED);
			break;

		case IDC_VSYNC:
			g_bForceVsync = Button_GetCheck(hVsync);
			g_bUnsavedChanges = TRUE;
			break;

		case IDC_REFRATE:
			if ( HIWORD(wParam) == CBN_SELCHANGE )
			{
				int idx = ComboBox_GetCurSel(hRefRate);
				REFRATE lRefRate = ComboBox_GetItemData(hRefRate, idx);

				if ( lRefRate != 0 )
					g_lRefRate = lRefRate;

				g_bUnsavedChanges = TRUE;
				break;
			}
			return FALSE;

		default: return FALSE;
		}
		break;

	default: return FALSE;
	}

	return FALSE;
}
