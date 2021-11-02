// -*- coding: utf-8 -*-
/***************************************************************************
 *   Copyright (C) 2020-2021 by Chernov A.A.                               *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef FORM_TEST_TEXT_RU_H
#define FORM_TEST_TEXT_RU_H

#include "app_state.h"

#ifdef __cplusplus
extern "C" {
#endif

void formTestTextRU_reset(AppState*);
void formTestTextRU_onDraw(AppState*);
void formTestTextRU_onButton(AppState*, uint8_t);

#ifdef __cplusplus
}
#endif

#endif // FORM_TEST_TEXT_RU_H
