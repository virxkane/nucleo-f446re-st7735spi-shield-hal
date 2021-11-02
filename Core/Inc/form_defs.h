// -*- coding: utf-8 -*-
/***************************************************************************
 *   Copyright (C) 2020-2021 by Chernov A.A.                               *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef FORM_DEFS_H
#define FORM_DEFS_H

#include "app_state.h"

// display forms
#define FORM_TEXT_TEST			0		// Font drawing test screen
#define FORM_TEXT_RU_TEST		1		// Font drawing test screen
#define FORM_WB_IMAGE_TEST		2		// Black and white images output test screen
#define FORM_BENCH				3		// Graphics performance test screen
#define FORM_MAX				FORM_BENCH

typedef struct form_def
{
	void (*reset_proc)(AppState*);
	void (*onDraw_proc)(AppState*);
	void (*onButton_proc)(AppState*, uint8_t btn);
} form_def;

#ifdef __cplusplus
extern "C" {
#endif

void form_drawHeader(AppState*);

#ifdef __cplusplus
}
#endif

#endif // FORM_DEFS_H
