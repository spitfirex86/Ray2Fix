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

HWND hResX;
HWND hResY;
HWND hResLabel;

HWND hFsModeWin;
HWND hFsModeFs;

HWND hAdvGroup;
HWND hVsync;
HWND hRefRateLabel;
HWND hRefRate;

int lCbCustomIdx;
tdstDisplayMode eLastMode;


tdstDisplayMode * fn_p_stGetSelectedDisplayMode( HWND hCB )
{
	int idx = ComboBox_GetCurSel(hCB);
	tdstDisplayMode *lpMode = (tdstDisplayMode*)ComboBox_GetItemData(hCB, idx);
	
	return lpMode;
}

void fn_vSetDisplayMode( HWND hCB )
{
	tdstDisplayMode *lpMode = fn_p_stGetSelectedDisplayMode(hCB);

	if ( lpMode != NULL )
	{
		g_stCurrentMode.dwWidth = lpMode->dwWidth;
		g_stCurrentMode.dwHeight = lpMode->dwHeight;
	}
}

BOOL fn_bSetCustomDisplayMode( void )
{
	char szBuffer[8];
	
	Edit_GetText(hResX, szBuffer, sizeof(szBuffer) - 1);
	DWORD dwWidth = strtol(szBuffer, NULL, 10);
	Edit_GetText(hResY, szBuffer, sizeof(szBuffer) - 1);
	DWORD dwHeight = strtol(szBuffer, NULL, 10);

	if ( dwWidth > 0 && dwHeight > 0 )
	{
		g_stCurrentMode.dwWidth = dwWidth;
		g_stCurrentMode.dwHeight = dwHeight;

		return TRUE;
	}

	g_stCurrentMode = eLastMode;
	return FALSE;
}

void fn_vUpdateStatus( HWND hEdit, HWND hButton )
{
	char szStatusLine[512];
	char szMsgTemplate[140];
	char szCurrentStatus[60];

	UINT uTemplateId = (g_bFixState == g_bFixPrevState) ? IDS_FIXSTATUS : IDS_STATUSCHANGED;
	UINT uCurrentStatusId = (g_bFixState) ? IDS_U_ENABLED : IDS_U_DISABLED;

	LoadRcString(uTemplateId, szMsgTemplate);
	LoadRcString(uCurrentStatusId, szCurrentStatus);

	int nChars = 0;

	if ( g_eError == e_ES_Ok )
	{
		nChars += sprintf_s(szStatusLine, sizeof(szStatusLine), szMsgTemplate, szCurrentStatus);
		nChars += LoadStringAtChar(IDS_VE_OK, szStatusLine, nChars);
	}
	else if ( g_eError == e_ES_Warning )
	{
		nChars += sprintf_s(szStatusLine, sizeof(szStatusLine), szMsgTemplate, szCurrentStatus);
	}

	if ( g_eError >= e_ES_Warning )
	{
		if ( g_eErrorDetails & e_VE_FilesMissing )
		{
			nChars += LoadStringAtChar(IDS_VE_HEADER, szStatusLine, nChars);

			if ( g_eErrorDetails & e_VE_DegeMissing )
				nChars += LoadStringAtChar(IDS_VE_DEGE, szStatusLine, nChars);

			if ( g_eErrorDetails & e_VE_FixMissing )
				nChars += LoadStringAtChar(IDS_VE_FIX, szStatusLine, nChars);

			if ( g_eErrorDetails & e_VE_DinputMissing )
				nChars += LoadStringAtChar(IDS_VE_DINPUT, szStatusLine, nChars);

			if ( g_eErrorDetails & e_VE_GlideMissing )
				nChars += LoadStringAtChar(IDS_VE_GLIDE, szStatusLine, nChars);

			if ( g_eErrorDetails & e_VE_XidiMissing )
				nChars += LoadStringAtChar(IDS_VE_XIDI, szStatusLine, nChars);
		}
		else
		{
			nChars += LoadStringAtChar(IDS_VE_OKWARN, szStatusLine, nChars);
		}

		if ( g_eError & e_ES_FixError )
			nChars += LoadStringAtChar(IDS_VE_REINSTALLFIX, szStatusLine, nChars);

		if ( g_eError & e_ES_GameError )
			nChars += LoadStringAtChar(IDS_VE_REINSTALLR2, szStatusLine, nChars);

		if ( g_eError & e_ES_Warning )
		{
			if ( g_eErrorDetails & e_VE_XidiMissing )
				nChars += LoadStringAtChar(IDS_VE_SETPAD, szStatusLine, nChars);

			if ( g_eErrorDetails & e_VE_XidiModified )
				nChars += LoadStringAtChar(IDS_VE_XIDICHANGED, szStatusLine, nChars);
		}
	}

	Edit_SetText(hEdit, szStatusLine);
	Button_SetCheck(hButton, g_bFixState);
}

