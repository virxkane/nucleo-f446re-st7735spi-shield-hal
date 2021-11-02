/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "st7735_halspi.h"
#include "st7735_cmds.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "gfx.h"

#include "app_state.h"
#include "form_defs.h"

#include "form_test_text.h"
#include "form_test_text_ru.h"
#include "form_test_image_wb.h"
#include "form_bench.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

struct op_status
{
	volatile uint8_t button_code;
	volatile uint8_t status;
};

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#if LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX1BPP
#define LCD1_BUFFER_SZ		2673			// 132x162 @ 1bpp
#elif LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX4BPP
#define LCD1_BUFFER_SZ		10692			// 132x162 @ 4bpp
#elif LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX8BPP
#define LCD1_BUFFER_SZ		21384			// 132x162 @ 8bpp
#elif LCD_COLOR_FORMAT == COLOR_FORMAT_RGB565
#define LCD1_BUFFER_SZ		42768			// 132x162 @ 16bpp
#elif LCD_COLOR_FORMAT == COLOR_FORMAT_RGB888
#define LCD1_BUFFER_SZ		64152			// 132x162 @ 24bpp
#else
#error Invalid color format value!
#endif

#define STATUS_BTN_RDY_Pos			0							// button ready flag
#define STATUS_BTN_RDY_Msk			(1 << STATUS_BTN_RDY_Pos)
#define STATUS_BTN_CPT_BT1_Pos		1							// BT1 capture flag
#define STATUS_BTN_CPT_BT1_Msk		(1 << STATUS_BTN_CPT_BT1_Pos)
#define STATUS_BTN_CPT_BT2_Pos		2							// BT2 capture flag
#define STATUS_BTN_CPT_BT2_Msk		(1 << STATUS_BTN_CPT_BT2_Pos)
#define STATUS_BTN_CPT_BT3_Pos		3							// BT3 capture flag
#define STATUS_BTN_CPT_BT3_Msk		(1 << STATUS_BTN_CPT_BT3_Pos)
#define STATUS_BTN_CPT_Msk			(STATUS_BTN_CPT_BT1_Msk | STATUS_BTN_CPT_BT2_Msk | STATUS_BTN_CPT_BT3_Msk)
#define STATUS_BTN_PRE_Pos			4							// button pressed flag
#define STATUS_BTN_PRE_Msk			(1 << STATUS_BTN_PRE_Pos)
#define STATUS_BTN_REP_Pos			5							// button repeat flag
#define STATUS_BTN_REP_Msk			(1 << STATUS_BTN_REP_Pos)
//#define STATUS_RTC_TO_Pos			7							// RTC timeout flag
//#define STATUS_RTC_TO_Msk			(1 << STATUS_RTC_TO_Pos)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */

#if RGBCOLOR_AS_NUMBER==1

#if LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX1BPP

static const RGBColor lcd1_1bpp_palette[2] = 	// 1bpp palette for LCD
{
	0xFF000000,			// 0 - black
	0xFFFFFFFF,			// 1 - white
};

#elif LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX4BPP

static const RGBColor lcd1_4bpp_palette[16] = 	// 4bpp palette for LCD
{
	// Standard VGA 16 color palette
	0xFF000000,			// 0  - black
	0xFF0000AA,			// 1  - blue
	0xFF00AA00,			// 2  - green
	0xFF00AAAA,			// 3  - cyan
	0xFFAA0000,			// 4  - red
	0xFFAA00AA,			// 5  - magenta
	0xFFAA5500,			// 6  - brown
	0xFFAAAAAA,			// 7  - gray
	0xFF555555,			// 8  - dark gray
	0xFF5555FF,			// 9  - bright blue
	0xFF55FF55,			// 10 - bright green
	0xFF55FFFF,			// 11 - bright cyan
	0xFFFF5555,			// 12 - bright red
	0xFFFF55FF,			// 13 - bright magenta
	0xFFFFFF55,			// 14 - yellow
	0xFFFFFFFF,			// 15 - white
};		// 64 bytes

#elif LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX8BPP

