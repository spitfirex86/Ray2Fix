#include "framework.h"
#include "resource.h"
#include "pad.h"
#include "config.h"


char const *g_szXidiPath = ".\\Xidi.ini";
char const *szDefaultInputValue = "Null";

tdstAnalogAction g_a_stAnalogAction[E_NbAnalogAction] = {
	#define M_DefineAction(Id, szName, szCfg_X, szCfg_Y) { Id, szName, szCfg_X, szCfg_Y },
	#include "def_analog.h"
};

tdstAction g_a_stButtonAction[E_NbButtonAction] = {
	#define M_DefineAction(Id, szName, szCfg) { Id, szName, szCfg },
	#include "def_button.h"
};

tdstPadConfig g_a_stPadInput[E_NbPadInput] = {
	#define M_DefineInput(Id, lCtrl, bAnalog, szCfg) { Id, E_BAc_Null, bAnalog, szCfg },
	#include "def_pad.h"
};


tdstAnalogAction * fn_p_stFindAnalogAction( char const *szCfg_X, char const *szCfg_Y )
{
	for ( int i = 0; i < E_NbAnalogAction; i++ )
	{
		tdstAnalogAction *pAction = &g_a_stAnalogAction[i];

		if ( !_stricmp(pAction->szConfigString_X, szCfg_X) && !_stricmp(pAction->szConfigString_Y, szCfg_Y) )
			return pAction;
	}

	return NULL;
}

tdstAction * fn_p_stFindButtonAction( char const *szCfg )
{
	for ( int i = 0; i < E_NbButtonAction; i++ )
	{
		tdstAction *pAction = &g_a_stButtonAction[i];

		if ( !_stricmp(pAction->szConfigString, szCfg) )
			return pAction;
	}

	return NULL;
}

void PAD_fn_vRead( void )
{
	char szBuffer[C_Pad_MaxCfgString];
	char szBuffer2[C_Pad_MaxCfgString];

	for ( int i = 0; i < E_NbPadInput; i++ )
	{
		tdstPadConfig *pInput = &g_a_stPadInput[i];

		if ( pInput->bIsAnalog )
		{
			char szNameAxis[C_Pad_MaxName];

			sprintf_s(szNameAxis, C_Pad_MaxName, "%s%s", pInput->szName, "X");
			GetPrivateProfileString("CustomMapper:Ray2Fix", szNameAxis, szDefaultInputValue, szBuffer, sizeof(szBuffer), g_szXidiPath);
			sprintf_s(szNameAxis, C_Pad_MaxName, "%s%s", pInput->szName, "Y");
			GetPrivateProfileString("CustomMapper:Ray2Fix", szNameAxis, szDefaultInputValue, szBuffer2, sizeof(szBuffer2), g_szXidiPath);

			tdstAnalogAction *pAction = fn_p_stFindAnalogAction(szBuffer, szBuffer2);
			if ( pAction )
			{
				pInput->lAction = pAction->lId;
			}
			else
			{
				pInput->lAction = C_Pad_InvalidAction;
				g_eError |= e_ES_Warning;
				g_eErrorDetails |= e_VE_XidiModified;
			}
		}
		else
		{
			GetPrivateProfileString("CustomMapper:Ray2Fix", pInput->szName, szDefaultInputValue, szBuffer, sizeof(szBuffer), g_szXidiPath);

			tdstAction *pAction = fn_p_stFindButtonAction(szBuffer);
			if ( pAction )
			{
				pInput->lAction = pAction->lId;
			}
			else
			{
				pInput->lAction = C_Pad_InvalidAction;
				g_eError |= e_ES_Warning;
				g_eErrorDetails |= e_VE_XidiModified;
			}
		}
	}
}

void PAD_fn_vWrite( void )
{
	WritePrivateProfileString("Mapper", "Type.1", "Ray2Fix", g_szXidiPath);

	for ( int i = 0; i < E_NbPadInput; i++ )
	{
		tdstPadConfig *pInput = &g_a_stPadInput[i];

		if ( pInput->lAction == C_Pad_InvalidAction )
			continue;

		if ( pInput->bIsAnalog )
		{
			tdstAnalogAction *pAction = &g_a_stAnalogAction[pInput->lAction];
			char szNameAxis[C_Pad_MaxName];

			sprintf_s(szNameAxis, C_Pad_MaxName, "%s%s", pInput->szName, "X");
			WritePrivateProfileString("CustomMapper:Ray2Fix", szNameAxis, pAction->szConfigString_X, g_szXidiPath);
			sprintf_s(szNameAxis, C_Pad_MaxName, "%s%s", pInput->szName, "Y");
			WritePrivateProfileString("CustomMapper:Ray2Fix", szNameAxis, pAction->szConfigString_Y, g_szXidiPath);
			
		}
		else
		{
			tdstAction *pAction = &g_a_stButtonAction[pInput->lAction];
			WritePrivateProfileString("CustomMapper:Ray2Fix", pInput->szName, pAction->szConfigString, g_szXidiPath);
		}
	}
}