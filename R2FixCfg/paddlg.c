#include "framework.h"
#include "resource.h"
#include "dialogs.h"
#include "config.h"
#include "main.h"


HWND hDlg;


BOOL CALLBACK PadDialogProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case WM_INITDIALOG:
			hDlg = hWnd;

			return TRUE;

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
			}
			break;
	}

	return FALSE;
}