void fn_vShowCustomRes( void )
{
	char szBuffer[8];
	
	sprintf_s(szBuffer, sizeof(szBuffer), "%d", g_stCurrentMode.dwWidth);
	Edit_SetText(hResX, szBuffer);
	sprintf_s(szBuffer, sizeof(szBuffer), "%d", g_stCurrentMode.dwHeight);
	Edit_SetText(hResY, szBuffer);

	eLastMode = g_stCurrentMode;
	
	ShowWindow(hResolution, SW_HIDE);
	ShowWindow(hResX, SW_SHOW);
	ShowWindow(hResY, SW_SHOW);
	ShowWindow(hResLabel, SW_SHOW);
	SetFocus(hResX);
}

void fn_vToggleAdvanced( BOOL bVisible )
{
	int nCmdShow = bVisible ? SW_SHOW : SW_HIDE;

	ShowWindow(hAdvGroup, nCmdShow);
	ShowWindow(hVsync, nCmdShow);
	ShowWindow(hRefRateLabel, nCmdShow);
	ShowWindow(hRefRate, nCmdShow);
}

void fn_vPopulateDisplayModes( HWND hCB )
{
	int idxSelected = -1;

	for ( int i = 0; i < C_MaxModes; i++ )
	{
		tdstDisplayMode *lpMode = &g_a_stDispModes[i];
		char szItemText[60];

		if ( lpMode->dwWidth == 0 )
			break;

		sprintf_s(szItemText, sizeof(szItemText), "%d x %d", lpMode->dwWidth, lpMode->dwHeight);

		if ( lpMode->eFlags & e_DMF_Safe )
			strcat_s(szItemText, sizeof(szItemText), " *");

		if ( lpMode->eFlags & e_DMF_Best )
			strcat_s(szItemText, sizeof(szItemText), " +");

		if ( lpMode->eFlags & e_DMF_Custom )
			strcat_s(szItemText, sizeof(szItemText), " (custom)");

		int idx = ComboBox_AddString(hCB, szItemText);
		ComboBox_SetItemData(hCB, idx, lpMode);

		if ( lpMode->eFlags )
			ComboBox_SetCurSel(hCB, idx);

		if ( lpMode->dwWidth == g_stCurrentMode.dwWidth &&
			 lpMode->dwHeight == g_stCurrentMode.dwHeight )
		{
			idxSelected = idx;
		}
	}

	if ( idxSelected >= 0 )
		ComboBox_SetCurSel(hCB, idxSelected);

	fn_vSetDisplayMode(hResolution);

	// Add special "Custom..." item at the end of the combo box
	lCbCustomIdx = ComboBox_AddString(hCB, "Custom...");
}

void fn_vPopulateRefRates( HWND hCB )
{
	int lFull = ComboBox_AddString(hCB, "60");
	ComboBox_SetItemData(hCB, lFull, e_RR_Full);

	int lHalf = ComboBox_AddString(hCB, "30");
	ComboBox_SetItemData(hCB, lHalf, e_RR_Half);

	if ( g_eRefRate == e_RR_Half )
	{
		ComboBox_SetCurSel(hCB, lHalf);
	}
	else
	{
		ComboBox_SetCurSel(hCB, lFull);
	}
}

