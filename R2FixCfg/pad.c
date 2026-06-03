#include "framework.h"
#include "resource.h"
#include "pad.h"
#include "config.h"


char const *g_szXidiPath = ".\\Xidi.ini";
char const *g_szXidiTmpPath = ".\\Xidi.ini.tmp";
char const *szDefaultInputValue = "Null";

static BOOL g_bUsingTmpFile = FALSE;
BOOL g_bUsingSDLBackend = FALSE;
BOOL g_bXidiLogging = FALSE;


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

BOOL fn_bPrepareTmpFile( void )
{
	FILE *hXidi, *hTmp;
	fopen_s(&hTmp, g_szXidiTmpPath, "w");
	if ( hTmp )
	{
		fputs("[Plugins]\n", hTmp);
		fopen_s(&hXidi, g_szXidiPath, "r");
		if ( hXidi )
		{
			int ch = 0;
			while ( (ch = fgetc(hXidi)) != EOF )
				fputc(ch, hTmp);
			fclose(hXidi);
		}
		fclose(hTmp);
		g_bUsingTmpFile = TRUE;
	}
	return g_bUsingTmpFile;
}

BOOL fn_bFinalizeTmpFile( void )
{
	FILE *hXidi, *hTmp;
	BOOL bSuccess = FALSE;

	fopen_s(&hTmp, g_szXidiTmpPath, "r");
	if ( hTmp )
	{
		fopen_s(&hXidi, g_szXidiPath, "w");
		if ( hXidi )
		{
			int ch = 0;
			while ( (ch = fgetc(hTmp)) != EOF && ch != '\n' );
			while ( (ch = fgetc(hTmp)) != EOF )
				fputc(ch, hXidi);
			fclose(hXidi);
			bSuccess = TRUE;
		}
		fclose(hTmp);
		if ( bSuccess )
		{
			DeleteFile(g_szXidiTmpPath);
			g_bUsingTmpFile = FALSE;
		}
	}
	return bSuccess;
}

void PAD_fn_vRead( void )
{
	char szBuffer[C_Pad_MaxCfgString];
	char szBuffer2[C_Pad_MaxCfgString];

	fn_bPrepareTmpFile();
	char const *szPath = (g_bUsingTmpFile) ? g_szXidiTmpPath : g_szXidiPath;

	for ( int i = 0; i < E_NbPadInput; i++ )
	{
		tdstPadConfig *pInput = &g_a_stPadInput[i];

		if ( pInput->bIsAnalog )
		{
			char szNameAxis[C_Pad_MaxName];

			sprintf_s(szNameAxis, C_Pad_MaxName, "%s%s", pInput->szName, "X");
			GetPrivateProfileString("CustomMapper:Ray2Fix", szNameAxis, szDefaultInputValue, szBuffer, sizeof(szBuffer), szPath);
			sprintf_s(szNameAxis, C_Pad_MaxName, "%s%s", pInput->szName, "Y");
			GetPrivateProfileString("CustomMapper:Ray2Fix", szNameAxis, szDefaultInputValue, szBuffer2, sizeof(szBuffer2), szPath);

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
			GetPrivateProfileString("CustomMapper:Ray2Fix", pInput->szName, szDefaultInputValue, szBuffer, sizeof(szBuffer), szPath);

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

	GetPrivateProfileString("Log", "Enabled", "no", szBuffer, sizeof(szBuffer), szPath);
	if ( !_stricmp(szBuffer, "yes") )
		g_bXidiLogging = TRUE;

	if ( g_bUsingTmpFile )
	{
		GetPrivateProfileString("Plugins", "Plugin", NULL, szBuffer, sizeof(szBuffer), szPath);
		g_bUsingSDLBackend = !_stricmp(szBuffer, "SDL");

		GetPrivateProfileString("Plugins", "ControllerBackend", NULL, szBuffer, sizeof(szBuffer), szPath);
	}
}

void PAD_fn_vWrite( void )
{
	char const *szPath = (g_bUsingTmpFile) ? g_szXidiTmpPath : g_szXidiPath;

	WritePrivateProfileString("Mapper", "Type.1", "Ray2Fix", szPath);

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
			WritePrivateProfileString("CustomMapper:Ray2Fix", szNameAxis, pAction->szConfigString_X, szPath);
			sprintf_s(szNameAxis, C_Pad_MaxName, "%s%s", pInput->szName, "Y");
			WritePrivateProfileString("CustomMapper:Ray2Fix", szNameAxis, pAction->szConfigString_Y, szPath);
		}
		else
		{
			tdstAction *pAction = &g_a_stButtonAction[pInput->lAction];
			WritePrivateProfileString("CustomMapper:Ray2Fix", pInput->szName, pAction->szConfigString, szPath);
		}
	}

	WritePrivateProfileString("Log", "Enabled", (g_bXidiLogging) ? "yes" : "no", szPath);
	WritePrivateProfileString("Log", "Level", (g_bXidiLogging) ? "4" : "1", szPath);

	if ( g_bUsingTmpFile )
	{
		if ( g_bUsingSDLBackend )
		{
			WritePrivateProfileString("Plugins", "Plugin", "SDL", szPath);
			WritePrivateProfileString("Plugins", "ControllerBackend", "SDL3", szPath);
		}
		else
		{
			WritePrivateProfileString("Plugins", "Plugin", NULL, szPath);
			WritePrivateProfileString("Plugins", "ControllerBackend", NULL, szPath);
		}
	}

	fn_bFinalizeTmpFile();
}
