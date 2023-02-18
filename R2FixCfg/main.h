#pragma once

#include "framework.h"


#define LoadRcString(uID, lpBuffer) LoadString(g_hInst, (uID), (lpBuffer), sizeof(lpBuffer))
#define LoadStringAtChar(uID, lpBuffer, lChar) LoadString(g_hInst, (uID), ((lpBuffer)+(lChar)), (sizeof(lpBuffer)-(lChar)-1))

#define WM_ADVTOGGLE (WM_APP + 1)


extern HINSTANCE g_hInst;
extern BOOL g_bUnsavedChanges;