static const RGBColor lcd1_8bpp_palette[256] = 	// 8bpp palette for LCD
{
	// Standard VGA 256 color palette
	// row 0 (R)
	0xFF000000,	// 0  - black
	0xFF0000AA,	// 1  - blue
	0xFF00AA00,	// 2  - green
	0xFF00AAAA,	// 3  - cyan
	0xFFAA0000,	// 4  - red
	0xFFAA00AA,	// 5  - magenta
	0xFFAA5500,	// 6  - brown
	0xFFAAAAAA,	// 7  - gray
	0xFF555555,	// 8  - dark gray
	0xFF5555FF,	// 9  - bright blue
	0xFF55FF55,	// 10 - bright green
	0xFF55FFFF,	// 11 - bright cyan
	0xFFFF5555,	// 12 - bright red
	0xFFFF55FF,	// 13 - bright magenta
	0xFFFFFF55,	// 14 - yellow
	0xFFFFFFFF,	// 15 - white
	// row 1, gray
	0xFF000000,	// 0, 16
	0xFF141414,	// 1, 17
	0xFF202020,	// 2, 18
	0xFF303030,	// 3, 19
	0xFF383838,	// 4, 20
	0xFF444444,	// 5, 21
	0xFF505050,	// 6, 22
	0xFF606060,	// 7, 23
	0xFF707070,	// 8, 24
	0xFF808080,	// 9, 25
	0xFF909090,	// 10,26
	0xFFA0A0A0,	// 11,27
	0xFFB4B4B4,	// 12,28
	0xFFC8C8C8,	// 13,29
	0xFFE0E0E0,	// 14,30
	0xFFFFFFFF,	// 15,31
	// row 2 (R)
	0xFF0000FF,	// 0, 32
	0xFF4000FF,	// 1, 33
	0xFF8000FF,	// 2, 34
	0xFFC000FF,	// 3, 35
	0xFFFF00FF,	// 4, 36
	0xFFFF00C0,	// 5, 37
	0xFFFF0080,	// 6, 38
	0xFFFF0040,	// 7, 39
	0xFFFF0000,	// 8, 40
	0xFFFF4000,	// 9, 41
	0xFFFF8000,	// 10,42
	0xFFFFC000,	// 11,43
	0xFFFFFF00,	// 12,44
	0xFFC0FF00,	// 13,45
	0xFF80FF00,	// 14,46
	0xFF40FF00,	// 15,47
	// row 3 (R)
	0xFF00FF00,	// 0, 48
	0xFF00FF40,	// 1, 49
	0xFF00FF80,	// 2, 50
	0xFF00FFC0,	// 3, 51
	0xFF00FFFF,	// 4, 52
	0xFF00C0FF,	// 5, 53
	0xFF0080FF,	// 6, 54
	0xFF0040FF,	// 7, 55
	0xFF8080FF,	// 8, 56
	0xFFA080FF,	// 9, 57
	0xFFC080FF,	// 10,58
	0xFFE080FF,	// 11,59
	0xFFFF80FF,	// 12,60
	0xFFFF80E0,	// 13,61
	0xFFFF80C0,	// 14,62
	0xFFFF80A0,	// 15,63
	// row 4 (R)
	0xFFFF8080,	// 0, 64
	0xFFFFA080,	// 1, 65
	0xFFFFC080,	// 2, 66
	0xFFFFE080,	// 3, 67
	0xFFFFFF80,	// 4, 68
	0xFFE0FF80,	// 5, 69
	0xFFC0FF80,	// 6, 70
	0xFFA0FF80,	// 7, 71
	0xFF80FF80,	// 8, 72
	0xFF80FFA0,	// 9, 73
	0xFF80FFC0,	// 10,74
	0xFF80FFE0,	// 11,75
	0xFF80FFFF,	// 12,76
	0xFF80E0FF,	// 13,77
	0xFF80C0FF,	// 14,78
	0xFF80A0FF,	// 15,79
	// row 5 (R)
	0xFFB4B4FF,	// 0, 80
	0xFFC4B4FF,	// 1, 81
	0xFFD8B4FF,	// 2, 82
	0xFFE8B4FF,	// 3, 83
	0xFFFFB4FF,	// 4, 84
	0xFFFFB4E8,	// 5, 85
	0xFFFFB4D8,	// 6, 86
	0xFFFFB4C4,	// 7, 87
	0xFFFFB4B4,	// 8, 88
	0xFFFFC4B4,	// 9, 89
	0xFFFFD8B4,	// 10,90
	0xFFFFE8B4,	// 11,91
	0xFFFFFFB4,	// 12,92
	0xFFE8FFB4,	// 13,93
	0xFFD8FFB4,	// 14,94
	0xFFC4FFB4,	// 15,95
	// row 6 (R)
	0xFFB4FFB4,	// 0, 96
	0xFFB4FFC4,	// 1, 97
	0xFFB4FFD8,	// 2, 98
	0xFFB4FFE8,	// 3, 99
	0xFFB4FFFF,	// 4, 100
	0xFFB4E8FF,	// 5, 101
	0xFFB4D8FF,	// 6, 102
	0xFFB4C4FF,	// 7, 103
	0xFF000070,	// 8, 104
	0xFF200070,	// 9, 105
	0xFF380070,	// 10,106
	0xFF540070,	// 11,107
	0xFF700070,	// 12,108
	0xFF700054,	// 13,109
	0xFF700038,	// 14,110
	0xFF700020,	// 15,111
	// row 7 (R)
	0xFF700000,	// 0, 112
	0xFF702000,	// 1, 113
	0xFF703800,	// 2, 114
	0xFF705400,	// 3, 115
	0xFF707000,	// 4, 116
	0xFF547000,	// 5, 117
	0xFF387000,	// 6, 118
	0xFF207000,	// 7, 119
	0xFF007000,	// 8, 120
	0xFF007020,	// 9, 121
	0xFF007038,	// 10,122
	0xFF007054,	// 11,123
	0xFF007070,	// 12,124
	0xFF005470,	// 13,125
	0xFF003870,	// 14,126
	0xFF002070,	// 15,127
	// row 8 (R)
	0xFF383870,	// 0, 128
	0xFF443870,	// 1, 129
	0xFF543870,	// 2, 130
	0xFF603870,	// 3, 131
	0xFF703870,	// 4, 132
	0xFF703860,	// 5, 133
	0xFF703854,	// 6, 134
	0xFF703844,	// 7, 135
	0xFF703838,	// 8, 136
	0xFF704438,	// 9, 137
	0xFF705438,	// 10,138
	0xFF706038,	// 11,139
	0xFF707038,	// 12,140
	0xFF607038,	// 13,141
	0xFF547038,	// 14,142
	0xFF447038,	// 15,143
	// row 9 (R)
	0xFF387038,	// 0, 144
	0xFF387044,	// 1, 145
	0xFF387054,	// 2, 145
	0xFF387060,	// 3, 147
	0xFF387070,	// 4, 148
	0xFF386070,	// 5, 149
	0xFF385470,	// 6, 150
	0xFF384470,	// 7, 151
	0xFF505070,	// 8, 152
	0xFF585070,	// 9, 153
	0xFF605070,	// 10,154
	0xFF685070,	// 11,155
	0xFF705070,	// 12,156
	0xFF705068,	// 13,157
	0xFF705060,	// 14,158
	0xFF705058,	// 15,159
	// row 10 (R)
	0xFF705050,	// 0, 160
	0xFF705850,	// 1, 161
	0xFF706050,	// 2, 162
	0xFF706850,	// 3, 163
	0xFF707050,	// 4, 164
	0xFF687050,	// 5, 165
	0xFF607050,	// 6, 166
	0xFF587050,	// 7, 167
	0xFF507050,	// 8, 168
	0xFF507058,	// 9, 169
	0xFF507060,	// 10,170
	0xFF507068,	// 11,171
	0xFF507070,	// 12,172
	0xFF506870,	// 13,173
	0xFF506070,	// 14,174
	0xFF505870,	// 15,175
	// row 11 (R)
	0xFF000040,	// 0, 176
	0xFF100040,	// 1, 177
	0xFF200040,	// 2, 178
	0xFF300040,	// 3, 179
	0xFF400040,	// 4, 180
	0xFF400030,	// 5, 181
	0xFF400020,	// 6, 182
	0xFF400010,	// 7, 183
	0xFF400000,	// 8, 184
	0xFF401000,	// 9, 185
	0xFF402000,	// 10,186
	0xFF403000,	// 11,187
	0xFF404000,	// 12,188
	0xFF304000,	// 13,189
	0xFF204000,	// 14,190
	0xFF104000,	// 15,191
	// row 12 (R)
	0xFF004000,	// 0, 192
	0xFF004010,	// 1, 193
	0xFF004020,	// 2, 194
	0xFF004030,	// 3, 195
	0xFF004040,	// 4, 196
	0xFF003040,	// 5, 197
	0xFF002040,	// 6, 198
	0xFF001040,	// 7, 199
	0xFF202040,	// 8, 200
	0xFF282040,	// 9, 201
	0xFF302040,	// 10,202
	0xFF382040,	// 11,203
	0xFF402040,	// 12,204
	0xFF402038,	// 13,205
	0xFF402030,	// 14,206
	0xFF402028,	// 15,207
	// row 13 (R)
	0xFF402020,	// 0, 208
	0xFF402820,	// 1, 209
	0xFF403020,	// 2, 210
	0xFF403820,	// 3, 211
	0xFF404020,	// 4, 212
	0xFF384020,	// 5, 213
	0xFF304020,	// 6, 214
	0xFF284020,	// 7, 215
	0xFF204020,	// 8, 216
	0xFF204028,	// 9, 217
	0xFF204030,	// 10,218
	0xFF204038,	// 11,219
	0xFF204040,	// 12,220
	0xFF203840,	// 13,221
	0xFF203040,	// 14,222
	0xFF202840,	// 15,223
	// row 14 (R)
	0xFF2C2C40,	// 0, 224
	0xFF302C40,	// 1, 225
	0xFF342C40,	// 2, 226
	0xFF3C2C40,	// 3, 227
	0xFF402C40,	// 4, 228
	0xFF402C3C,	// 5, 229
	0xFF402C34,	// 6, 230
	0xFF402C30,	// 7, 231
	0xFF402C2C,	// 8, 232
	0xFF40302C,	// 9, 233
	0xFF40342C,	// 10,234
	0xFF403C2C,	// 11,235
	0xFF40402C,	// 12,236
	0xFF3C402C,	// 13,237
	0xFF34402C,	// 14,238
	0xFF30402C,	// 15,239
	// row 15 (R)
	0xFF2C402C,	// 0, 240
	0xFF2C4030,	// 1, 241
	0xFF2C4034,	// 2, 242
	0xFF2C403C,	// 3, 243
	0xFF2C4040,	// 4, 244
	0xFF2C3C40,	// 5, 245
	0xFF2C3440,	// 6, 246
	0xFF000000,	// 7, 247
	0xFF000000,	// 8, 248 - black
	0xFF000000,	// 9, 249 - black
	0xFF000000,	// 10,250 - black
	0xFF000000,	// 11,251 - black
	0xFF000000,	// 12,252 - black
	0xFF000000,	// 13,253 - black
	0xFF000000,	// 14,254 - black
	0xFF000000,	// 15,255 - black
};

