// -*- coding: utf-8 -*-
/***************************************************************************
 *   Copyright (C) 2017-2021 by Чернов А.А.                                *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#include "gfx.h"
#include "utf8_proc.h"

#include <string.h>


// RAM mark: 0
static funcSetPixelColor s_setPixelColorFuncPtr = 0;
static const GFXfont* s_gfxFont = 0;
// RAM mark: +4 + 4 = 8
static uint8_t s_colorFormat = 0;
static uint8_t s_txtDrawMode = 0;		// 0 - transparent, 1 - opaque
// RAM mark: +2 = 10
static uint8_t s_rawfgColor[3] = { 0xFF, 0xFF, 0xFF };
static uint8_t s_rawtxtColor[3] = { 0xFF, 0xFF, 0xFF };
static uint8_t s_rawbgColor[3] = { 0x00, 0x00, 0x00 };
// RAM mark: +9 = 19

static inline int16_t _my_abs16(int16_t v)
{
	return v > 0 ? v : -v;
}

static inline void _RGBColor2Raw(uint8_t* raw_color, RGBColor src)
{
	switch (s_colorFormat)
	{
		case COLOR_FORMAT_INDEX1BPP:
		case COLOR_FORMAT_INDEX4BPP:
		case COLOR_FORMAT_INDEX8BPP:
#if RGBCOLOR_AS_NUMBER == 1
			raw_color[0] = RGB_GET_B(src);
#else
			raw_color[0] = src.b;
#endif
			break;
		case COLOR_FORMAT_RGB565:
			RGBColor2RGB565(raw_color, src);
			break;
		case COLOR_FORMAT_RGB888:
		default:
			RGBColor2RGB888(raw_color, src);
			break;
	}
}



void setFuncSetPixelColor(funcSetPixelColor funcPtr)
{
	s_setPixelColorFuncPtr = funcPtr;
}

uint8_t getColorFormat()
{
	return s_colorFormat;
}

void setColorFormat(uint8_t format)
{
	s_colorFormat = format;
	switch (s_colorFormat)
	{
		case COLOR_FORMAT_INDEX1BPP:
		case COLOR_FORMAT_INDEX4BPP:
		case COLOR_FORMAT_RGB565:
		case COLOR_FORMAT_RGB888:
			;
			break;
		default:
			s_colorFormat = COLOR_FORMAT_RGB888;
	}
}


void setForegroundRGBColor(RGBColor color)
{
	_RGBColor2Raw(s_rawfgColor, color);
}

void setBackgroundRGBColor(RGBColor color)
{
	_RGBColor2Raw(s_rawbgColor, color);
}

void setTextRGBColor(RGBColor color)
{
	_RGBColor2Raw(s_rawtxtColor, color);
}

void setForegroundIndexColor(uint8_t color)
{
	s_rawfgColor[0] = color;
}

void setBackgroundIndexColor(uint8_t color)
{
	s_rawbgColor[0] = color;
}

void setTextIndexColor(uint8_t color)
{
	s_rawtxtColor[0] = color;
}

void drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	if (!s_setPixelColorFuncPtr)
		return;
	int16_t x;
	int16_t y;
	uint8_t coordSwap = 0;
	if (_my_abs16(x2 - x1) < _my_abs16(y2 - y1))
	{
		x = x1;
		x1 = y1;
		y1 = x;
		x = x2;
		x2 = y2;
		y2 = x;
		coordSwap = 1;
	}
	if (x1 > x2)
	{
		x = x1;
		y = y1;
		x1 = x2;
		y1 = y2;
		x2 = x;
		y2 = y;
	}
	int16_t dx = x2 - x1;
	int16_t dy = _my_abs16(y2 - y1);
	int16_t err = 0;
	int8_t ystep = y2 >= y1 ? 1 : -1;
	y = y1;
	for (x = x1; x <= x2; x++)
	{
		if (!coordSwap)
			s_setPixelColorFuncPtr(x, y, s_rawfgColor);
		else
			s_setPixelColorFuncPtr(y, x, s_rawfgColor);
		err += dy;
		if (2*err > dx)		// err > dx/2
		{
			y += ystep;
			err -= dx;
		}
	}
}

void drawRect(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
	if (!s_setPixelColorFuncPtr)
		return;
	register int16_t t;
	for (t = x; t < x + w - 1; t++)
		s_setPixelColorFuncPtr(t, y, s_rawfgColor);
	for (t = y; t < y + h - 1; t++)
		s_setPixelColorFuncPtr(x + w - 1, t, s_rawfgColor);
	for (t = x + w - 1; t > x; t--)
		s_setPixelColorFuncPtr(t, y + h - 1, s_rawfgColor);
	for (t = y + 1; t < y + h; t++)
		s_setPixelColorFuncPtr(x, t, s_rawfgColor);
}

void fillRect(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
	if (!s_setPixelColorFuncPtr)
		return;
	int16_t xx;
	int16_t yy;
	for (yy = y; yy < y + h; yy++)
	{
		for (xx = x; xx < x + w; xx++)
			s_setPixelColorFuncPtr(xx, yy, s_rawbgColor);
	}
}

void drawXBitmap(int16_t x, int16_t y, uint16_t width, uint16_t height, const uint8_t* xbitmap, size_t sz)
{
	if (!s_setPixelColorFuncPtr)
		return;
	int16_t xx = x;
	int16_t yy = y;
	uint8_t c;
	const uint8_t* ptr = xbitmap;
	uint8_t exit_flag = 0;
	uint16_t pwidth = 0;
	// от младшего бита к старшему
	// от младшего байта к старшему
	for (size_t i = 0; i < sz; i++)
	{
		c = *ptr;
		for (uint8_t j = 0; j < 8; j++)
		{
			if (c & 0x01)		// black point
				s_setPixelColorFuncPtr(xx, yy, s_rawfgColor);
			else				// white point
				s_setPixelColorFuncPtr(xx, yy, s_rawbgColor);
			c >>= 1;
			xx++;
			pwidth++;
			if (pwidth >= width)
			{
				pwidth = 0;
				xx = x;
				yy++;
				if (yy >= height + y)
					exit_flag = 1;
				break;
			}
		}
		if (exit_flag)
			break;
		++ptr;
	}
}

static inline void _drawBitmap_1bpp(int16_t x, int16_t y, uint16_t width, uint16_t height, const uint8_t *bitmap, size_t sz, const uint8_t* raw_fgcolor, const uint8_t* raw_bgcolor)
{
	if (!s_setPixelColorFuncPtr)
		return;
	int16_t xx;
	int16_t yy;
	uint8_t c;
	const uint8_t* ptr = bitmap;
	uint8_t ptr_off = 7;
	// от старшего бита к младшему
	// от младшего байта к старшему
	for (yy = y; yy < y + height; yy++)
	{
		for (xx = x; xx < x + width; xx++)
		{
			//c = (*ptr & (1 << ptr_off)) >> ptr_off;
			c = (*ptr >> ptr_off) & 0x01;
			if (c != 0)
				s_setPixelColorFuncPtr(xx, yy, raw_fgcolor);
			else if (raw_bgcolor != 0)
				s_setPixelColorFuncPtr(xx, yy, raw_bgcolor);
			if (0 == ptr_off)
			{
				ptr_off = 7;
				ptr++;
				if (ptr >= bitmap + sz)
					return;
			}
			else
				ptr_off--;
		}
	}
}

void drawBitmap_1bpp(int16_t x, int16_t y, uint16_t width, uint16_t height, const uint8_t* bitmap, size_t sz)
{
	_drawBitmap_1bpp(x, y, width, height, bitmap, sz, s_rawfgColor, s_rawbgColor);
}

const GFXfont* gfxFont()
{
	return s_gfxFont;
}

void setGFXFont(const GFXfont* gfxont)
{
	s_gfxFont = gfxont;
}

void setTextDrawMode(uint8_t mode)
{
	s_txtDrawMode = mode;
}

int16_t drawChar(int16_t x, int16_t y, char ch)
{
	return drawChar32(x, y, (uint32_t)ch);
}

int16_t drawChar32(int16_t x, int16_t y, uint32_t ch)
{
	if (!s_setPixelColorFuncPtr || !s_gfxFont)
		return 0;
	const GFXglyph* glyph;
	size_t bitmap_sz;

	uint16_t idx = 0;
	for (int i = 0; i < s_gfxFont->rangesCount; i++)
	{
		if (ch >= s_gfxFont->ranges[i].first && ch <= s_gfxFont->ranges[i].last)
		{
			idx += ch - s_gfxFont->ranges[i].first;
			break;
		}
		idx += s_gfxFont->ranges[i].last - s_gfxFont->ranges[i].first + 1;
	}
	if (idx == s_gfxFont->charsCount)	// not found
		idx = 0;
	glyph = s_gfxFont->glyph + idx;
	if (glyph->width > 0 && glyph->height > 0)
	{
		if (idx == s_gfxFont->charsCount - 1)
			bitmap_sz = s_gfxFont->bitmapSize - glyph->bitmapOffset;
		else
			bitmap_sz = glyph[1].bitmapOffset - glyph->bitmapOffset;
		_drawBitmap_1bpp(x + glyph->xOffset, y + glyph->yOffset + 2*s_gfxFont->yAdvance/3, glyph->width, glyph->height, s_gfxFont->bitmap + glyph->bitmapOffset, bitmap_sz, s_rawtxtColor, s_txtDrawMode != 0 ? s_rawbgColor : 0);
	}
	return x + glyph->xAdvance;
}

int16_t drawString(int16_t x, int16_t y, const char* str)
{
	if (!s_setPixelColorFuncPtr || !s_gfxFont)
		return 0;
	const char* ptr = str;
	uint32_t ch;
	uint16_t idx = 0;
	const GFXglyph* glyph;
	size_t bitmap_sz;

	while (*ptr)
	{
		ch = (uint32_t)*ptr;
		idx = 0;
		for (int i = 0; i < s_gfxFont->rangesCount; i++)
		{
			if (ch >= s_gfxFont->ranges[i].first && ch <= s_gfxFont->ranges[i].last)
			{
				idx += ch - s_gfxFont->ranges[i].first;
				break;
			}
			idx += s_gfxFont->ranges[i].last - s_gfxFont->ranges[i].first + 1;
		}
		if (idx == s_gfxFont->charsCount)	// not found
			idx = 0;
		glyph = s_gfxFont->glyph + idx;
		if (glyph->width > 0 && glyph->height > 0)
		{
			if (idx == s_gfxFont->charsCount - 1)
				bitmap_sz = s_gfxFont->bitmapSize - glyph->bitmapOffset;
			else
				bitmap_sz = glyph[1].bitmapOffset - glyph->bitmapOffset;
			_drawBitmap_1bpp(x + glyph->xOffset, y + glyph->yOffset + 2*s_gfxFont->yAdvance/3, glyph->width, glyph->height, s_gfxFont->bitmap + glyph->bitmapOffset, bitmap_sz, s_rawtxtColor, s_txtDrawMode != 0 ? s_rawbgColor : 0);
		}
		// to next char
		ptr++;
		x += glyph->xAdvance;
	}
	return x;
}

int16_t drawStringUTF8(int16_t x, int16_t y, const char* str)
{
	if (!s_setPixelColorFuncPtr || !s_gfxFont)
		return 0;
	const char* ptr = str;
	uint32_t ch;
	int ret;
	uint16_t idx = 0;
	const GFXglyph* glyph;
	size_t bitmap_sz;

	while (*ptr)
	{
		ret = myutf8_codePoint(&ch, ptr);
		if (ret > 0)
			ptr += (unsigned int)ret;
		else
			break;
		idx = 0;
		for (int i = 0; i < s_gfxFont->rangesCount; i++)
		{
			if (ch >= s_gfxFont->ranges[i].first && ch <= s_gfxFont->ranges[i].last)
			{
				idx += ch - s_gfxFont->ranges[i].first;
				break;
			}
			idx += s_gfxFont->ranges[i].last - s_gfxFont->ranges[i].first + 1;
		}
		if (idx == s_gfxFont->charsCount)	// not found
			idx = 0;
		glyph = s_gfxFont->glyph + idx;
		if (glyph->width > 0 && glyph->height > 0)
		{
			if (idx == s_gfxFont->charsCount - 1)
				bitmap_sz = s_gfxFont->bitmapSize - glyph->bitmapOffset;
			else
				bitmap_sz = glyph[1].bitmapOffset - glyph->bitmapOffset;
			_drawBitmap_1bpp(x + glyph->xOffset, y + glyph->yOffset + 2*s_gfxFont->yAdvance/3, glyph->width, glyph->height, s_gfxFont->bitmap + glyph->bitmapOffset, bitmap_sz, s_rawtxtColor, s_txtDrawMode != 0 ? s_rawbgColor : 0);
		}
		// to next char
		x += glyph->xAdvance;
	}
	return x;
}

uint16_t charWidth(char ch)
{
	return char32Width((uint32_t)ch);
}

uint16_t char32Width(uint32_t ch)
{
	if (!s_gfxFont)
		return 0;
	uint16_t width = 0;
	const GFXglyph* glyph = 0;
	uint16_t idx = 0;

	if (ch)
	{
		for (int i = 0; i < s_gfxFont->rangesCount; i++)
		{
			if (ch >= s_gfxFont->ranges[i].first && ch <= s_gfxFont->ranges[i].last)
			{
				idx += ch - s_gfxFont->ranges[i].first;
				break;
			}
			idx += s_gfxFont->ranges[i].last - s_gfxFont->ranges[i].first + 1;
		}
		if (idx == s_gfxFont->charsCount)	// not found
			idx = 0;
		glyph = s_gfxFont->glyph + idx;
		if (glyph->width > 0 && glyph->height > 0)
			width = glyph->xAdvance;
	}
	return width;
}

uint16_t textWidth(const char* str)
{
	return textWidthLen(str, -1);
}

uint16_t textWidthLen(const char *str, int16_t len)
{
	if (!s_gfxFont)
		return 0;
	uint16_t width = 0;
	const GFXglyph* glyph;
	uint16_t idx = 0;
	const char* ptr = str;
	uint32_t ch;
	int16_t pos = 0;
	int i;
	if (len < 0)
		len = 32767;

	while (*ptr && pos < len)
	{
		ch = (uint32_t)*ptr;
		idx = 0;
		for (i = 0; i < s_gfxFont->rangesCount; i++)
		{
			if (ch >= s_gfxFont->ranges[i].first && ch <= s_gfxFont->ranges[i].last)
			{
				idx += ch - s_gfxFont->ranges[i].first;
				break;
			}
			idx += s_gfxFont->ranges[i].last - s_gfxFont->ranges[i].first + 1;
		}
		if (idx == s_gfxFont->charsCount)	// not found
			idx = 0;
		glyph = s_gfxFont->glyph + idx;
		if (glyph->width > 0 && glyph->height > 0)
			width += glyph->xAdvance;
		// to next char
		ptr++;
		pos++;
	}
	return width;
}

uint16_t textUTF8Width(const char* str)
{
	return textUTF8WidthLen(str, -1);
}

uint16_t textUTF8WidthLen(const char* str, int16_t len)
{
	if (!s_gfxFont)
		return 0;
	uint16_t width = 0;
	const GFXglyph* glyph;
	uint16_t idx = 0;
	const char* ptr = str;
	uint32_t ch;
	int ret;
	int16_t pos = 0;
	int i;
	if (len < 0)
		len = 32767;

	while (*ptr && pos < len)
	{
		ret = myutf8_codePoint(&ch, ptr);
		if (ret > 0)
		{
			ptr += (unsigned int)ret;
			pos += (unsigned int)ret;
		}
		else
			break;
		idx = 0;
		for (i = 0; i < s_gfxFont->rangesCount; i++)
		{
			if (ch >= s_gfxFont->ranges[i].first && ch <= s_gfxFont->ranges[i].last)
			{
				idx += ch - s_gfxFont->ranges[i].first;
				break;
			}
			idx += s_gfxFont->ranges[i].last - s_gfxFont->ranges[i].first + 1;
		}
		if (idx == s_gfxFont->charsCount)	// not found
			idx = 0;
		glyph = s_gfxFont->glyph + idx;
		if (glyph->width > 0 && glyph->height > 0)
			width += glyph->xAdvance;
	}
	return width;
}
