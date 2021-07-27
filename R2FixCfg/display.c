#include "framework.h"
#include "config.h"

DISP_MODE g_aDispModes[MAX_MODES];

BOOL IsSafeResolution( DISP_MODE *lpMode )
{
	// Safe resolutions are the two most commonly used in speedruns: 1024x768 and 1280x1024.
	// They're the most likely to work without any issues on most PCs.

	if ( lpMode->dwWidth == 1024 && lpMode->dwHeight == 768 )
		return TRUE;

	if ( lpMode->dwWidth == 1280 && lpMode->dwHeight == 1024 )
		return TRUE;

	return FALSE;
}

BOOL IsDuplicateMode( DISP_MODE *lpMode, int nModes )
{
	// This is dumb, but apparently EnumDisplaySettings does not always return the resolutions
	// in an order that makes sense. Which means we can't just compare it to the previous one
	// to check if it's duplicated/already present in the array.

	for ( int i = 0; i < nModes; i++ )
	{
		DISP_MODE *prevMode = &g_aDispModes[i];

		if ( lpMode->dwWidth == prevMode->dwWidth && lpMode->dwHeight == prevMode->dwHeight )
			return TRUE;
	}

	return FALSE;
}

void AddCustomModeIfSet( int nModes )
{
	if ( g_dmCurrentMode.dwWidth <= 0 || g_dmCurrentMode.dwHeight <= 0 ||
		 IsDuplicateMode(&g_dmCurrentMode, nModes) )
		return;

	g_dmCurrentMode.dmfFlags |= DMF_CUSTOM;
	g_aDispModes[nModes] = g_dmCurrentMode;
}

void FindBestResolution( int nModes )
{
	RECT rcWorkArea = { 0 };
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);

	for ( int i = nModes - 1; i >= 0; i-- )
	{
		DISP_MODE *mode = &g_aDispModes[i];

		// Find the largest resolution that fits in the work area
		if ( (int)mode->dwWidth <= rcWorkArea.right && (int)mode->dwHeight <= rcWorkArea.bottom )
		{
			mode->dmfFlags |= DMF_BEST;
			return;
		}
	}
}

void EnumResolutions( void )
{
	DEVMODE dm = { 0 };
	dm.dmSize = sizeof(dm);

	int nModes = 0;

	for ( int i = 0; EnumDisplaySettings(NULL, i, &dm) != 0; i++ )
	{
		DISP_MODE mode = {
			dm.dmPelsWidth,
			dm.dmPelsHeight,
			DMF_NONE
		};

		if ( mode.dwWidth <= 0 || mode.dwHeight <= 0 )
			continue;

		if ( IsDuplicateMode(&mode, nModes) )
			continue;

		DWORD ratio = 100 * dm.dmPelsHeight / dm.dmPelsWidth;

		// Only add resolutions "close" to 4:3 or 5:4 ratio
		if ( ratio > 73 && ratio < 82 )
		{
			if ( IsSafeResolution(&mode) )
				mode.dmfFlags |= DMF_SAFE;

			g_aDispModes[nModes++] = mode;
		}
	}

	FindBestResolution(nModes);
	AddCustomModeIfSet(nModes);
}
