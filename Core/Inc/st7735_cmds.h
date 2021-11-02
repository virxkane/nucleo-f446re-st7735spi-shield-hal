// -*- coding: utf-8 -*-
/***************************************************************************
 *   Copyright (C) 2019-2020 by Chernov A.A.                               *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef ST7735_CMDS_H
#define ST7735_CMDS_H

/**
 * No Operation.
 * No arguments.
 * No answer.
 *
 * This command is empty command.
 */
#define ST7735_CMD_NOP			0x00

/**
 * Software Reset.
 * No arguments.
 * No answer.
 *
 * If Software Reset is applied during Sleep In mode, it will be necessary
 * to wait 120msec before sending next command.
 * The display module loads all default values to the registers during 120msec.
 * If Software Reset is applied during Sleep Out or Display On Mode,
 * it will be necessary to wait 120msec before sending next command.
 */
#define ST7735_CMD_SWRESET		0x01

/**
 * Read display ID.
 * No arguments.
 * Answer - 24 bit integer.
 */
#define ST7735_CMD_RDDID		0x04

/**
 * Read display status.
 * No arguments.
 * Answer - 32 bit integer.
 */
#define ST7735_CMD_RDDST		0x09

/**
 * Read display power mode.
 * No arguments.
 * Answer - 8 bit integer.
 */
#define ST7735_CMD_RDDPM		0x0A

/**
 * Read display MADCTL.
 * No arguments.
 * Answer - 8 bit integer.
 */
#define ST7735_CMD_RDD_MADCTL	0x0B

/**
 * Read display pixel format.
 * No arguments.
 * Answer - 8 bit integer.
 */
#define ST7735_CMD_RDD_COLMOD	0x0C

/**
 * Read display image mode.
 * No arguments.
 * Answer - 8 bit integer.
 */
#define ST7735_CMD_RDDIM		0x0D

/**
 * Read display signal mode.
 * No arguments.
 * Answer - 8 bit integer.
 */
#define ST7735_CMD_RDDSM		0x0E

/**
 * Read display self-diagnostic result.
 * No arguments.
 * Answer - 8 bit integer.
 */
#define ST7735_CMD_RDDSDR		0x0F

/**
 * Sleep in & booster off.
 * No arguments.
 * No answer.
 */
#define ST7735_CMD_SLPIN		0x10

/**
 * Sleep out & booster on.
 * No arguments.
 * No answer.
 */
#define ST7735_CMD_SLPOUT		0x11

/**
 * Partial mode on.
 * No arguments.
 * No answer.
 */
#define ST7735_CMD_PTLON		0x12

/**
 * Partial mode off (Normal display mode on).
 * No arguments.
 * No answer.
 */
#define ST7735_CMD_NORON		0x13

/**
 * Display inversion off (normal).
 * No arguments.
 * No answer.
 */
#define ST7735_CMD_INVOFF		0x20

/**
 * Display inversion on.
 * No arguments.
 * No answer.
 */
#define ST7735_CMD_INVON		0x21

/**
 * Gamma curve select.
 * Arguments: 8 bit integer
 * No answer.
 */
#define ST7735_CMD_GAMSET		0x26

/**
 * Display off.
 * No arguments.
 * No answer.
 */
#define ST7735_CMD_DISPOFF		0x28

/**
 * Display on.
 * No arguments.
 * No answer.
 */
#define ST7735_CMD_DISPON		0x29

/**
 * Column address set.
 * Arguments:
 * 	1) 8 bit integer (MSB) - high byte of X address start (XS)
 * 	2) 8 bit integer (MSB) -  low byte of X address start (XS)
 * 	3) 8 bit integer (MSB) - high byte of X address end (XE)
 * 	4) 8 bit integer (MSB) -  low byte of X address end (XE)
 * No answer.
 *
 * The value of XS and XE are referred when RAMWR command comes.
 * Each value represents one column line in the Frame Memory.
 */
#define ST7735_CMD_CASET		0x2A

/**
 * Row address set.
 * Arguments:
 * 	1) 8 bit integer (MSB) - high byte of Y address start (YS)
 * 	2) 8 bit integer (MSB) -  low byte of Y address start (YS)
 * 	3) 8 bit integer (MSB) - high byte of Y address end (YE)
 * 	4) 8 bit integer (MSB) -  low byte of Y address end (YE)
 * No answer.
 *
 * The value of YS and YE are referred when RAMWR command comes.
 * Each value represents one column line in the Frame Memory.
 */
#define ST7735_CMD_RASET		0x2B

/**
 * Memory write.
 * Arguments:
 *  Array of 8 bit integer.
 * No answer.
 *
 * In all color modes, there is no restriction on length of parameters.
 * 1. 128X160 memory base (GM = ‘11’)
 * 	128x160x18-bit memory can be written by this command
 * 	Memory range: (0000h, 0000h) -> (007Fh, 09Fh)
 * 2. 132x132 memory base (GM = ‘01’)
 * 	132x132x18-bit memory can be written on this command.
 * 	Memory range: (0000h, 0000h) -> (0083h, 0083h)
 * 3. 132x162 memory base (GM = ‘00’)
 * 	132x162x18-bit memory can be written on this command.
 * 	Memory range: (0000h, 0000h) -> (0083h, 00A1h)
 */
