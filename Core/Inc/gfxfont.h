// Font structures for newer Adafruit_GFX (1.1 and later).
// Example fonts are included in 'Fonts' directory.
// To use a font in your Arduino sketch, #include the corresponding .h
// file and pass address of GFXfont struct to setFont().  Pass NULL to
// revert to 'classic' fixed-space bitmap font.

// Modified by Chernov A.A. <valexlin@gmail.com> (2018-2021)
// Added field bitmapSize to struct GFXfont.
// Added the ability to include multiple character ranges in one font file.

#ifndef _GFXFONT_H_
#define _GFXFONT_H_

#include <stdint.h>

typedef struct { // Data stored PER GLYPH
	uint16_t bitmapOffset;     // Pointer into GFXfont->bitmap
	uint8_t  width, height;    // Bitmap dimensions in pixels
	uint8_t  xAdvance;         // Distance to advance cursor (x axis)
	int8_t   xOffset, yOffset; // Dist from cursor pos to UL corner
} GFXglyph;

typedef struct {
	uint32_t first;
	uint32_t last;
} GFXglyphRange;

typedef struct { // Data stored for FONT AS A WHOLE:
	const uint8_t  *bitmap;			// Glyph bitmaps, concatenated
	const GFXglyph *glyph;			// Glyph array
	const GFXglyphRange* ranges;	// array of the code points ranges
	uint8_t   rangesCount;			// count of the the code points ranges
	uint16_t  charsCount;			// characters count
	uint8_t   yAdvance;				// Newline distance (y axis)
	uint16_t  bitmapSize;			// Size of Glyph bitmaps
} GFXfont;

#endif // _GFXFONT_H_