#endif	// LCD_COLOR_FORMAT

#else	// RGBCOLOR_AS_NUMBER==1

#if LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX1BPP

static const RGBColor lcd1_1bpp_palette[2] = 	// 1bpp palette for LCD
{
	{ 0x00, 0x00, 0x00 },	// 0  - black
	{ 0xFF, 0xFF, 0xFF },	// 1  - white
};		// 6 bytes

#elif LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX4BPP

static const RGBColor lcd1_4bpp_palette[16] = 	// 4bpp palette for LCD
{
	// Standard VGA 16 color palette
	{ 0x00, 0x00, 0x00 },	// 0  - black
	{ 0x00, 0x00, 0xAA },	// 1  - blue
	{ 0x00, 0xAA, 0x00 },	// 2  - green
	{ 0x00, 0xAA, 0xAA },	// 3  - cyan
	{ 0xAA, 0x00, 0x00 },	// 4  - red
	{ 0xAA, 0x00, 0xAA },	// 5  - magenta
	{ 0xAA, 0x55, 0x00 },	// 6  - brown
	{ 0xAA, 0xAA, 0xAA },	// 7  - gray
	{ 0x55, 0x55, 0x55 },	// 8  - dark gray
	{ 0x55, 0x55, 0xFF },	// 9  - bright blue
	{ 0x55, 0xFF, 0x55 },	// 10 - bright green
	{ 0x55, 0xFF, 0xFF },	// 11 - bright cyan
	{ 0xFF, 0x55, 0x55 },	// 12 - bright red
	{ 0xFF, 0x55, 0xFF },	// 13 - bright magenta
	{ 0xFF, 0xFF, 0x55 },	// 14 - yellow
	{ 0xFF, 0xFF, 0xFF },	// 15 - white
};		// 48 bytes

#elif LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX8BPP

