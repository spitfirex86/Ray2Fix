#include "framework.h"
#include "resource.h"
#include "dialogs.h"
#include "display.h"
#include "config.h"
#include "main.h"


static HWND hThis;

static HWND hStatus;
static HWND hToggle;
static HWND hResolution;
static HWND hPatchWidescreen;
static HWND hCleanupSnapShot;

static HWND hResX;
static HWND hResY;
static HWND hResLabel;

static HWND hFsModeWin;
static HWND hFsModeFs;

static HWND hAdvGroup;
static HWND hVsync;
static HWND hRefRateLabel;
static HWND hRefRate;
static HWND hCleanup;

static HWND hDEBUG_WaitFrame;
static HWND hDEBUG_WaitFrameLabel;

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
	ShowWindow(hCleanup, nCmdShow);
	ShowWindow(hDEBUG_WaitFrame, nCmdShow);
	ShowWindow(hDEBUG_WaitFrameLabel, nCmdShow);
}

int fn_lAddThisModeToCB( HWND hCB, tdstDisplayMode *lpMode )
{
	char szItemText[60];

	sprintf_s(szItemText, sizeof(szItemText), "%d x %d", lpMode->dwWidth, lpMode->dwHeight);

	if ( lpMode->eFlags & e_DMF_Safe )
		strcat_s(szItemText, sizeof(szItemText), " *");

	if ( lpMode->eFlags & e_DMF_Best )
		strcat_s(szItemText, sizeof(szItemText), " +");

	if ( lpMode->eFlags & e_DMF_Widescreen )
		strcat_s(szItemText, sizeof(szItemText), " (wide)");

	if ( lpMode->eFlags & e_DMF_Custom )
		strcat_s(szItemText, sizeof(szItemText), " (custom)");

	int idx = ComboBox_AddString(hCB, szItemText);
	ComboBox_SetItemData(hCB, idx, lpMode);

	return idx;
}

void fn_vPopulateDisplayModes( HWND hCB )
{
	int idxSelected = -1;
	int idxFallback = 0;

	for ( tdstDisplayMode *pMode = &g_a_stDispModes[0]; pMode < &g_a_stDispModes[C_MaxModes]; pMode++ )
	{
		if ( pMode->dwWidth == 0 )
			break;
		if ( pMode->eFlags & e_DMF_Widescreen )
			continue;

		int idx = fn_lAddThisModeToCB(hCB, pMode);

		// this is just to make sure a reasonably okay res is selected if all else fails
		if ( pMode->eFlags & (e_DMF_Safe | e_DMF_Best) )
			idxFallback = idx;

		if ( pMode->dwWidth == g_stCurrentMode.dwWidth &&
			 pMode->dwHeight == g_stCurrentMode.dwHeight )
			idxSelected = idx;
	}

	for ( tdstDisplayMode *pMode = &g_a_stDispModes[0]; pMode < &g_a_stDispModes[C_MaxModes]; pMode++ )
	{
		if ( pMode->dwWidth == 0 )
			break;
		if ( !(pMode->eFlags & e_DMF_Widescreen) )
			continue;

		int idx = fn_lAddThisModeToCB(hCB, pMode);

		if ( pMode->dwWidth == g_stCurrentMode.dwWidth &&
			pMode->dwHeight == g_stCurrentMode.dwHeight )
			idxSelected = idx;
	}

	ComboBox_SetCurSel(hCB, (idxSelected >= 0) ? idxSelected : idxFallback);

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

	ComboBox_SetCurSel(hCB, (g_eRefRate == e_RR_Half) ? lHalf : lFull);
}

void fn_vInitDebugWaitFrame( void )
{
	char szBuffer[8];
	sprintf_s(szBuffer, sizeof(szBuffer), "%d", g_DEBUG_lWaitFrame);
	SendMessage(hDEBUG_WaitFrame, EM_LIMITTEXT, 2, 0);
	Edit_SetText(hDEBUG_WaitFrame, szBuffer);
}