BOOL CALLBACK DLG_fn_bProc_General( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
	case WM_INITDIALOG:
		hThis = hWnd;
		hStatus = GetDlgItem(hWnd, IDC_STATUSLINE);
		hToggle = GetDlgItem(hWnd, IDC_MAINTOGGLE);
		hResolution = GetDlgItem(hWnd, IDC_RESOLUTION);
		hResX = GetDlgItem(hWnd, IDC_RESX);
		hResY = GetDlgItem(hWnd, IDC_RESY);
		hResLabel = GetDlgItem(hWnd, IDC_RES_LABEL);
		
		hFsModeWin = GetDlgItem(hWnd, IDC_FSMODE_WND);
		hFsModeFs = GetDlgItem(hWnd, IDC_FSMODE_FS);

		hAdvGroup = GetDlgItem(hWnd, IDC_ADVGROUP);
		hVsync = GetDlgItem(hWnd, IDC_VSYNC);
		hRefRateLabel = GetDlgItem(hWnd, IDC_REFRATE_LABEL);
		hRefRate = GetDlgItem(hWnd, IDC_REFRATE);

		fn_vUpdateStatus(hStatus, hToggle);
		fn_vPopulateDisplayModes(hResolution);
		fn_vPopulateRefRates(hRefRate);

		Button_SetCheck(hVsync, g_bForceVsync);
		Button_SetCheck(hFsModeWin, !g_bFullscreen);
		Button_SetCheck(hFsModeFs, g_bFullscreen);

		SendMessage(hResX, EM_LIMITTEXT, 7, 0);
		SendMessage(hResY, EM_LIMITTEXT, 7, 0);

		return TRUE;

	case WM_ADVTOGGLE:
		fn_vToggleAdvanced((BOOL)wParam);
		return TRUE;

	case WM_COMMAND:
		switch ( LOWORD(wParam) )
		{
		case IDC_MAINTOGGLE:
			g_bFixState = Button_GetCheck(hToggle);
			fn_vUpdateStatus(hStatus, hToggle);

			g_bUnsavedChanges = TRUE;
			return TRUE;

		case IDC_RESOLUTION:
			if ( HIWORD(wParam) == CBN_SELCHANGE )
			{
				if ( ComboBox_GetCurSel(hResolution) == lCbCustomIdx )
				{
					fn_vShowCustomRes();
				}
				else
				{
					fn_vSetDisplayMode(hResolution);
					g_bUnsavedChanges = TRUE;
				}
				return TRUE;
			}
			break;

		case IDC_RESX:
		case IDC_RESY:
			if ( HIWORD(wParam) == EN_KILLFOCUS )
			{
				if ( fn_bSetCustomDisplayMode() )
					g_bUnsavedChanges = TRUE;
				
				return TRUE;
			}
			break;

		case IDC_FSMODE_WND:
			g_bFullscreen = FALSE;
			Button_SetCheck(hFsModeWin, BST_CHECKED);
			Button_SetCheck(hFsModeFs, BST_UNCHECKED);
			return TRUE;

		case IDC_FSMODE_FS:
			g_bFullscreen = TRUE;
			Button_SetCheck(hFsModeWin, BST_UNCHECKED);
			Button_SetCheck(hFsModeFs, BST_CHECKED);
			return TRUE;

		case IDC_VSYNC:
			g_bForceVsync = Button_GetCheck(hVsync);
			g_bUnsavedChanges = TRUE;
			return TRUE;

		case IDC_REFRATE:
			if ( HIWORD(wParam) == CBN_SELCHANGE )
			{
				int idx = ComboBox_GetCurSel(hRefRate);
				tdeRefRate lRefRate = ComboBox_GetItemData(hRefRate, idx);

				if ( lRefRate != 0 )
					g_eRefRate = lRefRate;

				g_bUnsavedChanges = TRUE;
				return TRUE;
			}
			break;
		}
		break;
	}

	return FALSE;
}
