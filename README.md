# nucleo-f446re-st7735spi-shield-hal
Demo firmware for [LCD shield for STM32 Nucleo-64 boards](https://github.com/virxkane/nucleo64-lcd-st7735).

This demo program uses HAL to access the STM32 core and peripherals.

No libraries are used to work with graphics (including font rendering), all the code is written from scratch.

Bitmap fonts were prepared using the [fontconvert](https://github.com/virxkane/fontconvert) utility, which is a modification of the corresponding utility from the https://github.com/adafruit/Adafruit-GFX-Library project. This modified utility, unlike the original, allows you to save glyphs in a bitmap font not from one range, but from several. Moreover, when writing character codes, a 32-bit integer is used, i.e. any unicode characters can be included in the generated font. Due to the ability to include characters from several ranges in one font file, characters from several languages can be embedded in it, eliminating unnecessary gaps. This avoids creating multiple files of the same font for each language and switching between them.

#### Compare performance
| # |  bpp variant  | fill period, ms | transfer period, CMSIS, ms | transfer period, HAL, ms |
|---|---------------|--------|-----------|---------|
| 1 | fb=1, lcd=12  |     3  |      23   |    40   |
| 2 | fb=1, lcd=16  |     3  |      30   |    60   |
| 3 | fb=1, lcd=18  |     3  |      44   |    74   |
| 4 | fb=4, lcd=12  |     3  |      23   |    39   |
| 5 | fb=4, lcd=16  |     3  |      30   |    58   |
| 6 | fb=4, lcd=18  |     3  |      44   |    73   |
| 7 | fb=8, lcd=12  |     2  |      23   |    39   |
| 8 | fb=8, lcd=16  |     2  |      30   |    59   |
| 9 | fb=8, lcd=18  |     2  |      44   |    73   |
| 10 | fb=16, lcd=12 |    3  |      22   |    39   |
| 11 | fb=16, lcd=16 |    3  |      30   |    59   |
| 12 | fb=16, lcd=18 |    3  |      44   |    75   |
| 13 | fb=24, lcd=12 |    3  |      22   |    38   |
| 14 | fb=24, lcd=16 |    3  |      30   |    61   |
| 15 | fb=24, lcd=18 |    3  |      44   |    73   |

Where *fb* - frame buffer color depth, *lcd* - LCD module color depth, *fill period* - frame buffer fill period, *transfer period* - period of frame buffer transmission to LCD module.

**CMSIS** variant - [nucleo-f446re-st7735spi-shield](https://github.com/virxkane/nucleo-f446re-st7735spi-shield)  
**HAL** variant - this project.

Tests performed at SPI clock 11,25 Mbits/s.
