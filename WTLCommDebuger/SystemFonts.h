#pragma once

#include <windows.h>

// Get the LOGFONT for the system default monospace font.
// Uses "Consolas" (available on Windows Vista+) with DPI-aware sizing
// derived from the system message font.
inline void GetSystemMonospaceFontLogFont(LOGFONT& lf)
{
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
	lf = ncm.lfMessageFont;
	lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lstrcpy(lf.lfFaceName, _T("Consolas"));
}
