/***************************************************************************
 *   Copyright (C) 2021 by Chernov A.A.                                    *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#include "utf8_proc.h"


size_t myutf8_strlen(const char* str)
{
	size_t len = 0;
	uint32_t codePoint;
	size_t index = 0;
	int ret;
	while (1)
	{
		ret = myutf8_codePointAt(&codePoint, str, index);
		if (ret > 0 && codePoint != 0)
		{
			index += ret;
			len++;
		}
		else
			break;
	}
	return len;
}

int myutf8_codePoint(uint32_t *codePoint, const char* str)
{
	int ret = -1;
	int i;
	if (!codePoint)
		return ret;
	const unsigned char* ptr = (const unsigned char*)str;
	if (0 == *ptr)
		return 0;
	int tailChars = 0;
	// check first byte of UTF8 sequence
	if ((*ptr & 0x80) == 0)					// one byte character, 0xxxxxxx
	{
		*codePoint = *ptr;
		ret = 1;
		tailChars = 0;
	}
	else if ((*ptr & 0xE0) == 0xC0)			// two bytes character, first byte - 110yyyyy
	{
		*codePoint = (0x1F & *ptr) << 6;
		ret = 1;
		tailChars = 1;
	}
	else if ((*ptr & 0xF0) == 0xE0)			// three bytes character, first byte 1110zzzz
	{
		*codePoint = (*ptr & 0x0F) << 12;
		ret = 1;
		tailChars = 2;
	}
	else if ((*ptr & 0xF8) == 0xF0)			// four bytes character, first byte 11110uuu
	{
		*codePoint = (*ptr & 0x07) << 18;
		ret = 1;
		tailChars = 3;
	}
	else									// invalid first byte
	{
		*codePoint = 0;						// interpret as string termination
		ret = -1;
	}
	// get all trailing characters
	for (i = tailChars - 1; i >= 0; i--)
	{
		ptr++;
		if (*ptr && (*ptr & 0xC0) == 0x80)
		{
			*codePoint |= (*ptr & 0x3F) << 6*i;
			ret++;
		}
		else								// ill-formed byte
		{
			*codePoint = 0;
			ret = -1;
			break;
		}
	}
	return ret;
}

int myutf8_codePointAt(uint32_t *codePoint, const char* str, size_t index)
{
	return myutf8_codePoint(codePoint, str + index);
}

int myutf8_toChars(char *dest, size_t dest_sz, uint32_t codePoint)
{
	int count = 0;
	char* dptr = dest;
	if (codePoint <= 0x007F)
	{
		if (dest_sz > 0)
		{
			*dptr = (char)codePoint;
			count = 1;
		}
	}
	else if (codePoint <= 0x07FF)
	{
		if (dest_sz > 1)
		{
			*dptr = 0xC0 | ((codePoint >> 6) & 0x1F);
			dptr++;
			*dptr = 0x80 | (codePoint & 0x003F);
			count = 2;
		}
	}
	else if (codePoint <= 0xFFFF)
	{
		if (codePoint < 0xD800 || codePoint > 0xDFFF)
		{
			if (dest_sz > 2)
			{
				*dptr = 0xE0 | ((codePoint >> 12 ) & 0x0F);
				dptr++;
				*dptr = 0x80 | ((codePoint >> 6) & 0x3F);
				dptr++;
				*dptr = 0x80 | (codePoint & 0x3F);
				count = 3;
			}
		}
	}
	else if (codePoint <= 0x001FFFFF)
	{
		if (dest_sz > 3)
		{
			*dptr = 0xF0 | ((codePoint >> 18) & 0x07);
			dptr++;
			*dptr = 0x80 | ((codePoint >> 12) & 0x3F);
			dptr++;
			*dptr = 0x80 | ((codePoint >> 6) & 0x3F);
			dptr++;
			*dptr = 0x80 | (codePoint & 0x3F);
			count = 4;
		}
	}
	return count;
}
