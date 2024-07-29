#include "framework.h"
#include "config.h"


tdstDisplayMode g_a_stDispModes[C_MaxModes];


BOOL fn_bIsSafeResolution( tdstDisplayMode *lpMode )
{
	// Safe resolutions are the two most commonly used in speedruns: 1024x768 and 1280x1024.
	// They're the most likely to work without any issues on most PCs.

	if ( lpMode->dwWidth == 1024 && lpMode->dwHeight == 768 )
		return TRUE;

	if ( lpMode->dwWidth == 1280 && lpMode->dwHeight == 1024 )
		return TRUE;

	return FALSE;
}

BOOL fn_bIsDuplicateMode( tdstDisplayMode *lpMode, int nModes )
{
	// This is dumb, but apparently EnumDisplaySettings does not always return the resolutions
	// in an order that makes sense. Which means we can't just compare it to the previous one
	// to check if it's duplicated/already present in the array.

	for ( int i = 0; i < nModes; i++ )
	{
		tdstDisplayMode *prevMode = &g_a_stDispModes[i];

		if ( lpMode->dwWidth == prevMode->dwWidth && lpMode->dwHeight == prevMode->dwHeight )
			return TRUE;
	}

	return FALSE;
}

void fn_vAddCustomModeIfSet( int nModes )
{
	if ( g_stCurrentMode.dwWidth <= 0 || g_stCurrentMode.dwHeight <= 0 ||
		 fn_bIsDuplicateMode(&g_stCurrentMode, nModes) )
		return;

	g_stCurrentMode.eFlags |= e_DMF_Custom;
	g_a_stDispModes[nModes] = g_stCurrentMode;
}

void fn_vFindBestResolution( int nModes )
{
	RECT rcWorkArea = { 0 };
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);

	for ( int i = nModes - 1; i >= 0; i-- )
	{
		tdstDisplayMode *mode = &g_a_stDispModes[i];

		// Find the largest resolution that fits in the work area
		if ( (int)mode->dwWidth <= rcWorkArea.right && (int)mode->dwHeight <= rcWorkArea.bottom )
		{
			mode->eFlags |= e_DMF_Best;
			return;
		}
	}
}

void DSP_fn_vEnumResolutions( void )
{
	DEVMODE dm = { 0 };
	dm.dmSize = sizeof(dm);

	int nModes = 0;

	for ( int i = 0; EnumDisplaySettings(NULL, i, &dm) != 0; i++ )
	{
		tdstDisplayMode mode = {
			dm.dmPelsWidth,
			dm.dmPelsHeight,
			e_DMF_None
		};

		if ( mode.dwWidth <= 0 || mode.dwHeight <= 0 )
			continue;

		if ( fn_bIsDuplicateMode(&mode, nModes) )
			continue;

		DWORD ratio = 100 * dm.dmPelsHeight / dm.dmPelsWidth;

		// Only add resolutions "close" to 4:3 or 5:4 ratio
		if ( /*ratio > 73 &&*/ ratio < 82 ) // temp: allow for widescreen
		{
			if ( fn_bIsSafeResolution(&mode) )
				mode.eFlags |= e_DMF_Safe;

			g_a_stDispModes[nModes++] = mode;
		}
	}

	fn_vFindBestResolution(nModes);
	fn_vAddCustomModeIfSet(nModes);
}