static const RGBColor lcd1_8bpp_palette[256] = 	// 8bpp palette for LCD
{
	// Standard VGA 256 color palette
	// row 0 (R)
	{ 0x00, 0x00, 0x00 },	// 0  - black
	{ 0x00, 0x00, 0xAA },	// 1  - blue
	{ 0x00, 0xAA, 0x00 },	// 2  - green
	{ 0x00, 0xAA, 0xAA },	// 3  - cyan
	{ 0xAA, 0x00, 0x00 },	// 4  - red
	{ 0xAA, 0x00, 0xAA },	// 5  - magenta
	{ 0xAA, 0x55, 0x00 },	// 6  - brown
	{ 0xAA, 0xAA, 0xAA },	// 7  - gray
	{ 0x55, 0x55, 0x55 },	// 8  - dark gray
	{ 0x55, 0x55, 0xFF },	// 9  - bright blue
	{ 0x55, 0xFF, 0x55 },	// 10 - bright green
	{ 0x55, 0xFF, 0xFF },	// 11 - bright cyan
	{ 0xFF, 0x55, 0x55 },	// 12 - bright red
	{ 0xFF, 0x55, 0xFF },	// 13 - bright magenta
	{ 0xFF, 0xFF, 0x55 },	// 14 - yellow
	{ 0xFF, 0xFF, 0xFF },	// 15 - white
	// row 1, gray
	{ 0x00, 0x00, 0x00 },	// 0, 16
	{ 0x14, 0x14, 0x14 },	// 1, 17
	{ 0x20, 0x20, 0x20 },	// 2, 18
	{ 0x30, 0x30, 0x30 },	// 3, 19
	{ 0x38, 0x38, 0x38 },	// 4, 20
	{ 0x44, 0x44, 0x44 },	// 5, 21
	{ 0x50, 0x50, 0x50 },	// 6, 22
	{ 0x60, 0x60, 0x60 },	// 7, 23
	{ 0x70, 0x70, 0x70 },	// 8, 24
	{ 0x80, 0x80, 0x80 },	// 9, 25
	{ 0x90, 0x90, 0x90 },	// 10,26
	{ 0xA0, 0xA0, 0xA0 },	// 11,27
	{ 0xB4, 0xB4, 0xB4 },	// 12,28
	{ 0xC8, 0xC8, 0xC8 },	// 13,29
	{ 0xE0, 0xE0, 0xE0 },	// 14,30
	{ 0xFF, 0xFF, 0xFF },	// 15,31
	// row 2 (R)
	{ 0x00, 0x00, 0xFF },	// 0, 32
	{ 0x40, 0x00, 0xFF },	// 1, 33
	{ 0x80, 0x00, 0xFF },	// 2, 34
	{ 0xC0, 0x00, 0xFF },	// 3, 35
	{ 0xFF, 0x00, 0xFF },	// 4, 36
	{ 0xFF, 0x00, 0xC0 },	// 5, 37
	{ 0xFF, 0x00, 0x80 },	// 6, 38
	{ 0xFF, 0x00, 0x40 },	// 7, 39
	{ 0xFF, 0x00, 0x00 },	// 8, 40
	{ 0xFF, 0x40, 0x00 },	// 9, 41
	{ 0xFF, 0x80, 0x00 },	// 10,42
	{ 0xFF, 0xC0, 0x00 },	// 11,43
	{ 0xFF, 0xFF, 0x00 },	// 12,44
	{ 0xC0, 0xFF, 0x00 },	// 13,45
	{ 0x80, 0xFF, 0x00 },	// 14,46
	{ 0x40, 0xFF, 0x00 },	// 15,47
	// row 3 (R)
	{ 0x00, 0xFF, 0x00 },	// 0, 48
	{ 0x00, 0xFF, 0x40 },	// 1, 49
	{ 0x00, 0xFF, 0x80 },	// 2, 50
	{ 0x00, 0xFF, 0xC0 },	// 3, 51
	{ 0x00, 0xFF, 0xFF },	// 4, 52
	{ 0x00, 0xC0, 0xFF },	// 5, 53
	{ 0x00, 0x80, 0xFF },	// 6, 54
	{ 0x00, 0x40, 0xFF },	// 7, 55
	{ 0x80, 0x80, 0xFF },	// 8, 56
	{ 0xA0, 0x80, 0xFF },	// 9, 57
	{ 0xC0, 0x80, 0xFF },	// 10,58
	{ 0xE0, 0x80, 0xFF },	// 11,59
	{ 0xFF, 0x80, 0xFF },	// 12,60
	{ 0xFF, 0x80, 0xE0 },	// 13,61
	{ 0xFF, 0x80, 0xC0 },	// 14,62
	{ 0xFF, 0x80, 0xA0 },	// 15,63
	// row 4 (R)
	{ 0xFF, 0x80, 0x80 },	// 0, 64
	{ 0xFF, 0xA0, 0x80 },	// 1, 65
	{ 0xFF, 0xC0, 0x80 },	// 2, 66
	{ 0xFF, 0xE0, 0x80 },	// 3, 67
	{ 0xFF, 0xFF, 0x80 },	// 4, 68
	{ 0xE0, 0xFF, 0x80 },	// 5, 69
	{ 0xC0, 0xFF, 0x80 },	// 6, 70
	{ 0xA0, 0xFF, 0x80 },	// 7, 71
	{ 0x80, 0xFF, 0x80 },	// 8, 72
	{ 0x80, 0xFF, 0xA0 },	// 9, 73
	{ 0x80, 0xFF, 0xC0 },	// 10,74
	{ 0x80, 0xFF, 0xE0 },	// 11,75
	{ 0x80, 0xFF, 0xFF },	// 12,76
	{ 0x80, 0xE0, 0xFF },	// 13,77
	{ 0x80, 0xC0, 0xFF },	// 14,78
	{ 0x80, 0xA0, 0xFF },	// 15,79
	// row 5 (R)
	{ 0xB4, 0xB4, 0xFF },	// 0, 80
	{ 0xC4, 0xB4, 0xFF },	// 1, 81
	{ 0xD8, 0xB4, 0xFF },	// 2, 82
	{ 0xE8, 0xB4, 0xFF },	// 3, 83
	{ 0xFF, 0xB4, 0xFF },	// 4, 84
	{ 0xFF, 0xB4, 0xE8 },	// 5, 85
	{ 0xFF, 0xB4, 0xD8 },	// 6, 86
	{ 0xFF, 0xB4, 0xC4 },	// 7, 87
	{ 0xFF, 0xB4, 0xB4 },	// 8, 88
	{ 0xFF, 0xC4, 0xB4 },	// 9, 89
	{ 0xFF, 0xD8, 0xB4 },	// 10,90
	{ 0xFF, 0xE8, 0xB4 },	// 11,91
	{ 0xFF, 0xFF, 0xB4 },	// 12,92
	{ 0xE8, 0xFF, 0xB4 },	// 13,93
	{ 0xD8, 0xFF, 0xB4 },	// 14,94
	{ 0xC4, 0xFF, 0xB4 },	// 15,95
	// row 6 (R)
	{ 0xB4, 0xFF, 0xB4 },	// 0, 96
	{ 0xB4, 0xFF, 0xC4 },	// 1, 97
	{ 0xB4, 0xFF, 0xD8 },	// 2, 98
	{ 0xB4, 0xFF, 0xE8 },	// 3, 99
	{ 0xB4, 0xFF, 0xFF },	// 4, 100
	{ 0xB4, 0xE8, 0xFF },	// 5, 101
	{ 0xB4, 0xD8, 0xFF },	// 6, 102
	{ 0xB4, 0xC4, 0xFF },	// 7, 103
	{ 0x00, 0x00, 0x70 },	// 8, 104
	{ 0x20, 0x00, 0x70 },	// 9, 105
	{ 0x38, 0x00, 0x70 },	// 10,106
	{ 0x54, 0x00, 0x70 },	// 11,107
	{ 0x70, 0x00, 0x70 },	// 12,108
	{ 0x70, 0x00, 0x54 },	// 13,109
	{ 0x70, 0x00, 0x38 },	// 14,110
	{ 0x70, 0x00, 0x20 },	// 15,111
	// row 7 (R)
	{ 0x70, 0x00, 0x00 },	// 0, 112
	{ 0x70, 0x20, 0x00 },	// 1, 113
	{ 0x70, 0x38, 0x00 },	// 2, 114
	{ 0x70, 0x54, 0x00 },	// 3, 115
	{ 0x70, 0x70, 0x00 },	// 4, 116
	{ 0x54, 0x70, 0x00 },	// 5, 117
	{ 0x38, 0x70, 0x00 },	// 6, 118
	{ 0x20, 0x70, 0x00 },	// 7, 119
	{ 0x00, 0x70, 0x00 },	// 8, 120
	{ 0x00, 0x70, 0x20 },	// 9, 121
	{ 0x00, 0x70, 0x38 },	// 10,122
	{ 0x00, 0x70, 0x54 },	// 11,123
	{ 0x00, 0x70, 0x70 },	// 12,124
	{ 0x00, 0x54, 0x70 },	// 13,125
	{ 0x00, 0x38, 0x70 },	// 14,126
	{ 0x00, 0x20, 0x70 },	// 15,127
	// row 8 (R)
	{ 0x38, 0x38, 0x70 },	// 0, 128
	{ 0x44, 0x38, 0x70 },	// 1, 129
	{ 0x54, 0x38, 0x70 },	// 2, 130
	{ 0x60, 0x38, 0x70 },	// 3, 131
	{ 0x70, 0x38, 0x70 },	// 4, 132
	{ 0x70, 0x38, 0x60 },	// 5, 133
	{ 0x70, 0x38, 0x54 },	// 6, 134
	{ 0x70, 0x38, 0x44 },	// 7, 135
	{ 0x70, 0x38, 0x38 },	// 8, 136
	{ 0x70, 0x44, 0x38 },	// 9, 137
	{ 0x70, 0x54, 0x38 },	// 10,138
	{ 0x70, 0x60, 0x38 },	// 11,139
	{ 0x70, 0x70, 0x38 },	// 12,140
	{ 0x60, 0x70, 0x38 },	// 13,141
	{ 0x54, 0x70, 0x38 },	// 14,142
	{ 0x44, 0x70, 0x38 },	// 15,143
	// row 9 (R)
	{ 0x38, 0x70, 0x38 },	// 0, 144
	{ 0x38, 0x70, 0x44 },	// 1, 145
	{ 0x38, 0x70, 0x54 },	// 2, 145
	{ 0x38, 0x70, 0x60 },	// 3, 147
	{ 0x38, 0x70, 0x70 },	// 4, 148
	{ 0x38, 0x60, 0x70 },	// 5, 149
	{ 0x38, 0x54, 0x70 },	// 6, 150
	{ 0x38, 0x44, 0x70 },	// 7, 151
	{ 0x50, 0x50, 0x70 },	// 8, 152
	{ 0x58, 0x50, 0x70 },	// 9, 153
	{ 0x60, 0x50, 0x70 },	// 10,154
	{ 0x68, 0x50, 0x70 },	// 11,155
	{ 0x70, 0x50, 0x70 },	// 12,156
	{ 0x70, 0x50, 0x68 },	// 13,157
	{ 0x70, 0x50, 0x60 },	// 14,158
	{ 0x70, 0x50, 0x58 },	// 15,159
	// row 10 (R)
	{ 0x70, 0x50, 0x50 },	// 0, 160
	{ 0x70, 0x58, 0x50 },	// 1, 161
	{ 0x70, 0x60, 0x50 },	// 2, 162
	{ 0x70, 0x68, 0x50 },	// 3, 163
	{ 0x70, 0x70, 0x50 },	// 4, 164
	{ 0x68, 0x70, 0x50 },	// 5, 165
	{ 0x60, 0x70, 0x50 },	// 6, 166
	{ 0x58, 0x70, 0x50 },	// 7, 167
	{ 0x50, 0x70, 0x50 },	// 8, 168
	{ 0x50, 0x70, 0x58 },	// 9, 169
	{ 0x50, 0x70, 0x60 },	// 10,170
	{ 0x50, 0x70, 0x68 },	// 11,171
	{ 0x50, 0x70, 0x70 },	// 12,172
	{ 0x50, 0x68, 0x70 },	// 13,173
	{ 0x50, 0x60, 0x70 },	// 14,174
	{ 0x50, 0x58, 0x70 },	// 15,175
	// row 11 (R)
	{ 0x00, 0x00, 0x40 },	// 0, 176
	{ 0x10, 0x00, 0x40 },	// 1, 177
	{ 0x20, 0x00, 0x40 },	// 2, 178
	{ 0x30, 0x00, 0x40 },	// 3, 179
	{ 0x40, 0x00, 0x40 },	// 4, 180
	{ 0x40, 0x00, 0x30 },	// 5, 181
	{ 0x40, 0x00, 0x20 },	// 6, 182
	{ 0x40, 0x00, 0x10 },	// 7, 183
	{ 0x40, 0x00, 0x00 },	// 8, 184
	{ 0x40, 0x10, 0x00 },	// 9, 185
	{ 0x40, 0x20, 0x00 },	// 10,186
	{ 0x40, 0x30, 0x00 },	// 11,187
	{ 0x40, 0x40, 0x00 },	// 12,188
	{ 0x30, 0x40, 0x00 },	// 13,189
	{ 0x20, 0x40, 0x00 },	// 14,190
	{ 0x10, 0x40, 0x00 },	// 15,191
	// row 12 (R)
	{ 0x00, 0x40, 0x00 },	// 0, 192
	{ 0x00, 0x40, 0x10 },	// 1, 193
	{ 0x00, 0x40, 0x20 },	// 2, 194
	{ 0x00, 0x40, 0x30 },	// 3, 195
	{ 0x00, 0x40, 0x40 },	// 4, 196
	{ 0x00, 0x30, 0x40 },	// 5, 197
	{ 0x00, 0x20, 0x40 },	// 6, 198
	{ 0x00, 0x10, 0x40 },	// 7, 199
	{ 0x20, 0x20, 0x40 },	// 8, 200
	{ 0x28, 0x20, 0x40 },	// 9, 201
	{ 0x30, 0x20, 0x40 },	// 10,202
	{ 0x38, 0x20, 0x40 },	// 11,203
	{ 0x40, 0x20, 0x40 },	// 12,204
	{ 0x40, 0x20, 0x38 },	// 13,205
	{ 0x40, 0x20, 0x30 },	// 14,206
	{ 0x40, 0x20, 0x28 },	// 15,207
	// row 13 (R)
	{ 0x40, 0x20, 0x20 },	// 0, 208
	{ 0x40, 0x28, 0x20 },	// 1, 209
	{ 0x40, 0x30, 0x20 },	// 2, 210
	{ 0x40, 0x38, 0x20 },	// 3, 211
	{ 0x40, 0x40, 0x20 },	// 4, 212
	{ 0x38, 0x40, 0x20 },	// 5, 213
	{ 0x30, 0x40, 0x20 },	// 6, 214
	{ 0x28, 0x40, 0x20 },	// 7, 215
	{ 0x20, 0x40, 0x20 },	// 8, 216
	{ 0x20, 0x40, 0x28 },	// 9, 217
	{ 0x20, 0x40, 0x30 },	// 10,218
	{ 0x20, 0x40, 0x38 },	// 11,219
	{ 0x20, 0x40, 0x40 },	// 12,220
	{ 0x20, 0x38, 0x40 },	// 13,221
	{ 0x20, 0x30, 0x40 },	// 14,222
	{ 0x20, 0x28, 0x40 },	// 15,223
	// row 14 (R)
	{ 0x2C, 0x2C, 0x40 },	// 0, 224
	{ 0x30, 0x2C, 0x40 },	// 1, 225
	{ 0x34, 0x2C, 0x40 },	// 2, 226
	{ 0x3C, 0x2C, 0x40 },	// 3, 227
	{ 0x40, 0x2C, 0x40 },	// 4, 228
	{ 0x40, 0x2C, 0x3C },	// 5, 229
	{ 0x40, 0x2C, 0x34 },	// 6, 230
	{ 0x40, 0x2C, 0x30 },	// 7, 231
	{ 0x40, 0x2C, 0x2C },	// 8, 232
	{ 0x40, 0x30, 0x2C },	// 9, 233
	{ 0x40, 0x34, 0x2C },	// 10,234
	{ 0x40, 0x3C, 0x2C },	// 11,235
	{ 0x40, 0x40, 0x2C },	// 12,236
	{ 0x3C, 0x40, 0x2C },	// 13,237
	{ 0x34, 0x40, 0x2C },	// 14,238
	{ 0x30, 0x40, 0x2C },	// 15,239
	// row 15 (R)
	{ 0x2C, 0x40, 0x2C },	// 0, 240
	{ 0x2C, 0x40, 0x30 },	// 1, 241
	{ 0x2C, 0x40, 0x34 },	// 2, 242
	{ 0x2C, 0x40, 0x3C },	// 3, 243
	{ 0x2C, 0x40, 0x40 },	// 4, 244
	{ 0x2C, 0x3C, 0x40 },	// 5, 245
	{ 0x2C, 0x34, 0x40 },	// 6, 246
	{ 0x00, 0x00, 0x00 },	// 7, 247
	{ 0x00, 0x00, 0x00 },	// 8, 248 - black
	{ 0x00, 0x00, 0x00 },	// 9, 249 - black
	{ 0x00, 0x00, 0x00 },	// 10,250 - black
	{ 0x00, 0x00, 0x00 },	// 11,251 - black
	{ 0x00, 0x00, 0x00 },	// 12,252 - black
	{ 0x00, 0x00, 0x00 },	// 13,253 - black
	{ 0x00, 0x00, 0x00 },	// 14,254 - black
	{ 0x00, 0x00, 0x00 },	// 15,255 - black
};		// 768 bytes

