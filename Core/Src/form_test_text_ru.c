// -*- coding: utf-8 -*-
/***************************************************************************
 *   Copyright (C) 2020-2021 by Chernov A.A.                               *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#include "form_test_text_ru.h"
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

void formTestTextRU_reset(AppState* state)
{
	//FormData* data = (FormData*)state->form_data;
}

void formTestTextRU_onDraw(AppState* state)
{
	const int16_t base_y = 38;
	int16_t y;

	form_drawHeader(state);

	//FormData* data = (FormData*)state->form_data;

	// test text drawing functions with Russian text.

	setTextDrawMode(0);

	y = base_y;

#if (LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX1BPP)
	//setBackgroundIndexColor(0);
	setForegroundIndexColor(1);
	setTextIndexColor(1);
#elif (LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX4BPP)
	//setBackgroundIndexColor(0);
	setForegroundIndexColor(VGA16_WHITE);
	setTextIndexColor(VGA16_WHITE);
#elif (LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX8BPP)
	//setBackgroundIndexColor(0);
	setForegroundIndexColor(15);
	setTextIndexColor(15);
#elif (LCD_COLOR_FORMAT == COLOR_FORMAT_RGB565) || (LCD_COLOR_FORMAT == COLOR_FORMAT_RGB888)
	//setBackgroundColor(COLOR_BLACK);
	setForegroundRGBColor(COLOR_WHITE);
	setTextRGBColor(COLOR_WHITE);
#endif

	setGFXFont(&NotoSans_Regular7pt_mixed);

	drawStringUTF8(2, y, "Текст с буквами ё и Ё.");
	y += NotoSans_Regular7pt_mixed.yAdvance + 2;

	drawStringUTF8(2, y, "Съешь ещё этих мяг-");
	y += NotoSans_Regular7pt_mixed.yAdvance - 2;

	drawStringUTF8(2, y, "ких французских бу-");
	y += NotoSans_Regular7pt_mixed.yAdvance - 2;

	drawStringUTF8(2, y, "лочек!");
	y += NotoSans_Regular7pt_mixed.yAdvance + 2;

	// TODO:
	setGFXFont(&NotoSans_Regular14pt_mixed);
	drawStringUTF8(2, y, "Привет,");
	y += NotoSans_Regular14pt_mixed.yAdvance - 9;
	drawStringUTF8(2, y, "мир!");
	y += NotoSans_Regular14pt_mixed.yAdvance - 9;
}

void formTestTextRU_onButton(AppState* state, uint8_t btn)
{
	//FormData* data = (FormData*)state->form_data;
	switch (btn)
	{
	case BUTTON_CODE_2:
		// Change form
		state->form_code = FORM_WB_IMAGE_TEST;
		state->changed |= APPSTATE_CHANGED_FORM;
		break;
	}
}
