// -*- coding: utf-8 -*-
/***************************************************************************
 *   Copyright (C) 2020-2021 by Chernov A.A.                               *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef IMAGE_WB_TEST_FORM_H
#define IMAGE_WB_TEST_FORM_H

#include "app_state.h"

#ifdef __cplusplus
extern "C" {
#endif

void formTestImageWB_reset(AppState*);
void formTestImageWB_onDraw(AppState*);
void formTestImageWB_onButton(AppState*, uint8_t);

#ifdef __cplusplus
}
#endif

#endif // IMAGE_WB_TEST_FORM_H