#endif	// LCD_COLOR_FORMAT

#endif	// RGBCOLOR_AS_NUMBER==1

static uint8_t lcd1_buffer[LCD1_BUFFER_SZ];
static st7735_handle lcd1_handle = 0;

// general status
volatile struct op_status op_stat = { 0, 0 };

// current form data
uint8_t form_data[FORM_DATA_MAX_SZ];

AppState appState;

form_def form_test_text = { &formTestText_reset, &formTestText_onDraw, &formTestText_onButton };
form_def form_test_text_ru = { &formTestTextRU_reset, &formTestTextRU_onDraw, &formTestTextRU_onButton };
form_def form_test_image_wb = { &formTestImageWB_reset, &formTestImageWB_onDraw, &formTestImageWB_onButton };
form_def form_bench = { &formBench_reset, &formBench_onDraw, &formBench_onButton };

form_def* curr_form;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

void lcd1_clearBuffer();

#if LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX1BPP

static int8_t lcd1_setPixel_fb_1bpp(int16_t x, int16_t y, const uint8_t* raw_color);
static int8_t lcd1_syncBuffer_1bpp();

#elif LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX4BPP

static int8_t lcd1_setPixel_fb_4bpp(int16_t x, int16_t y, const uint8_t* raw_color);
static int8_t lcd1_syncBuffer_4bpp();

