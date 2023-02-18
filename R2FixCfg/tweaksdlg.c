#include "framework.h"
#include "resource.h"
#include "dialogs.h"
#include "config.h"
#include "main.h"


HWND hDlg;

HWND hShowPos;
HWND hLumControl;
HWND hModLoader;

// TODO: reenable checkboxes after adding missing tweaks


BOOL CALLBACK DLG_fn_bProc_Tweaks( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
	case WM_INITDIALOG:
		hDlg = hWnd;

		hShowPos = GetDlgItem(hWnd, IDC_SHOWPOS);
		hLumControl = GetDlgItem(hWnd, IDC_LUMCONTROL);
		hModLoader = GetDlgItem(hWnd, IDC_MODLOADER);

		Button_SetCheck(hShowPos, (g_eTweaks & e_TWK_ShowPos) ? TRUE : FALSE);
		Button_SetCheck(hLumControl, (g_eTweaks & e_TWK_LuminosityKeys) ? TRUE : FALSE);
		Button_SetCheck(hModLoader, (g_eTweaks & e_TWK_ModLoader) ? TRUE : FALSE);
		return TRUE;

	case WM_COMMAND:
		switch ( LOWORD(wParam) )
		{
		case IDC_SHOWPOS:
			if ( Button_GetCheck(hShowPos) )
			{
				g_eTweaks |= e_TWK_ShowPos;
			}
			else
			{
				g_eTweaks &= ~e_TWK_ShowPos;
			}
			g_bUnsavedChanges = TRUE;
			return TRUE;
			
		case IDC_LUMCONTROL:
			if ( Button_GetCheck(hLumControl) )
			{
				g_eTweaks |= e_TWK_LuminosityKeys;
			}
			else
			{
				g_eTweaks &= ~e_TWK_LuminosityKeys;
			}
			g_bUnsavedChanges = TRUE;
			return TRUE;
			
		case IDC_MODLOADER:
			if ( Button_GetCheck(hModLoader) )
			{
				g_eTweaks |= e_TWK_ModLoader;
			}
			else
			{
				g_eTweaks &= ~e_TWK_ModLoader;
			}
			g_bUnsavedChanges = TRUE;
			return TRUE;
		}
		break;
	}

	return FALSE;
}
