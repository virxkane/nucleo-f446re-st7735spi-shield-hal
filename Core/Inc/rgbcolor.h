// -*- coding: utf-8 -*-
/***************************************************************************
 *   Copyright (C) 2020-2021 by Chernov A.A.                               *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef RGBCOLOR_H
#define RGBCOLOR_H

#define RGBCOLOR_AS_NUMBER		0

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if RGBCOLOR_AS_NUMBER==1
// RGB: AARRGGBB
typedef uint32_t RGBColor;

#define RGB_SET_R(rgb, r) { rgb = (rgb & 0xFF00FFFF) | ((r & 0xFF) << 16); }
#define RGB_SET_G(rgb, g) { rgb = (rgb & 0xFFFF00FF) | ((g & 0xFF) << 8); }
#define RGB_SET_B(rgb, b) { rgb = (rgb & 0xFFFFFF00) | (b & 0xFF); }
#define RGB_SET_A(rgb, a) { rgb = (rgb & 0x00FFFFFF) | ((a & 0xFF) << 24); }

#define RGB_GET_R(rgb)    ( (rgb >> 16) & 0xFF )
#define RGB_GET_G(rgb)    ( (rgb >> 8) & 0xFF )
#define RGB_GET_B(rgb)    ( rgb & 0xFF )
#define RGB_GET_A(rgb)    ( rgb >> 24 )

#else

typedef struct RGBColor
{
	uint8_t r;		// red component
	uint8_t g;		// green component
	uint8_t b;		// blue component or index color value
} RGBColor;

#endif

#define COLOR_FORMAT_INDEX1BPP	0x11
#define COLOR_FORMAT_INDEX4BPP	0x14
#define COLOR_FORMAT_INDEX8BPP	0x18
#define COLOR_FORMAT_RGB565		0x56
#define COLOR_FORMAT_RGB888		0x88

void RGBColor2RGB565(uint8_t* dst, RGBColor src);
void RGBColor2RGB888(uint8_t* dst, RGBColor src);

extern const RGBColor COLOR_BLACK;
extern const RGBColor COLOR_WHITE;
extern const RGBColor COLOR_RED;
extern const RGBColor COLOR_GREEN;
extern const RGBColor COLOR_BLUE;
extern const RGBColor COLOR_GRAY;
//extern const RGBColor COLOR_TRANSP;		// special value for transparent color


#ifdef __cplusplus
}
#endif

#endif	// RGBCOLOR_H