#elif LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX8BPP

static int8_t lcd1_setPixel_fb_8bpp(int16_t x, int16_t y, const uint8_t* raw_color);
static int8_t lcd1_syncBuffer_8bpp();

#elif LCD_COLOR_FORMAT == COLOR_FORMAT_RGB565

static int8_t lcd1_setPixel_fb_16bpp(int16_t x, int16_t y, const uint8_t* raw_color);
static int8_t lcd1_syncBuffer_16bpp();

#elif LCD_COLOR_FORMAT == COLOR_FORMAT_RGB888

static int8_t lcd1_setPixel_fb_24bpp(int16_t x, int16_t y, const uint8_t* raw_color);
static int8_t lcd1_syncBuffer_24bpp();

#endif

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

#if 1
	// LCD Reset sequence
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(120);		// delay to wait LCD initialization
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);		// reset impulse minimum is 10us length
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(120);		// 5-120 ms LCD is initialized
#endif

	// Turn on LED1
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);

	// Init ST7735 LCD
	lcd1_handle = st7735_init_hal(&hspi1, GPIOB, GPIO_PIN_6, GPIOC, GPIO_PIN_7,
			130, 161, ST7735_IPF_16BIT);
	uint8_t ret = lcd1_handle != 0 ? 0 : -1;
	if (ret != 0)
	{
		// LCD init failed, blink LED1 (PA3)
		while (1)
		{
			// Blink LED1
			HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
			HAL_Delay(200);
			HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
			HAL_Delay(200);
		}
	}

	memset((void*)&appState, 0, sizeof(AppState));

	uint8_t data[4] = { 0, 0, 0, 0 };
	ret = st7735_rdcmd(lcd1_handle, ST7735_CMD_RDDID, data, 4);
	if (0 != ret)
		printf("ST7735: failed to read DeviceID!\n");
	appState.devID = ((uint32_t)(data[0]) << 16) |
					 ((uint32_t)(data[1]) << 8) |
					  (uint32_t)(data[2]);

	appState.gfx_color_format = LCD_COLOR_FORMAT;
	appState.st7735_ipf = st7735_getipf(lcd1_handle);
	appState.form_data = form_data;
	appState.changed = APPSTATE_CHANGED_NEEDREDRAW;

	// Turn ON LCD Backlight
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);

	lcd1_clearBuffer();
	// Setup GFX module