void fn_vSetDebugWaitFrame( void )
{
	char szBuffer[8];
	Edit_GetText(hDEBUG_WaitFrame, szBuffer, sizeof(szBuffer) - 1);

	char *pEnd;
	int lWaitFrame = strtol(szBuffer, &pEnd, 10);
	if ( pEnd != szBuffer && lWaitFrame != g_DEBUG_lWaitFrame )
	{
		g_DEBUG_lWaitFrame = lWaitFrame;
		g_bUnsavedChanges = TRUE;
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
		hPatchWidescreen = GetDlgItem(hWnd, IDC_PATCHWIDE);
		hCleanupSnapShot = GetDlgItem(hWnd, IDC_CLEANSNAP);
		
		hFsModeWin = GetDlgItem(hWnd, IDC_FSMODE_WND);
		hFsModeFs = GetDlgItem(hWnd, IDC_FSMODE_FS);

		hAdvGroup = GetDlgItem(hWnd, IDC_ADVGROUP);
		hVsync = GetDlgItem(hWnd, IDC_VSYNC);
		hRefRateLabel = GetDlgItem(hWnd, IDC_REFRATE_LABEL);
		hRefRate = GetDlgItem(hWnd, IDC_REFRATE);
		hCleanup = GetDlgItem(hWnd, IDC_CLEANUP);
		hDEBUG_WaitFrame = GetDlgItem(hWnd, IDC_DEBUG_WF);
		hDEBUG_WaitFrameLabel = GetDlgItem(hWnd, IDC_DEBUG_WF_LABEL);

		fn_vUpdateStatus(hStatus, hToggle);
		fn_vPopulateDisplayModes(hResolution);
		fn_vPopulateRefRates(hRefRate);

		Button_SetCheck(hVsync, g_bForceVsync);
		Button_SetCheck(hFsModeWin, !g_bFullscreen);
		Button_SetCheck(hFsModeFs, g_bFullscreen);
		Button_SetCheck(hPatchWidescreen, g_bPatchWidescreen);
		Button_SetCheck(hCleanupSnapShot, g_bCleanupSnapShot);

		fn_vRegisterToolTip(hWnd, hCleanupSnapShot, IDS_CLEANSNAP);

		SendMessage(hResX, EM_LIMITTEXT, 7, 0);
		SendMessage(hResY, EM_LIMITTEXT, 7, 0);

		fn_vInitDebugWaitFrame();

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

		case IDC_PATCHWIDE:
			g_bPatchWidescreen = Button_GetCheck(hPatchWidescreen);
			g_bUnsavedChanges = TRUE;
			return TRUE;

		case IDC_CLEANSNAP:
			g_bCleanupSnapShot = Button_GetCheck(hCleanupSnapShot);
			g_bUnsavedChanges = TRUE;
			return TRUE;

		case IDC_FSMODE_WND:
			g_bFullscreen = FALSE;
			Button_SetCheck(hFsModeWin, BST_CHECKED);
			Button_SetCheck(hFsModeFs, BST_UNCHECKED);
			g_bUnsavedChanges = TRUE;
			return TRUE;

		case IDC_FSMODE_FS:
			g_bFullscreen = TRUE;
			Button_SetCheck(hFsModeWin, BST_UNCHECKED);
			Button_SetCheck(hFsModeFs, BST_CHECKED);
			g_bUnsavedChanges = TRUE;
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

		case IDC_CLEANUP:
			{
				char szPrompt[250];
				LoadString(g_hInst, IDS_CLEANUPWARN, szPrompt, sizeof(szPrompt));

				if ( MessageBox(hWnd, szPrompt, g_szAppName, MB_OKCANCEL | MB_ICONWARNING) == IDOK )
					fn_vManualCleanUp();

				return TRUE;
			}

		case IDC_DEBUG_WF:
			if ( HIWORD(wParam) == EN_KILLFOCUS )
			{
				fn_vSetDebugWaitFrame();
				return TRUE;
			}
			break;
		}
		break;
	}

	return FALSE;
}
