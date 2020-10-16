#pragma once

/*
* Author: Brian Atwell
* File: StringUtilities.h
* Description: Handles various String manipulation functions.
*/

#include "stdafx.h"
#include <tchar.h>
#include <strsafe.h>
#include <windows.h>

int StringCopyExcept(_TCHAR* dest, size_t destSize, _TCHAR exceptChar, _TCHAR* source);

int StringEndPosition(_TCHAR *str1, _TCHAR* str2);

BOOL StringEndPosition(_TCHAR *str1, size_t str1Len, _TCHAR str2, size_t& str1Pos);

int ReplaceChar(_TCHAR* str1, size_t maxStr1, _TCHAR findChar, _TCHAR replaceChar);

BOOL ContainsString(_TCHAR* str1, _TCHAR* findStr, size_t maxStrLen);

