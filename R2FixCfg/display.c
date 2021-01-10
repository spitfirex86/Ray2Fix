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

void EnumResolutions( void )
{
	DEVMODE dm = { 0 };
	dm.dmSize = sizeof(dm);

	int idx = 0;
	DISP_MODE lastMode = { 0 };

	for ( int i = 0; EnumDisplaySettings(NULL, i, &dm) != 0; i++ )
	{
		// Skip display mode if resolution is the same as previous one
		if ( dm.dmPelsWidth == lastMode.dwWidth && dm.dmPelsHeight == lastMode.dwHeight )
			continue;

		lastMode.dwWidth = dm.dmPelsWidth;
		lastMode.dwHeight = dm.dmPelsHeight;
		lastMode.dmfFlags = DMF_NONE;

		DWORD ratio = 100 * dm.dmPelsHeight / dm.dmPelsWidth;

		// Only add resolutions "close" to 4:3 or 5:4 ratio
		if ( ratio > 73 && ratio < 82 )
		{
			if ( IsSafeResolution(&lastMode) )
			{
				lastMode.dmfFlags |= DMF_SAFE;
			}

			g_aDispModes[idx++] = lastMode;
		}
	}

	// HACK: Choose the 2nd largest resolution as "best" to account for window borders
	// This is a terrible way of doing this, but it should work in most cases anyway.
	if ( idx >= 2 )
	{
		g_aDispModes[idx - 2].dmfFlags |= DMF_BEST;
	}
}
