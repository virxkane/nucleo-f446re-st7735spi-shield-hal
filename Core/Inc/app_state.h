// -*- coding: utf-8 -*-
/***************************************************************************
 *   Copyright (C) 2020-2021 by Чернов А.А.                                *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef APP_STATE_H
#define APP_STATE_H

#include "rgbcolor.h"

#include <stdint.h>

//#define FORM_DATA_MAX_SZ			20
#define FORM_DATA_MAX_SZ			32

typedef struct AppState
{
	uint32_t devID;					// device ID
	uint8_t gfx_color_format;
	uint8_t st7735_ipf;

	uint8_t form_code;				// current form code
	//int8_t cursor_pos;				// current cursor position in current form
	uint8_t changed;				// bit field mask of changed state, for example, config, form, etc.

    uint32_t rendTime;				// current form render time, ms
    uint32_t syncTime;				// LCD buffer sync time, ms

	uint8_t* form_data;				// pointer to current form data
} AppState;

// Bitfield masks for field `changed'
#define APPSTATE_CHANGED_NEEDREDRAW		0x01
#define APPSTATE_CHANGED_FORM			0x02

//#define FORM_INVALIDATE(state)		{ state->changed |= APPSTATE_CHANGED_NEEDREDRAW; }

// LCD color format, one of:
//   COLOR_FORMAT_INDEX1BPP
//   COLOR_FORMAT_INDEX4BPP
//   COLOR_FORMAT_INDEX8BPP
//   COLOR_FORMAT_RGB565
//   COLOR_FORMAT_RGB888
#define LCD_COLOR_FORMAT		COLOR_FORMAT_RGB565
//#define LCD_COLOR_FORMAT		COLOR_FORMAT_INDEX8BPP

#endif // APP_STATE_H
