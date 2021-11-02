// -*- coding: utf-8 -*-
/***************************************************************************
 *   Copyright (C) 2020-2021 by Chernov A.A.                               *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef FORM_TEST_TEXT_H
#define FORM_TEST_TEXT_H

#include "app_state.h"

#ifdef __cplusplus
extern "C" {
#endif

void formTestText_reset(AppState*);
void formTestText_onDraw(AppState*);
void formTestText_onButton(AppState*, uint8_t);

#ifdef __cplusplus
}
#endif

#endif // FORM_TEST_TEXT_H
