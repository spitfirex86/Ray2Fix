#include "framework.h"
#include "resource.h"
#include "dialogs.h"
#include "config.h"
#include "main.h"
#include "pad.h"


HWND hDlg;

tdstPadDlgItem a_stPadDlgItem[E_NbPadInput] = {
	#define M_DefineInput(Id, lCtrl, bAnalog, szCfg) { Id, lCtrl, NULL, bAnalog },
	#include "def_pad.h"
};


void fn_vPopulateInputCB( tdstPadDlgItem *pItem )
{
	HWND hCtrl = pItem->hCtrl;

	if ( pItem->bIsAnalog )
	{
		for ( int i = 0; i < E_NbAnalogAction; i++ )
		{
			int lId = ComboBox_AddString(hCtrl, g_a_stAnalogAction[i].szName);
			ComboBox_SetItemData(hCtrl, lId, i);
		}
	}
	else
	{
		for ( int i = 0; i < E_NbButtonAction; i++ )
		{
			int lId = ComboBox_AddString(hCtrl, g_a_stButtonAction[i].szName);
			ComboBox_SetItemData(hCtrl, lId, i);
		}
	}

	ComboBox_SetCurSel(hCtrl, g_a_stPadInput[pItem->lId].lAction);
}

tdstPadDlgItem * fn_p_stGetPadDlgItem( int lCtrlId )
{
	for ( int i = 0; i < E_NbPadInput; i++ )
	{
		tdstPadDlgItem *pItem = &a_stPadDlgItem[i];

		if ( pItem->lCtrlId == lCtrlId )
			return pItem;
	}

	return NULL;
}

void fn_vUpdatePadInput( tdstPadDlgItem *pItem )
{
	tdstPadConfig *pInput = &g_a_stPadInput[pItem->lId];

	int lIdx = ComboBox_GetCurSel(pItem->hCtrl);
	if ( lIdx > -1 )
	{
		int lActionId = ComboBox_GetItemData(pItem->hCtrl, lIdx);
		pInput->lAction = lActionId;
	}
	else
	{
		pInput->lAction = C_Pad_InvalidAction;
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
				tdstPadDlgItem *pItem = &a_stPadDlgItem[i];
				pItem->hCtrl = GetDlgItem(hWnd, pItem->lCtrlId);

				fn_vPopulateInputCB(pItem);
			}
			return TRUE;

		case WM_COMMAND:
		{
			tdstPadDlgItem *pItem = fn_p_stGetPadDlgItem(LOWORD(wParam));
			if ( pItem && HIWORD(wParam) == CBN_SELCHANGE )
			{
				fn_vUpdatePadInput(pItem);
				g_bUnsavedChanges = TRUE;
				return TRUE;
			}
			break;
		}
	}

	return FALSE;
}
