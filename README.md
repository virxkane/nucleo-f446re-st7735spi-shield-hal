# nucleo-f446re-st7735spi-shield-hal
Demo firmware for [LCD shield for STM32 Nucleo-64 boards](https://github.com/virxkane/nucleo64-lcd-st7735).

This demo program uses HAL to access the STM32 core and peripherals.

No libraries are used to work with graphics (including font rendering), all the code is written from scratch.

Bitmap fonts were prepared using the [fontconvert](https://github.com/virxkane/fontconvert) utility, which is a modification of the corresponding utility from the https://github.com/adafruit/Adafruit-GFX-Library project. This modified utility, unlike the original, allows you to save glyphs in a bitmap font not from one range, but from several. Moreover, when writing character codes, a 32-bit integer is used, i.e. any unicode characters can be included in the generated font. Due to the ability to include characters from several ranges in one font file, characters from several languages can be embedded in it, eliminating unnecessary gaps. This avoids creating multiple files of the same font for each language and switching between them.
