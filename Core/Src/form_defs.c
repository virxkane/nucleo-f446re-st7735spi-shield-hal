// -*- coding: utf-8 -*-
/***************************************************************************
 *   Copyright (C) 2020-2021 by Chernov A.A.                               *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#include "form_defs.h"

#include "gfx.h"
#include "fonts.h"
#include "vga_palette.h"
#include "st7735_cmds.h"

#include <string.h>
#include <stdio.h>


void form_drawHeader(AppState* state)
{
#if (LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX1BPP)
	setBackgroundIndexColor(1);
	setForegroundIndexColor(1);
	setTextIndexColor(0);
#elif (LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX4BPP) || (LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX8BPP)
	setBackgroundIndexColor(VGA16_WHITE);
	setForegroundIndexColor(VGA16_WHITE);
	setTextIndexColor(VGA16_BLACK);
#elif (LCD_COLOR_FORMAT == COLOR_FORMAT_RGB565) || (LCD_COLOR_FORMAT == COLOR_FORMAT_RGB888)
	setBackgroundRGBColor(COLOR_WHITE);
	setForegroundRGBColor(COLOR_WHITE);
	setTextRGBColor(COLOR_BLACK);
#endif

	fillRect(0, 0, 128, 38);

	setTextDrawMode(0);

	static char gfx_cf[10] = { 0 };
	static char st7735_cf[10] = { 0 };
	static char str_format[40] = { 0 };

	if (gfx_cf[0] == 0)
	{
		switch(state->gfx_color_format)
		{
		case COLOR_FORMAT_INDEX1BPP:
			strcpy(gfx_cf, "1bpp");
			break;
		case COLOR_FORMAT_INDEX4BPP:
			strcpy(gfx_cf, "4bpp");
			break;
		case COLOR_FORMAT_INDEX8BPP:
			strcpy(gfx_cf, "8bpp");
			break;
		case COLOR_FORMAT_RGB565:
			strcpy(gfx_cf, "16bpp");
			break;
		case COLOR_FORMAT_RGB888:
			strcpy(gfx_cf, "24bpp");
			break;
		default:
			strcpy(gfx_cf, "unk");
			break;
		}
	}
	if (st7735_cf[0] == 0)
	{
		switch(state->st7735_ipf)
		{
		case ST7735_IPF_12BIT:
			strcpy(st7735_cf, "12bit");
			break;
		case ST7735_IPF_16BIT:
			strcpy(st7735_cf, "16bit");
			break;
		case ST7735_IPF_18BIT:
			strcpy(st7735_cf, "18bit");
			break;
		default:
			strcpy(st7735_cf, "unk");
			break;
		}
	}
	if (str_format[0] == 0)
	{
		sprintf(str_format, "FB:%s; LCD:%s", gfx_cf, st7735_cf);
	}

	setGFXFont(&NotoSans_Regular7pt_mixed);
	drawString(3,  0, "ST7735 shield Demo.");
	drawString(3, 12, "Hardware SPI, HAL.");
	drawString(3, 24, str_format);
}
