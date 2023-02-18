#include "framework.h"
#include "resource.h"
#include "dialogs.h"
#include "config.h"
#include "main.h"
#include "pad.h"


tdstAnalogAction a_stAnalogAction[E_NbAnalogAction] = {
	#define M_DefineAction(Id, szName, szCfg_X, szCfg_Y) [Id] = { szName, szCfg_X, szCfg_Y },
	#include "def_analog.h"
};
tdstAction a_stButtonAction[E_NbButtonAction] = {
	#define M_DefineAction(Id, szName, szCfg) { szName, szCfg },
	#include "def_button.h"
};

tdstPadConfig a_stPadInput[E_NbPadInput] = {
	#define M_DefineInput(Id, lCtrl, bAnalog, szCfg) { lCtrl, NULL, bAnalog, 0, szCfg },
	#include "def_pad.h"
};

HWND hDlg;


void fn_vPopulateInputCB( tdstPadConfig *pInput )
{
	HWND hCtrl = pInput->hCtrl;

	if ( pInput->bIsAnalog )
	{
		for ( int i = 0; i < E_NbAnalogAction; i++ )
		{
			int lId = ComboBox_AddString(hCtrl, a_stAnalogAction[i].szName);
			ComboBox_SetItemData(hCtrl, lId, i);
		}
	}
	else
	{
		for ( int i = 0; i < E_NbButtonAction; i++ )
		{
			int lId = ComboBox_AddString(hCtrl, a_stButtonAction[i].szName);
			ComboBox_SetItemData(hCtrl, lId, i);
		}
	}
}

BOOL CALLBACK DLG_fn_bProc_Pad( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case WM_INITDIALOG:
			hDlg = hWnd;

			for ( int i = 0; i < E_NbPadInput; i++ )
			{
				tdstPadConfig *pInput = &a_stPadInput[i];
				pInput->hCtrl = GetDlgItem(hWnd, pInput->lCtrlId);

				fn_vPopulateInputCB(pInput);
			}
			return TRUE;

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
			}
			break;
	}

	return FALSE;
}