#define ST7735_CMD_RAMWR		0x2C

/**
 * LUT for 4k,65k,262k Color display.
 * No arguments.
 * No answer.
 */
#define ST7735_CMD_RGBSET		0x2D

/**
 * Memory read.
 * No arguments.
 * Answer - 8 bit integer
 */
#define ST7735_CMD_RAMRD		0x2E

/**
 * Partial start/end address set.
 * Arguments:
 * 	1) 8 bit integer - high byte of partial start address
 * 	2) 8 bit integer -  low byte of partial start address
 * 	3) 8 bit integer - high byte of partial end address
 * 	4) 8 bit integer -  low byte of partial end address
 * No answer.
 */
#define ST7735_CMD_PTLAR		0x30

/**
 * Scroll area set.
 * Arguments:
 * 	1) 8 bit integer (ignored)
 * 	2) 8 bit integer - top fixed area
 * 	3) 8 bit integer (ignored)
 * 	4) 8 bit integer - vertical scroll area
 * 	5) 8 bit integer (ignored)
 * 	6) 8 bit integer - bottom fixed area
 * No answer.
 */
#define ST7735_CMD_SCRLAR		0x33

/**
 * Tearing effect line off.
 * No arguments.
 * No answer.
 */
#define ST7735_CMD_TEOFF		0x34

/**
 * Tearing effect mode set & on.
 * Arguments:
 * 	1) 8 bit integer - tearing mode.
 * No answer.
 */
#define ST7735_CMD_TEON			0x35

/**
 * Memory data access control.
 * Arguments:
 * 	1) 8 bit integer - parameter:
 * 	| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 * 	| MY| MX| MV| ML|RGB| MH| - | - |
 * No answer.
 *
 * This command defines read/ write scanning direction of frame memory.
 * |Bit| NAME                   |DESCRIPTION
 * -----------------------------------------------------------------------------
 * | MY|Row Address Order       | These 3bits controls                         |
 * | MX|Column Address Order    |   MCU to memory                              |
 * | MV|Row/Column Exchange     |   write/read direction.                      |
 * -----------------------------------------------------------------------------
 * | ML|Vertical Refresh Order  | LCD vertical refresh direction control       |
 * |   |                        |   ‘0’ = LCD vertical refresh Top to Bottom   |
 * |   |                        |   ‘1’ = LCD vertical refresh Bottom to Top   |
 * -----------------------------------------------------------------------------
 * |RGB|RGB-BGR ORDER           | Color selector switch control                |
 * |   |                        |   ‘0’ =RGB color filter panel,               |
 * |   |                        |   ‘1’ =BGR color filter panel)               |
 * -----------------------------------------------------------------------------
 * | MH|Horizontal Refresh Order| LCD horizontal refresh direction control     |
 * |   |                        |   ‘0’ = LCD horizontal refresh Left to right |
 * |   |                        |   ‘1’ = LCD horizontal refresh right to left |
 * -----------------------------------------------------------------------------
 */
#define ST7735_CMD_MADCTL		0x36

/**
 * Scroll start address of RAM.
 * Arguments:
 * 	1) 8 bit integer (ignored)
 * 	2) 8 bit integer - scroll start address (0..161)
 * No answer.
 */
#define ST7735_CMD_VSCSAD		0x37

/**
 * Idle mode off.
 * No arguments.
 * No answer.
 */
#define ST7735_CMD_IDMOFF		0x38

/**
 * Idle mode on.
 * No arguments.
 * No answer.
 */
#define ST7735_CMD_IDMON		0x39

/**
 * Interface pixel format.
 * Arguments:
 * 	1) 8 bit integer - interface format (IFPF).
 * No answer.
 *
 * This command is used to define the format of RGB picture data, which is to be transferred via
 * the MCU interface. The formats are shown in the table:
 * IFPF[2:0] MCU Interface Color Format
 * 011 | 3 | 12-bit/pixel
 * 101 | 5 | 16-bit/pixel
 * 110 | 6 | 18-bit/pixel
 * 111 | 7 | No used
 */
#define ST7735_CMD_COLMOD		0x3A

#define ST7735_IPF_12BIT	0x03
#define ST7735_IPF_16BIT	0x05
#define ST7735_IPF_18BIT	0x06
#define ST7735_IPF_NOUSED	0x07

/**
 * Read ID1.
 * No arguments.
 * Answer:
 * 	1) 8 bit integer - low byte of display ID
 */
#define ST7735_CMD_RDID1		0xDA

/**
 * Read ID1.
 * No arguments.
 * Answer:
 * 	1) 8 bit integer - mid byte of display ID
 */
#define ST7735_CMD_RDID2		0xDB

/**
 * Read ID3.
 * No arguments.
 * Answer:
 * 	1) 8 bit integer - high byte of display ID
 */
#define ST7735_CMD_RDID3		0xDC


#endif /* ST7735_CMDS_H */
