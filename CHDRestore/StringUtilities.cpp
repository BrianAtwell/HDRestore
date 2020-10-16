/*
* Author: Brian Atwell
* File: StringUtilities.cpp
* Description: Handles various String manipulation functions.
*/

#include "stdafx.h"

#include "StringUtilities.h"

#include <tchar.h>
#include <strsafe.h>
#include <windows.h>

/*
* Description: Copy source to dest except if it is exceptChar.
*/
int StringCopyExcept(_TCHAR* dest, size_t destSize, _TCHAR exceptChar, _TCHAR* source)
{
	size_t sourcePos = 0;
	size_t destPos = 0;
	int numberCharsSkipped = 0;

	while (destPos < destSize && source[sourcePos] != _T('\0'))
	{
		if (source[sourcePos] != exceptChar)
		{
			dest[destPos] = source[sourcePos];
			destPos++;
		}
		else
		{
			numberCharsSkipped++;
		}

		sourcePos++;

	}

	return numberCharsSkipped;
}

/*
* Description: Returns position of the end of str2 in str1.
*/
int StringEndPosition(_TCHAR *str1, _TCHAR* str2)
{
	int index = 0;
	int offset = 0;

	while (str1[index + offset] != str2[index] && str1[index + offset] != 0 && str2[index] != 0)
	{
		offset++;
	}

	while (str1[index + offset] == str2[index] && str1[index + offset] != 0 && str2[index] != 0)
	{
		index++;
	}
	return index + offset;
}

/*
* Description: Returns position of the end of str2 in str1.
*/
BOOL StringEndPosition(_TCHAR *str1, size_t str1Len, _TCHAR str2, size_t& str1Pos)
{
	size_t index = 0;

	while (str1[index] != str2 && str1[index] != 0 && index < str1Len)
	{
		index++;
	}
	if (str1[index] == str2 && str1[index] != 0 && index < str1Len)
	{
		str1Pos = index;
		return 1;
	}
	return 0;
}

/*
* Description: Replaces findChar with replaceChar in str1. The contents of str1 will be modified.
*
*/
int ReplaceChar(_TCHAR* str1, size_t maxStr1, _TCHAR findChar, _TCHAR replaceChar)
{
	int retVal = 0;
	for (size_t i = 0; i < maxStr1 && str1[i] != _T('\0'); i++)
	{
		if (str1[i] == findChar)
		{
			str1[i] = replaceChar;
			retVal++;
		}
	}
	return retVal;
}

/*
* Description: Returns true if str1 contains findStr
*
*/
BOOL ContainsString(_TCHAR* str1, _TCHAR* findStr, size_t maxStrLen)
{
	size_t pos = 0;
	size_t lenFindStr = 0;

	if (StringCchLength(findStr, maxStrLen, &lenFindStr) == S_OK)
	{
		pos = StringEndPosition(str1, findStr);
		if (lenFindStr == pos)
		{
			return true;
		}
	}
	_tprintf(_T("ContainsString failed StringCchLength failed!\n"));

	return false;
}