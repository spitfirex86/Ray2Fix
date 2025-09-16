#include "framework.h"
#include "extra.h"
#include "config.h"

#include <ACP_Ray2.h>


BOOL EXT_bSeemsLegalForSpeedruns = FALSE;


BOOL fn_bCleanupSnapShot( int *p_lCounter, int *p_lDeleted )
{
	int lCounter = 0;
	int lDeleted = 0;
	BOOL bResult = TRUE;

	WIN32_FIND_DATA stFFD;
	HANDLE hFound = FindFirstFile(".\\SnapShot\\Snap*.bmp", &stFFD);
	if ( hFound == INVALID_HANDLE_VALUE )
	{
		bResult = (GetLastError() == ERROR_FILE_NOT_FOUND);
		*p_lCounter = 0;
		*p_lDeleted = 0;
		return bResult;
	}

	char szFilePath[MAX_PATH];
	do
	{
		if ( stFFD.dwFileAttributes & FILE_ATTRIBUTE_READONLY )
			continue;
		++lCounter;
		sprintf_s(szFilePath, sizeof(szFilePath), ".\\SnapShot\\%s", stFFD.cFileName);
		printf(szFilePath);
		if ( DeleteFile(szFilePath) )
			++lDeleted;
		else
			bResult = FALSE;
	}
	while ( FindNextFile(hFound, &stFFD) );

	bResult &= (GetLastError() == ERROR_NO_MORE_FILES);
	FindClose(hFound);

	*p_lCounter = lCounter;
	*p_lDeleted = lDeleted;
	return bResult;
}

void EXT_fn_vDealWithSnapShot( void )
{
	if ( g_bConfigToolIsAboutToOpen )
		return;

	int lFiles, lDeleted;
	BOOL bResult = fn_bCleanupSnapShot(&lFiles, &lDeleted);

	char szBuffer[512];
	int lWritten = sprintf_s(szBuffer, sizeof(szBuffer), "Cleaned up %d / %d screenshots.\n\n", lDeleted, lFiles);
	UINT uIcon = MB_ICONASTERISK;
	if ( bResult )
	{
		if ( lDeleted == 0 && lFiles == 0 )
			return;

		char const *szTagLine = NULL;
		if ( lDeleted == 0 )
			szTagLine = "No screenshots this time!";
		else if ( lDeleted == 1 )
			szTagLine = "Just one, I guess.";
		else if ( lDeleted <= 50 ) 
			szTagLine = "A reasonable amount.";
		else if ( lDeleted <= 400 )
			szTagLine = "Speedrunner detected!\n(Or you just really love pressing the F8 key.)";
		else if ( lDeleted <= 800 )
			szTagLine = "You're a Screenshot's Master!!";
		else if ( lDeleted <= 2000 )
			szTagLine = "It's possible that you may have a problem.";
		else
			szTagLine = "Lemme guess, you're a speedrunner and this is the first time you're using the cleanup tool...";


		unsigned long ulBytesPer = CFG_stDispMode.dwWidth * CFG_stDispMode.dwHeight * 3;
		float fKbPer = ulBytesPer / 1024.f;
		float fTotalConsumed = lDeleted * fKbPer;
		char *pcUnit = "kMGTP";
		while ( fTotalConsumed > 1024 && pcUnit[1] )
		{
			fTotalConsumed /= 1024.f;
			++pcUnit;
		}

		lWritten += sprintf_s(szBuffer+lWritten, sizeof(szBuffer)-lWritten,
			"%s\n\nFun fact:\nAt the current game resolution, %d screenshots take up approximately %.2f%cB of disk space.",
			szTagLine, lDeleted, fTotalConsumed, *pcUnit);
	}
	else
	{
		sprintf_s(szBuffer+lWritten, sizeof(szBuffer)-lWritten,
			"An error occurred while deleting. Please delete any remaining screenshots manually.");
		uIcon = MB_ICONEXCLAMATION;
	}

	HWND hWnd = GAM_fn_hGetWindowHandle();
	ShowCursor(TRUE);
	MessageBox((IsWindow(hWnd) ? hWnd : NULL), szBuffer, "Session Stats", MB_OK | uIcon);
}

void EXT_fn_vInitEvaluateSpeedrunValidity( void )
{
	BOOL bTest = (
		CFG_bIsMainModuleR2 && /* sanity checks */
		CFG_bIsFixEnabled &&
		!(CFG_bIsWidescreen && CFG_bPatchWidescreen) && /* no widescreen */
		!CFG_bHalfRefRate && /* 60fps only */
		(CFG_DEBUG_lWaitFrame == 0) && /* default framerate fix */
		(CFG_lDegeFPSLimit == 60) /* refresh rate fix (screenshot lag) */
		);

	EXT_bSeemsLegalForSpeedruns = bTest;
}