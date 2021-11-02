// -*- coding: utf-8 -*-
/***************************************************************************
 *   Copyright (C) 2017-2021 by Chernov A.A.                               *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef FB_GFX_H
#define FB_GFX_H

#include <stdint.h>
#include <string.h>		// for size_t

#include "gfxfont.h"
#include "rgbcolor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int8_t (*funcSetPixelColor)(int16_t x, int16_t y, const uint8_t* raw_color);

void setFuncSetPixelColor(funcSetPixelColor funcPtr);

uint8_t getColorFormat();
void setColorFormat(uint8_t format);

// For RGB colors
void setForegroundRGBColor(RGBColor color);
void setBackgroundRGBColor(RGBColor color);
void setTextRGBColor(RGBColor color);

// For indexed color palettes
void setForegroundIndexColor(uint8_t color);
void setBackgroundIndexColor(uint8_t color);
void setTextIndexColor(uint8_t color);

void drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void drawRect(int16_t x, int16_t y, uint16_t w, uint16_t h);
void fillRect(int16_t x, int16_t y, uint16_t w, uint16_t h);

/**
 * @brief Draw X BitMap.
 * @param x
 * @param y
 * @param width
 * @param height
 * @param bitmap - bitmap's data, one bit per pixel.
 * @param sz - size of bitmap data.
 *
 * X Bitmap is a black and white image, @see https://en.wikipedia.org/wiki/X_BitMap.
 * As black color uses foreground color - @see setForegroundRGBColor(), setForegroundIndexColor(),
 * as white color - background color - @see setBackgroundRGBColor(), setBackgroundIndexColor().
 */
void drawXBitmap(int16_t x, int16_t y, uint16_t width, uint16_t height, const uint8_t* xbitmap, size_t sz);

/**
 * @brief Draw black and white (1bpp) bitmap.
 * @param x
 * @param y
 * @param width
 * @param height
 * @param bitmap - bitmap's data, one bit per pixel.
 * @param sz - size of bitmap data.
 */
void drawBitmap_1bpp(int16_t x, int16_t y, uint16_t width, uint16_t height, const uint8_t* bitmap, size_t sz);

/**
 * @brieft Set text drawing mode: how paint pixels outside of character glyph.
 * @param mode values 0 mean transparent mode - pixels outside glyph not changed.
 *             Any other values mean opaque mode - pixels outside glyph painted over.
 */
void setTextDrawMode(uint8_t mode);

const GFXfont* gfxFont();
void setGFXFont(const GFXfont* gfxont);

int16_t drawChar(int16_t x, int16_t y, char c);
int16_t drawChar32(int16_t x, int16_t y, uint32_t c);

int16_t drawString(int16_t x, int16_t y, const char* str);
int16_t drawStringUTF8(int16_t x, int16_t y, const char* str);

uint16_t charWidth(char ch);
uint16_t char32Width(uint32_t ch);
uint16_t textWidth(const char* str);
uint16_t textWidthLen(const char* str, int16_t len);
uint16_t textUTF8Width(const char* str);
uint16_t textUTF8WidthLen(const char* str, int16_t len);

#ifdef __cplusplus
}
#endif

#endif // FB_GFX_H
