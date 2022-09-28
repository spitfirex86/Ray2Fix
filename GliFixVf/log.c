#include "log.h"
#include "framework.h"

char const *szLogPath = "Ray2Fix.log";

BOOL bLogInit = FALSE;

void LOG_fn_vInit( void )
{
	FILE *hFile = fopen(szLogPath, "w");
	if ( hFile )
	{
		fclose(hFile);
		bLogInit = TRUE;
	}
}

void LOG_fn_vPrintf( char const *szFmt, ... )
{
	va_list args;
	va_start(args, szFmt);

	FILE *hFile = fopen(szLogPath, "a");
	if ( hFile )
	{
		SYSTEMTIME stTime = { 0 };
		GetLocalTime(&stTime);
		fprintf(hFile, "[%02d:%02d:%02d.%03d] ", stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds);

		vfprintf(hFile, szFmt, args);
		fprintf(hFile, "\n");

		fclose(hFile);
	}

	va_end(args);
}