#if LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX1BPP
	setFuncSetPixelColor(&lcd1_setPixel_fb_1bpp);
#elif LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX4BPP
	setFuncSetPixelColor(&lcd1_setPixel_fb_4bpp);
#elif LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX8BPP
	setFuncSetPixelColor(&lcd1_setPixel_fb_8bpp);
#elif LCD_COLOR_FORMAT == COLOR_FORMAT_RGB565
	setFuncSetPixelColor(&lcd1_setPixel_fb_16bpp);
#elif LCD_COLOR_FORMAT == COLOR_FORMAT_RGB888
	setFuncSetPixelColor(&lcd1_setPixel_fb_24bpp);
#endif
	setColorFormat(LCD_COLOR_FORMAT);

	curr_form = &form_test_text;
	curr_form->reset_proc(&appState);

	// Turn off LED1
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
	// Init OK, burn LED2
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);

	__enable_irq();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		// Poll buttons/joystick
		if (op_stat.status & STATUS_BTN_RDY_Msk)
		{
			printf("btn=%u\n", op_stat.button_code);

			if ((op_stat.status & STATUS_BTN_PRE_Msk) == STATUS_BTN_PRE_Msk)
			{
				//curr_form->onButtonPress_proc(&appState, op_stat.button_code);
				curr_form->onButton_proc(&appState, op_stat.button_code);
			}
			else if ((op_stat.status & STATUS_BTN_REP_Msk) == STATUS_BTN_REP_Msk)
				curr_form->onButton_proc(&appState, op_stat.button_code);
			//else
			//	curr_form->onButtonRelease_proc(op_stat.button_code);		// not pressed, not repeated => released
			// Clear for next button event
			op_stat.status &= ~STATUS_BTN_RDY_Msk;
		}
		if (appState.changed & APPSTATE_CHANGED_FORM)
		{
			appState.changed &= ~APPSTATE_CHANGED_FORM;
			// запрошено переключение на другую форму
			switch (appState.form_code)
			{
				case FORM_TEXT_TEST:
					curr_form = &form_test_text;
					break;
				case FORM_TEXT_RU_TEST:
					curr_form = &form_test_text_ru;
					break;
				case FORM_WB_IMAGE_TEST:
					curr_form = &form_test_image_wb;
					break;
				case FORM_BENCH:
					curr_form = &form_bench;
					break;
				default:
					curr_form = &form_test_text;
					break;
			}
			curr_form->reset_proc(&appState);
			appState.changed |= APPSTATE_CHANGED_NEEDREDRAW;
		}
		if (curr_form == &form_bench) {
			appState.changed |= APPSTATE_CHANGED_NEEDREDRAW;
		}
		if (appState.changed & APPSTATE_CHANGED_NEEDREDRAW)
		{
			appState.changed &= ~APPSTATE_CHANGED_NEEDREDRAW;
			lcd1_clearBuffer();
			uint32_t t1 = HAL_GetTick();
			curr_form->onDraw_proc(&appState);
			appState.rendTime = HAL_GetTick() - t1;
			t1 = HAL_GetTick();
#if LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX1BPP
			lcd1_syncBuffer_1bpp();
#elif LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX4BPP
			lcd1_syncBuffer_4bpp();
#elif LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX8BPP
			lcd1_syncBuffer_8bpp();
#elif LCD_COLOR_FORMAT == COLOR_FORMAT_RGB565
			lcd1_syncBuffer_16bpp();
#elif LCD_COLOR_FORMAT == COLOR_FORMAT_RGB888
			lcd1_syncBuffer_24bpp();
#endif
			appState.syncTime = HAL_GetTick() - t1;
		}
  }
  st7735_free(lcd1_handle);
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 50;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 17999;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED1_Pin|LED2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED1_Pin LED2_Pin LCD_RESET_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|LCD_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_DCX_Pin */
  GPIO_InitStruct.Pin = LCD_DCX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LCD_DCX_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BT1_Pin */
  GPIO_InitStruct.Pin = BT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BT3_Pin BT2_Pin */
  GPIO_InitStruct.Pin = BT3_Pin|BT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_CS_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

void lcd1_clearBuffer()
{
	memset(lcd1_buffer, 0, sizeof(lcd1_buffer));
}

#if LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX1BPP

static int8_t lcd1_setPixel_fb_1bpp(int16_t x, int16_t y, const uint8_t* raw_color)
{
	// In 1bpp color mode *raw_color is an integer in range 0-1
	if (x < 0 || x > 131 || y < 0 || y > 161)
		return -1;
	register uint8_t xoff = 7 - x % 8;
	//uint8_t* ptr = lcd1_buffer + y*16 + x/8;
	uint8_t* ptr = lcd1_buffer + (y << 4) + (x >> 3);
	if (*raw_color)
		*ptr |= (1 << xoff);		// white
	else
		*ptr &= ~(1 << xoff);		// black
	return 0;
}

static int8_t lcd1_syncBuffer_1bpp()
{
	return st7735_drawBitmap_1bpp(lcd1_handle, 0, 0, 128, 160, lcd1_buffer, sizeof(lcd1_buffer), lcd1_1bpp_palette);
}

#elif LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX4BPP

static int8_t lcd1_setPixel_fb_4bpp(int16_t x, int16_t y, const uint8_t* raw_color)
{
	// In 4bpp color mode *raw_color is an integer in range 0-15
	if (x < 0 || x > 131 || y < 0 || y > 161)
		return -1;
	//uint8_t* ptr = lcd1_buffer + y*64 + x/2;
	uint8_t* ptr = lcd1_buffer + (y << 6) + (x >> 1);
	//uint8_t* ptr = lcd1_buffer + y*65 + x/2;
	if (x & 0x01)
		*ptr = (*ptr & 0xF0) | (*raw_color & 0x0F);
	else
		*ptr = (*ptr & 0x0F) | (*raw_color << 4);
	return 0;
}

static int8_t lcd1_syncBuffer_4bpp()
{
	return st7735_drawBitmap_4bpp(lcd1_handle, 0, 0, 128, 160, lcd1_buffer, sizeof(lcd1_buffer), lcd1_4bpp_palette);
}

#elif LCD_COLOR_FORMAT == COLOR_FORMAT_INDEX8BPP

