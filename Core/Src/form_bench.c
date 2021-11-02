// -*- coding: utf-8 -*-
/***************************************************************************
 *   Copyright (C) 2020-2021 by Chernov A.A.                               *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#include "form_bench.h"
#include "buttons.h"
#include "form_defs.h"
#include "gfx.h"
#include "fonts.h"
#include "vga_palette.h"

#include <stdio.h>

#if 0
typedef struct FormData
{
	// volatile form data
	;
} FormData;
#endif

void formBench_reset(AppState* state)
{
	//FormData* data = (FormData*)state->form_data;
}

void formBench_onDraw(AppState* state)
{
	//FormData* data = (FormData*)state->form_data;

	const int16_t base_y = 38;
	int16_t y;

	form_drawHeader(state);

	setTextDrawMode(0);

#if LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX1BPP
	uint8_t foreground_color = 1;
	uint8_t text_color_1 = 1;
	uint8_t text_color_2 = 1;
	uint8_t text_color_3 = 1;
	uint8_t text_color_4 = 1;
	uint8_t text_color_5 = 1;
	uint8_t text_color_6 = 1;
	uint8_t text_color_7 = 1;
#elif LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX4BPP
	uint8_t foreground_color = VGA16_WHITE;
	uint8_t text_color_1 = VGA16_WHITE;
	uint8_t text_color_2 = VGA16_BRIGHT_GREEN;
	uint8_t text_color_3 = VGA16_BRIGHT_RED;
	uint8_t text_color_4 = VGA16_BRIGHT_CYAN;
	uint8_t text_color_5 = VGA16_BRIGHT_BLUE;
	uint8_t text_color_6 = VGA16_WHITE;
	uint8_t text_color_7 = VGA16_BRIGHT_MAGENTA;
#elif LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX8BPP
	uint8_t foreground_color = 0x0F;
	uint8_t text_color_1 = 0x0F;
	uint8_t text_color_2 = 0x0A;
	uint8_t text_color_3 = 0x40;
	uint8_t text_color_4 = 0x35;
	uint8_t text_color_5 = 0x4E;
	uint8_t text_color_6 = 0x1F;
	uint8_t text_color_7 = 0x24;
#elif (LCD_COLOR_FORMAT == COLOR_FORMAT_RGB565) || (LCD_COLOR_FORMAT == COLOR_FORMAT_RGB888)
	RGBColor foreground_color = COLOR_WHITE;
	RGBColor text_color_1 = COLOR_WHITE;
	RGBColor text_color_2 = COLOR_GREEN;
#if RGBCOLOR_AS_NUMBER==1
	RGBColor text_color_3 = 0xFFFFA0A0;
	RGBColor text_color_4 = 0xFF00C0FF;
	RGBColor text_color_5 = 0xFF50E0FF;
	RGBColor text_color_6 = 0xFFFFFFFF;
	RGBColor text_color_7 = 0xFFFF00FF;
#else
	RGBColor text_color_3 = { 0xFF, 0xA0, 0xA0 };
	RGBColor text_color_4 = { 0x00, 0xC0, 0xFF };
	RGBColor text_color_5 = { 0x80, 0xE0, 0xFF };
	RGBColor text_color_6 = { 0xFF, 0xFF, 0xFF };
	RGBColor text_color_7 = { 0xFF, 0x00, 0xFF };
#endif
#endif	// LCD_COLOR_FORMAT

	char str_speed[40];
	sprintf(str_speed, "rend: %lums, sync: %lums", state->rendTime, state->syncTime);

	y = base_y;

#if (LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX1BPP) || (LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX4BPP) || (LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX8BPP)
	//setBackgroundIndexColor(0);
	setForegroundIndexColor(foreground_color);

	setGFXFont(&NotoSans_Regular7pt_mixed);
	setTextIndexColor(text_color_1);
	drawString(2, y, str_speed);
	y += 20;

	setGFXFont(&NotoSans_Regular7pt_mixed);
	setTextIndexColor(text_color_2);
	drawString(2, y, "Text Line 1");
	y += 13;

	setGFXFont(&NotoSans_Bold14pt_mixed);
	setTextIndexColor(text_color_3);
	drawString(2, y, "XY 101");
	y += 22;

	setGFXFont(&NotoSans_BoldItalic7pt_mixed);
	setTextIndexColor(text_color_4);
	drawString(2, y, "Text Line 3");
	y += 13;
#elif (LCD_COLOR_FORMAT == COLOR_FORMAT_RGB565) || (LCD_COLOR_FORMAT == COLOR_FORMAT_RGB888)
	//setBackgroundRGBColor(0);
	setForegroundRGBColor(foreground_color);

	setGFXFont(&NotoSans_Regular7pt_mixed);
	setTextRGBColor(text_color_1);
	drawString(2, y, str_speed);
	y += 20;

	setGFXFont(&NotoSans_Regular7pt_mixed);
	setTextRGBColor(text_color_2);
	drawString(2, y, "Text Line 1");
	y += 13;

	setGFXFont(&NotoSans_Bold14pt_mixed);
	setTextRGBColor(text_color_3);
	drawString(2, y, "XY 101");
	y += 22;

	setGFXFont(&NotoSans_BoldItalic7pt_mixed);
	setTextRGBColor(text_color_4);
	drawString(2, y, "Text Line 3");
	y += 13;
#endif
}

void formBench_onButton(AppState* state, uint8_t btn)
{
	//FormData* data = (FormData*)state->form_data;
	switch (btn)
	{
	case BUTTON_CODE_2:
		// Change form
		state->form_code = FORM_TEXT_TEST;
		state->changed |= APPSTATE_CHANGED_FORM;
		break;
	}
}
