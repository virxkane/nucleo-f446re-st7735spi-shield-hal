// -*- coding: utf-8 -*-
/***************************************************************************
 *   Copyright (C) 2020-2021 by Chernov A.A.                               *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#include "rgbcolor.h"

#if RGBCOLOR_AS_NUMBER==1

const RGBColor COLOR_BLACK  = 0xFF000000;
const RGBColor COLOR_WHITE  = 0xFFFFFFFF;
const RGBColor COLOR_RED    = 0xFFFF0000;
const RGBColor COLOR_GREEN  = 0xFF00FF00;
const RGBColor COLOR_BLUE   = 0xFF0000FF;
const RGBColor COLOR_GRAY   = 0xFF808080;
const RGBColor COLOR_TRANSP = 0x00000000;

#else

const RGBColor COLOR_BLACK = { 0x00, 0x00, 0x00 };
const RGBColor COLOR_WHITE = { 0xFF, 0xFF, 0xFF };
const RGBColor COLOR_RED =   { 0xFF, 0x00, 0x00 };
const RGBColor COLOR_GREEN = { 0x00, 0xFF, 0x00 };
const RGBColor COLOR_BLUE =  { 0x00, 0x00, 0xFF };
const RGBColor COLOR_GRAY =  { 0x80, 0x80, 0x80 };

#endif

void RGBColor2RGB565(uint8_t* dst, RGBColor src)
{
	// |RRRRRGGG|GGGBBBBB|
#if RGBCOLOR_AS_NUMBER==1
	dst[0] = (RGB_GET_R(src) & 0xF8) |
			 (RGB_GET_G(src) >> 5);
	dst[1] = ((RGB_GET_G(src) & 0x1C) << 3) |
			  (RGB_GET_B(src) >> 3);
#else
	dst[0] = (src.r & 0xF8) |
			 (src.g >> 5);
	dst[1] = ((src.g & 0x1C) << 3) |
			  (src.b >> 3);
#endif
}

void RGBColor2RGB888(uint8_t* dst, RGBColor src)
{
	// RGB666: |RRRRRRXX|GGGGGGXX|BBBBBBXX|
	// RGB888: |RRRRRRRR|GGGGGGGG|BBBBBBBB|
#if RGBCOLOR_AS_NUMBER==1
	dst[0] = (RGB_GET_R(src) & 0xFF);
	dst[1] = (RGB_GET_G(src) & 0xFF);
	dst[2] = (RGB_GET_B(src) & 0xFF);
#else
	dst[0] = src.r;
	dst[1] = src.g;
	dst[2] = src.b;
#endif
}
