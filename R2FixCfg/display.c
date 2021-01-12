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
		{
			return TRUE;
		}
	}

	return FALSE;
}

void EnumResolutions( void )
{
	DEVMODE dm = { 0 };
	dm.dmSize = sizeof(dm);

	int idx = 0;

	for ( int i = 0; EnumDisplaySettings(NULL, i, &dm) != 0; i++ )
	{
		DISP_MODE mode = {
			dm.dmPelsWidth,
			dm.dmPelsHeight,
			DMF_NONE
		};

		if ( IsDuplicateMode(&mode, idx) )
			continue;

		DWORD ratio = 100 * dm.dmPelsHeight / dm.dmPelsWidth;

		// Only add resolutions "close" to 4:3 or 5:4 ratio
		if ( ratio > 73 && ratio < 82 )
		{
			if ( IsSafeResolution(&mode) )
			{
				mode.dmfFlags |= DMF_SAFE;
			}

			g_aDispModes[idx++] = mode;
		}
	}

	// HACK: Choose the 2nd largest resolution as "best" to account for window borders
	// This is a terrible way of doing this, but it should work in most cases anyway.
	if ( idx >= 2 )
	{
		g_aDispModes[idx - 2].dmfFlags |= DMF_BEST;
	}
}