static int8_t lcd1_setPixel_fb_8bpp(int16_t x, int16_t y, const uint8_t* raw_color)
{
	// In 8bpp color mode *raw_color is whole byte
	if (x < 0 || x > 131 || y < 0 || y > 161)
		return -1;
	//uint8_t* ptr = lcd1_buffer + y*128 + x;
	uint8_t* ptr = lcd1_buffer + (y << 7) + x;
	*ptr = *raw_color;
	return 0;
}

static int8_t lcd1_syncBuffer_8bpp()
{
	return st7735_drawBitmap_8bpp(lcd1_handle, 0, 0, 128, 160, lcd1_buffer, sizeof(lcd1_buffer), lcd1_8bpp_palette);
}

#elif LCD_COLOR_FORMAT == COLOR_FORMAT_RGB565

static int8_t lcd1_setPixel_fb_16bpp(int16_t x, int16_t y, const uint8_t* raw_color)
{
	// In 16bpp (R5B6G5) color mode raw_color is two bytes: |RRRRRGGG|GGGBBBBB|
	if (x < 0 || x > 131 || y < 0 || y > 161)
		return -1;
	//uint8_t* ptr = lcd1_buffer + (y*128 + x)*2;
	uint8_t* ptr = lcd1_buffer + (((y << 7) + x) << 1);
	*ptr++ = *raw_color++;
	*ptr = *raw_color;
	return 0;
}

static int8_t lcd1_syncBuffer_16bpp()
{
	return st7735_drawBitmap_16bpp(lcd1_handle, 0, 0, 128, 160, lcd1_buffer, sizeof(lcd1_buffer));
}

#elif LCD_COLOR_FORMAT == COLOR_FORMAT_RGB888

static int8_t lcd1_setPixel_fb_24bpp(int16_t x, int16_t y, const uint8_t* raw_color)
{
	// In 24bpp (R8B8G8) color mode raw_color is three bytes: |RRRRRRRR|GGGGGGGG|BBBBBBBB|
	if (x < 0 || x > 131 || y < 0 || y > 161)
		return -1;
	//uint8_t* ptr = lcd1_buffer + (y*128 + x)*3;
	uint8_t* ptr = lcd1_buffer + (((y << 7) + x)*3);
	*ptr++ = *raw_color++;
	*ptr++ = *raw_color++;
	*ptr = *raw_color;
	return 0;
}

static int8_t lcd1_syncBuffer_24bpp()
{
	return st7735_drawBitmap_24bpp(lcd1_handle, 0, 0, 128, 160, lcd1_buffer, sizeof(lcd1_buffer));
}

#endif	// LCD_COLOR_FORMAT

// for SWV ITM Data Console
int __io_putchar(int ch)
{
	return ITM_SendChar(ch);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if ((op_stat.status & STATUS_BTN_CPT_Msk) != 0)
	{
		// handle contact bounce timeout
		op_stat.status = (op_stat.status & ~STATUS_BTN_CPT_Msk) | STATUS_BTN_RDY_Msk;
		// Stop timer (both contact bouncer and autorepeat)
		HAL_TIM_Base_Stop_IT(htim);
		if (op_stat.status & STATUS_BTN_PRE_Msk)
		{
			// check if any buttons still is pressed
			if ( (GPIOA->IDR & GPIO_IDR_ID10_Msk) == 0 ||	// BT1
				 (GPIOB->IDR & GPIO_IDR_ID5_Msk) == 0 ||	// BT2
				 (GPIOB->IDR & GPIO_IDR_ID4_Msk) == 0 )		// BT3
			{
				// any buttons in low state - pressed
				// Restart timer (TIM3) for autorepeat key events
				htim->Instance->CNT = 0;
				htim->Instance->ARR = 19999;
				//htim->Instance->CR1 |= TIM_CR1_CEN_Msk;
				HAL_TIM_Base_Start_IT(htim);
			}
			else
			{
				op_stat.status &= ~STATUS_BTN_REP_Msk;
			}
		}
	}
	else
	{
		// handle key event autorepeat
		if (op_stat.status & STATUS_BTN_PRE_Msk)
		{
			// check if any buttons already is not pressed
			if ( (GPIOA->IDR & GPIO_IDR_ID10_Msk) &&		// BT1
				 (GPIOB->IDR & GPIO_IDR_ID5_Msk) &&			// BT2
				 (GPIOB->IDR & GPIO_IDR_ID4_Msk) )			// BT3
				// all buttons in High state - unpressed
				op_stat.status &= ~(STATUS_BTN_PRE_Msk | STATUS_BTN_REP_Msk);
		}
		if (op_stat.status & STATUS_BTN_PRE_Msk)
		{
			// Button pressed, repeat key event
			op_stat.status |= STATUS_BTN_REP_Msk | STATUS_BTN_RDY_Msk;
			// Set repeat delay to 200ms
			htim->Instance->CNT = 0;
			htim->Instance->ARR = 1999;
		}
		else
		{
			// Button released
			op_stat.status |= STATUS_BTN_RDY_Msk;
			// Stop autorepeat timer
			HAL_TIM_Base_Stop_IT(&htim3);
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	static uint8_t code;
	uint8_t btn_mask;
	switch (GPIO_Pin)
	{
	case GPIO_PIN_10:
		btn_mask = STATUS_BTN_CPT_BT1_Msk;
		break;
	case GPIO_PIN_5:
		btn_mask = STATUS_BTN_CPT_BT2_Msk;
		break;
	case GPIO_PIN_4:
		btn_mask = STATUS_BTN_CPT_BT3_Msk;
		break;
	default:
		return;
	}
	if ((op_stat.status & btn_mask) == 0)
	{
		op_stat.status = btn_mask;
		code = 0;
		if ((GPIOA->IDR & GPIO_IDR_ID10_Msk) == 0)		// BT1
			code |= 0x01;
		if ((GPIOB->IDR & GPIO_IDR_ID5_Msk) == 0)		// BT2
			code |= 0x02;
		if ((GPIOB->IDR & GPIO_IDR_ID4_Msk) == 0)		// BT3
			code |= 0x04;
		if (code > 0)
		{
			op_stat.button_code = code;
			op_stat.status |= STATUS_BTN_PRE_Msk;
		}
		// start delay timer to ignore contact bounce
		htim3.Instance->CNT = 0;
		htim3.Instance->ARR = 999;
		HAL_TIM_Base_Start_IT(&htim3);
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
