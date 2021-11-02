// -*- coding: utf-8 -*-
/***************************************************************************
 *   Copyright (C) 2020-2021 by Chernov A.A.                               *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef BENCH_FORM_H
#define BENCH_FORM_H

#include "app_state.h"

#ifdef __cplusplus
extern "C" {
#endif

void formBench_reset(AppState*);
void formBench_onDraw(AppState*);
void formBench_onButton(AppState*, uint8_t);

#ifdef __cplusplus
}
#endif

#endif // BENCH_FORM_H
