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

BOOL CALLBACK TweaksDialogProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
	)
{
	switch ( uMsg )
	{
	case WM_INITDIALOG:
		hDlg = hWnd;

		hShowPos = GetDlgItem(hWnd, IDC_SHOWPOS);
		hLumControl = GetDlgItem(hWnd, IDC_LUMCONTROL);
		hModLoader = GetDlgItem(hWnd, IDC_MODLOADER);

		Button_SetCheck(hShowPos, (g_lTweaks & TWK_SHOWPOS) ? TRUE : FALSE);
		Button_SetCheck(hLumControl, (g_lTweaks & TWK_LUMCONTROL) ? TRUE : FALSE);
		Button_SetCheck(hModLoader, (g_lTweaks & TWK_MODLOADER) ? TRUE : FALSE);
		break;

	case WM_COMMAND:
		switch ( LOWORD(wParam) )
		{
		case IDC_SHOWPOS:
			if ( Button_GetCheck(hShowPos) )
			{
				g_lTweaks |= TWK_SHOWPOS;
			}
			else
			{
				g_lTweaks &= ~TWK_SHOWPOS;
			}
			g_bUnsavedChanges = TRUE;
			break;
			
		case IDC_LUMCONTROL:
			if ( Button_GetCheck(hLumControl) )
			{
				g_lTweaks |= TWK_LUMCONTROL;
			}
			else
			{
				g_lTweaks &= ~TWK_LUMCONTROL;
			}
			g_bUnsavedChanges = TRUE;
			break;
			
		case IDC_MODLOADER:
			if ( Button_GetCheck(hModLoader) )
			{
				g_lTweaks |= TWK_MODLOADER;
			}
			else
			{
				g_lTweaks &= ~TWK_MODLOADER;
			}
			g_bUnsavedChanges = TRUE;
			break;
			
		default: return FALSE;
		}
		break;

	default: return FALSE;
	}

	return TRUE;
}
