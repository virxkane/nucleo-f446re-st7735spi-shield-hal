/*
 * st7735_hwspi.c
 *
 *  Created on: Oct 26, 2019
 *      Author: alex
 */

#include "st7735_halspi.h"
#include "st7735_cmds.h"

#include <stdlib.h>

#pragma pack(push, 1)
struct _st7735_handle
{
	SPI_HandleTypeDef *hspi;
	GPIO_TypeDef *gpio_cs;
	GPIO_TypeDef *gpio_dc;
	uint32_t _1us_delay_cnt;
	uint32_t _1ms_delay_cnt;
	uint32_t _10ms_delay_cnt;
	uint16_t gpio_cs_pin_msk;
	uint16_t gpio_dc_pin_msk;
	uint8_t ipf;
	uint8_t width;
	uint8_t height;
};
#pragma pack(pop)

#define _SET_CS_LOW(handle)		{ HAL_GPIO_WritePin(handle->gpio_cs, handle->gpio_cs_pin_msk, GPIO_PIN_RESET); }
#define _SET_CS_HIGH(handle)	{ HAL_GPIO_WritePin(handle->gpio_cs, handle->gpio_cs_pin_msk, GPIO_PIN_SET); }
#define _SET_DC_LOW(handle)		{ HAL_GPIO_WritePin(handle->gpio_dc, handle->gpio_dc_pin_msk, GPIO_PIN_RESET); }
#define _SET_DC_HIGH(handle)	{ HAL_GPIO_WritePin(handle->gpio_dc, handle->gpio_dc_pin_msk, GPIO_PIN_SET); }

static inline int8_t _st7735_wrcmd(struct _st7735_handle* handle, uint8_t cmd)
{
	_SET_CS_LOW(handle);
	// set D/CX to 0 - next byte is command
	_SET_DC_LOW(handle);
	HAL_StatusTypeDef ret = HAL_SPI_Transmit(handle->hspi, &cmd, 1, 1);
	_SET_CS_HIGH(handle);
	return HAL_OK == ret ? 0 : -1;
}

static inline int8_t _st7735_wrcmd_args(struct _st7735_handle* handle, uint8_t cmd, uint8_t *args, uint16_t args_sz)
{
	_SET_CS_LOW(handle);
	// set D/CX to 0 - next byte is command
	_SET_DC_LOW(handle);
	HAL_StatusTypeDef ret = HAL_SPI_Transmit(handle->hspi, &cmd, 1, 1);
	if (HAL_OK == ret)
	{
		// set D/CX to 1 - next bytes is data
		_SET_DC_HIGH(handle);
		ret = HAL_SPI_Transmit(handle->hspi, args, args_sz, args_sz);
	}
	_SET_CS_HIGH(handle);
	return HAL_OK == ret ? 0 : -1;
}

static inline int8_t _st7735_rdcmd(struct _st7735_handle* handle, uint8_t cmd, uint8_t *outdata, uint8_t outdata_sz)
{
	_SET_CS_LOW(handle);
	_SET_DC_LOW(handle);
	HAL_StatusTypeDef ret = HAL_SPI_Transmit(handle->hspi, &cmd, 1, 1);
	if (HAL_OK == ret)
	{
		// TODO: first bit must is ignored
		ret = HAL_SPI_Receive(handle->hspi, outdata, outdata_sz, 1);
#if 0
		uint8_t tail;
		if (HAL_OK == ret)
			ret = HAL_SPI_Receive(handle->hspi, &tail, 1, 1);
		if (HAL_OK == ret)
		{
			for (int i = 0; i < outdata_sz - 1; i++)
			{
				outdata[i] <<= 1;
				outdata[i] |= outdata[i + 1] >> 7;
			}
			outdata[outdata_sz - 1] <<= 1;
			outdata[outdata_sz - 1] |= tail >> 7;
		}
#endif
	}
	_SET_CS_HIGH(handle);
	return HAL_OK == ret ? 0 : -1;
}

static inline void _RGBColorToNativeColor(uint8_t* dstcolor, RGBColor rgbcolor, uint8_t ipf)
{
	switch (ipf)
	{
	case ST7735_IPF_12BIT:
		// not applicable here!
		break;
	case ST7735_IPF_16BIT:
		RGBColor2RGB565(dstcolor, rgbcolor);
		break;
	default:
	case ST7735_IPF_18BIT:
		RGBColor2RGB888(dstcolor, rgbcolor);
		break;
	}
}



st7735_handle st7735_init_hal(SPI_HandleTypeDef* hspi, GPIO_TypeDef* gpio_cs, uint16_t gpio_cs_pin_msk,
		GPIO_TypeDef* gpio_dc, uint16_t gpio_dc_pin_msk, uint8_t width, uint8_t height, uint8_t ipf)
{
	uint8_t args[3];
	struct _st7735_handle* handle = (struct _st7735_handle*)malloc(sizeof(struct _st7735_handle));
	if (!handle)
		return 0;
	handle->hspi = hspi;
	handle->gpio_cs = gpio_cs;
	handle->gpio_cs_pin_msk = gpio_cs_pin_msk;
	handle->gpio_dc = gpio_dc;
	handle->gpio_dc_pin_msk = gpio_dc_pin_msk;

	// timings
	handle->_1us_delay_cnt = SystemCoreClock/4000000;	// us * (SystemCoreClock / 1000000) / 4
	if (0 == handle->_1us_delay_cnt)
		handle->_1us_delay_cnt = 1;
	handle->_1ms_delay_cnt = SystemCoreClock/4000;		// us * (SystemCoreClock / 1000000) / 4
	if (0 == handle->_1ms_delay_cnt)
		handle->_1ms_delay_cnt = 1;
	handle->_10ms_delay_cnt = SystemCoreClock/400;
	if (0 == handle->_10ms_delay_cnt)
		handle->_10ms_delay_cnt = 1;

	// Software reset
	int8_t ret = _st7735_wrcmd(handle, ST7735_CMD_SWRESET);
	if (0 == ret)
	{
		// wait 120 ms.
		HAL_Delay(125);
		// Sleep Out
		ret = _st7735_wrcmd(handle, ST7735_CMD_SLPOUT);
	}
	if (0 == ret)
	{
		// wait 120 ms.
		HAL_Delay(125);
		args[0] = 0x00;
		ret = _st7735_wrcmd_args(handle, ST7735_CMD_MADCTL, args, 1);
	}
	if (0 == ret)
	{
		args[0] = ipf;
		ret = _st7735_wrcmd_args(handle, ST7735_CMD_COLMOD, args, 1);
	}
	if (0 == ret)
	{
		// clear RAM
		handle->width = width;
		handle->height = height;
		ret = st7735_cls(handle, COLOR_BLACK);
	}
	if (0 == ret)
	{
		handle->ipf = ipf;
		// Display ON
		ret = _st7735_wrcmd(handle, ST7735_CMD_DISPON);
	}
	if (0 != ret)
	{
		free(handle);
		handle = 0;
	}
	return handle;
}

st7735_handle st7735_init_hal_defs(SPI_HandleTypeDef* hspi, GPIO_TypeDef* gpio_cs, uint16_t gpio_cs_pin_msk,
		GPIO_TypeDef* gpio_dc, uint16_t gpio_dc_pin_msk)
{
	return st7735_init_hal(hspi, gpio_cs, gpio_cs_pin_msk, gpio_dc, gpio_dc_pin_msk, 128, 160, ST7735_IPF_18BIT);
}

void st7735_free(st7735_handle handle)
{
	if (handle)
		free(handle);
}

int8_t st7735_cls(st7735_handle handle, RGBColor rgbcolor)
{
	if (!handle)
		return -1;
	struct _st7735_handle* _handle = (struct _st7735_handle*)handle;
	uint8_t ipfcolor[3];
	uint8_t ret = 0;
	uint8_t args[4];
	uint16_t i;
	args[0] = 0;
	args[1] = 0;
	args[2] = (uint8_t)((_handle->width - 1) >> 8);
	args[3] = (uint8_t)((_handle->width - 1) & 0xFF);
	ret = _st7735_wrcmd_args(_handle, ST7735_CMD_CASET, args, 4);
	if (0 == ret)
	{
		args[0] = 0;
		args[1] = 0;
		args[2] = (uint8_t)((_handle->height - 1) >> 8);
		args[3] = (uint8_t)((_handle->height - 1) & 0xFF);
		ret = _st7735_wrcmd_args(_handle, ST7735_CMD_RASET, args, 4);
	}
	if (0 == ret)
	{
		// set CS to 0
		_SET_CS_LOW(_handle);
		_SET_DC_LOW(_handle);
		uint8_t cmd = ST7735_CMD_RAMWR;
		ret = (HAL_SPI_Transmit(_handle->hspi, &cmd, 1, 1) == HAL_OK) ? 0 : -1;
		// set D/CX to 1 - next bytes is data
		_SET_DC_HIGH(_handle);
		if (ST7735_IPF_12BIT == _handle->ipf)
		{
			// |RRRRGGGG|BBBBRRRR|GGGGBBBB|
#if RGBCOLOR_AS_NUMBER==1
			ipfcolor[0] = (RGB_GET_R(rgbcolor) & 0xF0) | (RGB_GET_G(rgbcolor) >> 4);
			ipfcolor[1] = (RGB_GET_B(rgbcolor) & 0xF0) | (RGB_GET_R(rgbcolor) >> 4);
			ipfcolor[2] = (RGB_GET_G(rgbcolor) & 0xF0) | (RGB_GET_B(rgbcolor) >> 4);
#else
			ipfcolor[0] = (rgbcolor.r & 0xF0) | (rgbcolor.g >> 4);
			ipfcolor[1] = (rgbcolor.b & 0xF0) | (rgbcolor.r >> 4);
			ipfcolor[2] = (rgbcolor.g & 0xF0) | (rgbcolor.b >> 4);
#endif
		}
		else
			_RGBColorToNativeColor(ipfcolor, rgbcolor, _handle->ipf);
		uint8_t color_write_sz = _handle->ipf == ST7735_IPF_16BIT ? 2 : 3;
		if (ST7735_IPF_12BIT == _handle->ipf)
		{
			for (i = 0; i < _handle->width*_handle->height/2; i++)
			{
				ret = (HAL_SPI_Transmit(_handle->hspi, ipfcolor, color_write_sz, 3) == HAL_OK) ? 0 : -1;
				if (0 != ret)
					break;
			}
		}
		else
		{
			for (i = 0; i < _handle->width*_handle->height; i++)
			{
				ret = (HAL_SPI_Transmit(_handle->hspi, ipfcolor, color_write_sz, 3) == HAL_OK) ? 0 : -1;
				if (0 != ret)
					break;
			}
		}
		// set CS to 1
		_SET_CS_HIGH(_handle);
	}
	return ret;
}

int8_t st7735_getipf(st7735_handle handle)
{
	if (!handle)
		return -1;
	struct _st7735_handle* _handle = (struct _st7735_handle*)handle;
	return _handle->ipf;
}

int8_t st7735_setipf(st7735_handle handle, uint8_t ipf)
{
	if (!handle)
		return -1;
	struct _st7735_handle* _handle = (struct _st7735_handle*)handle;
	int8_t ret = _st7735_wrcmd_args(_handle, ST7735_CMD_COLMOD, &ipf, 1);
	if (0 == ret)
		_handle->ipf = ipf;
	return ret;
}

int8_t st7735_setWindow(st7735_handle handle, int16_t xs, int16_t ys, int16_t xe, int16_t ye)
{
	if (!handle)
		return -1;
	struct _st7735_handle* _handle = (struct _st7735_handle*)handle;
	int8_t ret;
	uint8_t args[4];
	args[0] = (uint8_t)((uint16_t)xs >> 8);
	args[1] = (uint8_t)((uint16_t)xs & 0xFF);
	args[2] = (uint8_t)((uint16_t)xe >> 8);
	args[3] = (uint8_t)((uint16_t)xe & 0xFF);
	ret = _st7735_wrcmd_args(_handle, ST7735_CMD_CASET, args, 4);
	if (0 == ret)
	{
		args[0] = (uint8_t)((uint16_t)ys >> 8);
		args[1] = (uint8_t)((uint16_t)ys & 0xFF);
		args[2] = (uint8_t)((uint16_t)ye >> 8);
		args[3] = (uint8_t)((uint16_t)ye & 0xFF);
		ret = _st7735_wrcmd_args(_handle, ST7735_CMD_RASET, args, 4);
	}
	return ret;
}

int8_t st7735_setPixel(st7735_handle handle, int16_t x, int16_t y, RGBColor rgbcolor)
{
	int8_t ret = -1;
	struct _st7735_handle* _handle = (struct _st7735_handle*)handle;
	uint8_t ipfcolor[3];
	if (ST7735_IPF_12BIT == _handle->ipf)
	{
		// TODO: Read sibling pixel, combine with this and write back
	}
	else
	{
		ret = st7735_setWindow(handle, x, y, x + 1, y + 1);
		if (0 == ret)
		{
			_RGBColorToNativeColor(ipfcolor, rgbcolor, _handle->ipf);
			ret = _st7735_wrcmd_args(_handle, ST7735_CMD_RAMWR, ipfcolor, _handle->ipf == ST7735_IPF_18BIT ? 3 : 2);
		}
	}
	return ret;
}

static int8_t _st7735_drawBitmap_1bpp_12ipf(struct _st7735_handle* _handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz, const RGBColor palette[])
{
	int8_t ret = st7735_setWindow(_handle, x, y, x + w - 1, y + h - 1);
	if (0 == ret)
	{
		uint16_t i;
		int8_t j;
		uint8_t pixel_data[3];
		const uint8_t* bptr = bitmap_data;

		_SET_CS_LOW(_handle);
		_SET_DC_LOW(_handle);
		uint8_t cmd = ST7735_CMD_RAMWR;
		HAL_StatusTypeDef status = HAL_SPI_Transmit(_handle->hspi, &cmd, 1, 1);
		if (HAL_OK == status)
		{
			uint8_t color_index;
			// set D/CX to 1 - next bytes is data
			_SET_DC_HIGH(_handle);
			for (i = 0; i < ((w*h) >> 3) && (i < bitmap_sz); i++)
			{
				for (j = 7; j >= 0; j--)
				{
					color_index = (*bptr >> j) & 0x01;
					if (j & 0x01)
					{
						// prepare left/low half-pixel
#if RGBCOLOR_AS_NUMBER==1
						pixel_data[0] = (RGB_GET_R(palette[color_index]) & 0xF0) | (RGB_GET_G(palette[color_index]) >> 4);
						pixel_data[1] = RGB_GET_B(palette[color_index]) & 0xF0;
#else
						pixel_data[0] = (palette[color_index].r & 0xF0) | (palette[color_index].g >> 4);
						pixel_data[1] = palette[color_index].b & 0xF0;
#endif
					}
					else
					{
						// prepare right/high half-pixel
#if RGBCOLOR_AS_NUMBER==1
						pixel_data[1] |= RGB_GET_R(palette[color_index]) >> 4;
						pixel_data[2] = (RGB_GET_G(palette[color_index]) & 0xF0) | (RGB_GET_B(palette[color_index]) >> 4);
#else
						pixel_data[1] |= palette[color_index].r >> 4;
						pixel_data[2] = (palette[color_index].g & 0xF0) | (palette[color_index].b >> 4);
#endif
						// write pair of pixels to RAM
						status = HAL_SPI_Transmit(_handle->hspi, pixel_data, 3, 3);
						if (HAL_OK != status) {
							ret = -1;
							break;
						}
					}
				}
				if (0 != ret)
					break;
				bptr++;
			}
		}
		else
			ret = -1;
		// set CS to 1
		_SET_CS_HIGH(_handle);
	}
	return ret;
}


int8_t st7735_drawBitmap_1bpp(st7735_handle handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz, const RGBColor palette[])
{
	struct _st7735_handle* _handle = (struct _st7735_handle*)handle;
	if (w > _handle->width - x)
		w = _handle->width - x;
	if (h > _handle->height - y)
		h = _handle->height - y;
	if (ST7735_IPF_12BIT == _handle->ipf)
		return _st7735_drawBitmap_1bpp_12ipf(_handle, x, y, w, h, bitmap_data, bitmap_sz, palette);
	int8_t ret = st7735_setWindow(handle, x, y, x + w - 1, y + h - 1);
	if (0 == ret)
	{
		uint16_t i;
		int8_t j;
		static uint8_t pal_ipfcolor[2][3];
		const uint8_t* bptr = bitmap_data;

		_RGBColorToNativeColor(pal_ipfcolor[0], palette[0], _handle->ipf);
		_RGBColorToNativeColor(pal_ipfcolor[1], palette[1], _handle->ipf);

		_SET_CS_LOW(_handle);
		_SET_DC_LOW(_handle);
		// start transmission
		uint8_t cmd = ST7735_CMD_RAMWR;
		HAL_StatusTypeDef status = HAL_SPI_Transmit(_handle->hspi, &cmd, 1, 1);
		if (HAL_OK == status)
		{
			uint8_t color_sz;
			uint8_t color_index;
			// set D/CX to 1 - next bytes is data
			_SET_DC_HIGH(_handle);
			color_sz = _handle->ipf == ST7735_IPF_18BIT ? 3 : 2;
			for (i = 0; i < ((w*h) >> 3) && (i < bitmap_sz); i++)
			{
				for (j = 7; j >= 0; j--)
				{
					color_index = (*bptr >> j) & 0x01;
					// write pixel to RAM
					status = HAL_SPI_Transmit(_handle->hspi, pal_ipfcolor[color_index], color_sz, color_sz);
					if (HAL_OK != status) {
						ret = -1;
						break;
					}
				}
				if (0 != ret)
					break;
				bptr++;
			}
		}
		else
		{
			ret = -1;
		}
		// set CS to 1
		_SET_CS_HIGH(_handle);
	}
	return ret;
}

static int8_t _st7735_drawBitmap_4bpp_12ipf(struct _st7735_handle* _handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz, const RGBColor palette[])
{
	int8_t ret = st7735_setWindow(_handle, x, y, x + w - 1, y + h - 1);
	if (0 == ret)
	{
		uint16_t i;
		uint8_t pixel_data[3];
		const uint8_t* bptr = bitmap_data;

		_SET_CS_LOW(_handle);
		_SET_DC_LOW(_handle);
		uint8_t cmd = ST7735_CMD_RAMWR;
		HAL_StatusTypeDef status = HAL_SPI_Transmit(_handle->hspi, &cmd, 1, 2);
		if (HAL_OK == status)
		{
			uint8_t color_index;
			// set D/CX to 1 - next bytes is data
			_SET_DC_HIGH(_handle);
			for (i = 0; i < ((w*h) >> 1) && (i < bitmap_sz); i++)
			{
				// high nibble of pixel data
				color_index = *bptr >> 4;
#if RGBCOLOR_AS_NUMBER==1
				pixel_data[0] = (RGB_GET_R(palette[color_index]) & 0xF0) | (RGB_GET_G(palette[color_index]) >> 4);
				pixel_data[1] = RGB_GET_B(palette[color_index]) & 0xF0;
#else
				pixel_data[0] = (palette[color_index].r & 0xF0) | (palette[color_index].g >> 4);
				pixel_data[1] = palette[color_index].b & 0xF0;
#endif
				// low nibble of pixel data
				color_index = *bptr & 0x0F;
#if RGBCOLOR_AS_NUMBER==1
				pixel_data[1] |= RGB_GET_R(palette[color_index]) >> 4;
				pixel_data[2] = (RGB_GET_G(palette[color_index]) & 0xF0) | (RGB_GET_B(palette[color_index]) >> 4);
#else
				pixel_data[1] |= palette[color_index].r >> 4;
				pixel_data[2] = (palette[color_index].g & 0xF0) | (palette[color_index].b >> 4);
#endif
				// write pair of pixels to RAM
				status = HAL_SPI_Transmit(_handle->hspi, pixel_data, 3, 2);
				if (HAL_OK != status) {
					ret = -1;
					break;
				}
				bptr++;
			}
		}
		else
			ret = -1;
		// set CS to 1
		_SET_CS_HIGH(_handle);
	}
	return ret;
}

int8_t st7735_drawBitmap_4bpp(st7735_handle handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz, const RGBColor palette[])
{
	struct _st7735_handle* _handle = (struct _st7735_handle*)handle;
	if (w > _handle->width - x)
		w = _handle->width - x;
	if (h > _handle->height - y)
		h = _handle->height - y;
	if (ST7735_IPF_12BIT == _handle->ipf)
		return _st7735_drawBitmap_4bpp_12ipf(_handle, x, y, w, h, bitmap_data, bitmap_sz, palette);
	int8_t ret = st7735_setWindow(handle, x, y, x + w - 1, y + h - 1);
	if (0 == ret)
	{
		uint16_t i;
		uint8_t k;
		static uint8_t pal_ipfcolor[16][3];
		const uint8_t* bptr = bitmap_data;

		for (k = 0; k < 16; k++)
			_RGBColorToNativeColor(pal_ipfcolor[k], palette[k], _handle->ipf);

		_SET_CS_LOW(_handle);
		_SET_DC_LOW(_handle);
		uint8_t cmd = ST7735_CMD_RAMWR;
		HAL_StatusTypeDef status = HAL_SPI_Transmit(_handle->hspi, &cmd, 1, 1);
		if (HAL_OK == status)
		{
			uint8_t color_sz;
			uint8_t color_index;
			// set D/CX to 1 - next bytes is data
			_SET_DC_HIGH(_handle);
			color_sz = _handle->ipf == ST7735_IPF_18BIT ? 3 : 2;
			for (i = 0; i < ((w*h) >> 1) && (i < bitmap_sz); i++)
			{
				// high nibble of pixel data
				color_index = *bptr >> 4;
				// write pixel to RAM
				status = HAL_SPI_Transmit(_handle->hspi, pal_ipfcolor[color_index], color_sz, 2);
				if (HAL_OK != status)
				{
					ret = -1;
					break;
				}
				// low nibble of pixel data
				color_index = *bptr & 0x0F;
				// write pixel to RAM
				status = HAL_SPI_Transmit(_handle->hspi, pal_ipfcolor[color_index], color_sz, 2);
				if (HAL_OK != status)
				{
					ret = -1;
					break;
				}
				bptr++;
			}
		}
		else
			ret = -1;
		// set CS to 1
		_SET_CS_HIGH(_handle);
	}
	return ret;
}

static int8_t _st7735_drawBitmap_8bpp_12ipf(struct _st7735_handle* _handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz, const RGBColor palette[])
{
	int8_t ret = st7735_setWindow(_handle, x, y, x + w - 1, y + h - 1);
	if (0 == ret)
	{
		uint16_t i;
		uint8_t pixel_data[3];
		const uint8_t* bptr = bitmap_data;

		_SET_CS_LOW(_handle);
		_SET_DC_LOW(_handle);
		uint8_t cmd = ST7735_CMD_RAMWR;
		HAL_StatusTypeDef status = HAL_SPI_Transmit(_handle->hspi, &cmd, 1, 2);
		if (HAL_OK == status)
		{
			uint8_t color_index;
			// set D/CX to 1 - next bytes is data
			_SET_DC_HIGH(_handle);
			for (i = 0; i < (w*h) && (i < bitmap_sz); i += 2)
			{
				// high nibble of pixel data
				color_index = *bptr;
#if RGBCOLOR_AS_NUMBER==1
				pixel_data[0] = (RGB_GET_R(palette[color_index]) & 0xF0) | (RGB_GET_G(palette[color_index]) >> 4);
				pixel_data[1] = RGB_GET_B(palette[color_index]) & 0xF0;
#else
				pixel_data[0] = (palette[color_index].r & 0xF0) | (palette[color_index].g >> 4);
				pixel_data[1] = palette[color_index].b & 0xF0;
#endif
				bptr++;
				// low nibble of pixel data
				color_index = *bptr;
#if RGBCOLOR_AS_NUMBER==1
				pixel_data[1] |= RGB_GET_R(palette[color_index]) >> 4;
				pixel_data[2] = (RGB_GET_G(palette[color_index]) & 0xF0) | (RGB_GET_B(palette[color_index]) >> 4);
#else
				pixel_data[1] |= palette[color_index].r >> 4;
				pixel_data[2] = (palette[color_index].g & 0xF0) | (palette[color_index].b >> 4);
#endif
				// write pair of pixels to RAM
				status = HAL_SPI_Transmit(_handle->hspi, pixel_data, 3, 2);
				if (HAL_OK != status) {
					ret = -1;
					break;
				}
				bptr++;
			}
		}
		else
			ret = -1;
		// set CS to 1
		_SET_CS_HIGH(_handle);
	}
	return ret;
}

int8_t st7735_drawBitmap_8bpp(st7735_handle handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz, const RGBColor palette[])
{
	struct _st7735_handle* _handle = (struct _st7735_handle*)handle;
	if (w > _handle->width - x)
		w = _handle->width - x;
	if (h > _handle->height - y)
		h = _handle->height - y;
	if (ST7735_IPF_12BIT == _handle->ipf)
		return _st7735_drawBitmap_8bpp_12ipf(_handle, x, y, w, h, bitmap_data, bitmap_sz, palette);
	int8_t ret = st7735_setWindow(handle, x, y, x + w - 1, y + h - 1);
	if (0 == ret)
	{
		uint16_t i;
		uint16_t k;
		static uint8_t pal_ipfcolor[256][3];
		const uint8_t* bptr = bitmap_data;

		for (k = 0; k < 256; k++)
			_RGBColorToNativeColor(pal_ipfcolor[k], palette[k], _handle->ipf);

		_SET_CS_LOW(_handle);
		_SET_DC_LOW(_handle);
		uint8_t cmd = ST7735_CMD_RAMWR;
		HAL_StatusTypeDef status = HAL_SPI_Transmit(_handle->hspi, &cmd, 1, 1);
		if (HAL_OK == status)
		{
			uint8_t color_sz;
			uint8_t color_index;
			// set D/CX to 1 - next bytes is data
			_SET_DC_HIGH(_handle);
			color_sz = _handle->ipf == ST7735_IPF_18BIT ? 3 : 2;
			for (i = 0; (i < w*h) && (i < bitmap_sz); i++)
			{
				// write pixel data
				color_index = *bptr;
				// write pixel to RAM
				status = HAL_SPI_Transmit(_handle->hspi, pal_ipfcolor[color_index], color_sz, 2);
				if (HAL_OK != status)
				{
					ret = -1;
					break;
				}
				bptr++;
			}
		}
		else
			ret = -1;
		// set CS to 1
		_SET_CS_HIGH(_handle);
	}
	return ret;
}

static int8_t _st7735_drawBitmap_16bpp_12ipf(struct _st7735_handle* _handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz)
{
	int8_t ret = st7735_setWindow(_handle, x, y, x + w - 1, y + h - 1);
	if (0 == ret)
	{
		uint16_t i;
		uint8_t pixel_data[3];
		const uint8_t* bptr = bitmap_data;

		_SET_CS_LOW(_handle);
		_SET_DC_LOW(_handle);
		uint8_t cmd = ST7735_CMD_RAMWR;
		HAL_StatusTypeDef status = HAL_SPI_Transmit(_handle->hspi, &cmd, 1, 2);
		if (HAL_OK == status)
		{
			uint8_t r, g, b;
			// set D/CX to 1 - next bytes is data
			_SET_DC_HIGH(_handle);
			for (i = 0; i < ((w*h) << 1) && (i < bitmap_sz); i += 4)
			{
				r = *bptr & 0xF8;
				g = (*bptr & 0x07) << 5;
				bptr++;
				g |= ((*bptr & 0xE0) >> 3);
				b = (*bptr & 0x1F) << 3;
				// high nibble of pixel data
				pixel_data[0] = (r & 0xF0) | (g >> 4);
				pixel_data[1] = b & 0xF0;
				bptr++;
				// low nibble of pixel data
				r = *bptr & 0xF8;
				g = (*bptr & 0x07) << 5;
				bptr++;
				g |= ((*bptr & 0xE0) >> 3);
				b = (*bptr & 0x1F) << 3;
				pixel_data[1] |= (r >> 4);
				pixel_data[2] = (g & 0xF0) | (b >> 4);
				// write pair of pixels to RAM
				status = HAL_SPI_Transmit(_handle->hspi, pixel_data, 3, 2);
				if (HAL_OK != status) {
					ret = -1;
					break;
				}
				bptr++;
			}
		}
		else
			ret = -1;
		// set CS to 1
		_SET_CS_HIGH(_handle);
	}
	return ret;
}

static int8_t _st7735_drawBitmap_16bpp_18ipf(struct _st7735_handle* _handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz)
{
	int8_t ret = st7735_setWindow(_handle, x, y, x + w - 1, y + h - 1);
	if (0 == ret)
	{
		uint16_t i;
		uint8_t pixel_data[3];
		const uint8_t* bptr = bitmap_data;

		_SET_CS_LOW(_handle);
		_SET_DC_LOW(_handle);
		uint8_t cmd = ST7735_CMD_RAMWR;
		HAL_StatusTypeDef status = HAL_SPI_Transmit(_handle->hspi, &cmd, 1, 2);
		if (HAL_OK == status)
		{
			uint8_t r, g, b;
			// set D/CX to 1 - next bytes is data
			_SET_DC_HIGH(_handle);
			for (i = 0; i < ((w*h) << 1) && (i < bitmap_sz); i += 2)
			{
				r = *bptr & 0xF8;
				g = (*bptr & 0x07) << 5;
				bptr++;
				g |= ((*bptr & 0xE0) >> 3);
				b = (*bptr & 0x1F) << 3;

				pixel_data[0] = r;
				pixel_data[1] = g;
				pixel_data[2] = b;

				// write pixel data to RAM
				status = HAL_SPI_Transmit(_handle->hspi, pixel_data, 3, 2);
				if (HAL_OK != status) {
					ret = -1;
					break;
				}
				bptr++;
			}
		}
		else
			ret = -1;
		// set CS to 1
		_SET_CS_HIGH(_handle);
	}
	return ret;
}

int8_t st7735_drawBitmap_16bpp(st7735_handle handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz)
{
	struct _st7735_handle* _handle = (struct _st7735_handle*)handle;
	if (w > _handle->width - x)
		w = _handle->width - x;
	if (h > _handle->height - y)
		h = _handle->height - y;
	if (ST7735_IPF_12BIT == _handle->ipf)
		return _st7735_drawBitmap_16bpp_12ipf(_handle, x, y, w, h, bitmap_data, bitmap_sz);
	else if (ST7735_IPF_18BIT == _handle->ipf)
		return _st7735_drawBitmap_16bpp_18ipf(_handle, x, y, w, h, bitmap_data, bitmap_sz);
	// For IPF 16
	int8_t ret = st7735_setWindow(handle, x, y, x + w - 1, y + h - 1);
	if (0 == ret)
	{
		uint16_t i;
		const uint8_t* bptr = bitmap_data;

		_SET_CS_LOW(_handle);
		_SET_DC_LOW(_handle);
		uint8_t cmd = ST7735_CMD_RAMWR;
		HAL_StatusTypeDef status = HAL_SPI_Transmit(_handle->hspi, &cmd, 1, 1);
		if (HAL_OK == status)
		{
			// set D/CX to 1 - next bytes is data
			_SET_DC_HIGH(_handle);
			for (i = 0; i < ((w*h) << 1) && (i < bitmap_sz); i += 2)
			{
				// write pixel to RAM
				status = HAL_SPI_Transmit(_handle->hspi, (uint8_t *)bptr, 2, 2);
				if (HAL_OK != status)
				{
					ret = -1;
					break;
				}
				bptr += 2;
			}
		}
		else
			ret = -1;
		// set CS to 1
		_SET_CS_HIGH(_handle);
	}
	return ret;
}

static int8_t _st7735_drawBitmap_24bpp_12ipf(struct _st7735_handle* _handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz)
{
	int8_t ret = st7735_setWindow(_handle, x, y, x + w - 1, y + h - 1);
	if (0 == ret)
	{
		uint16_t i;
		uint8_t pixel_data[3];
		const uint8_t* bptr = bitmap_data;

		_SET_CS_LOW(_handle);
		_SET_DC_LOW(_handle);
		uint8_t cmd = ST7735_CMD_RAMWR;
		HAL_StatusTypeDef status = HAL_SPI_Transmit(_handle->hspi, &cmd, 1, 2);
		if (HAL_OK == status)
		{
			uint8_t r, g, b;
			// set D/CX to 1 - next bytes is data
			_SET_DC_HIGH(_handle);
			for (i = 0; i < (w*h*3) && (i < bitmap_sz); i += 6)
			{
				r = *bptr++;
				g = *bptr++;
				b = *bptr++;
				// high nibble of pixel data
				pixel_data[0] = (r & 0xF0) | (g >> 4);
				pixel_data[1] = b & 0xF0;
				// low nibble of pixel data
				r = *bptr++;
				g = *bptr++;
				b = *bptr++;
				pixel_data[1] |= (r >> 4);
				pixel_data[2] = (g & 0xF0) | (b >> 4);
				// write pair of pixels to RAM
				status = HAL_SPI_Transmit(_handle->hspi, pixel_data, 3, 3);
				if (HAL_OK != status) {
					ret = -1;
					break;
				}
			}
		}
		else
			ret = -1;
		// set CS to 1
		_SET_CS_HIGH(_handle);
	}
	return ret;
}

static int8_t _st7735_drawBitmap_24bpp_18ipf(struct _st7735_handle* _handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz)
{
	int8_t ret = st7735_setWindow(_handle, x, y, x + w - 1, y + h - 1);
	if (0 == ret)
	{
		uint16_t i;
		const uint8_t* bptr = bitmap_data;

		_SET_CS_LOW(_handle);
		_SET_DC_LOW(_handle);
		uint8_t cmd = ST7735_CMD_RAMWR;
		HAL_StatusTypeDef status = HAL_SPI_Transmit(_handle->hspi, &cmd, 1, 2);
		if (HAL_OK == status)
		{
			// set D/CX to 1 - next bytes is data
			_SET_DC_HIGH(_handle);
			for (i = 0; i < (w*h*3) && (i < bitmap_sz); i += 3)
			{
				// write pixel data to RAM
				status = HAL_SPI_Transmit(_handle->hspi, (uint8_t*)bptr, 3, 2);
				if (HAL_OK != status) {
					ret = -1;
					break;
				}
				bptr += 3;
			}
		}
		else
			ret = -1;
		// set CS to 1
		_SET_CS_HIGH(_handle);
	}
	return ret;
}

int8_t st7735_drawBitmap_24bpp(st7735_handle handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz)
{
	struct _st7735_handle* _handle = (struct _st7735_handle*)handle;
	if (w > _handle->width - x)
		w = _handle->width - x;
	if (h > _handle->height - y)
		h = _handle->height - y;
	if (ST7735_IPF_12BIT == _handle->ipf)
		return _st7735_drawBitmap_24bpp_12ipf(_handle, x, y, w, h, bitmap_data, bitmap_sz);
	else if (ST7735_IPF_18BIT == _handle->ipf)
		return _st7735_drawBitmap_24bpp_18ipf(_handle, x, y, w, h, bitmap_data, bitmap_sz);
	// For IPF 16
	int8_t ret = st7735_setWindow(handle, x, y, x + w - 1, y + h - 1);
	if (0 == ret)
	{
		uint16_t i;
		uint8_t pixel_data[2];
		const uint8_t* bptr = bitmap_data;

		_SET_CS_LOW(_handle);
		_SET_DC_LOW(_handle);
		uint8_t cmd = ST7735_CMD_RAMWR;
		HAL_StatusTypeDef status = HAL_SPI_Transmit(_handle->hspi, &cmd, 1, 1);
		if (HAL_OK == status)
		{
			// set D/CX to 1 - next bytes is data
			_SET_DC_HIGH(_handle);
			for (i = 0; i < (w*h*3) && (i < bitmap_sz); i += 3)
			{
				// convert pixel data
				uint8_t r = *bptr++;
				uint8_t g = *bptr++;
				uint8_t b = *bptr++;
				pixel_data[0] = (r & 0xF8) | (g >> 5);
				pixel_data[1] = ((g & 0x1C) << 3) | (b >> 3);
				// write pixel to RAM
				status = HAL_SPI_Transmit(_handle->hspi, pixel_data, 2, 2);
				if (HAL_OK != status)
				{
					ret = -1;
					break;
				}
			}
		}
		else
			ret = -1;
		// set CS to 1
		_SET_CS_HIGH(_handle);
	}
	return ret;
}

int8_t st7735_wrcmd(st7735_handle handle, uint8_t cmd)
{
	if (!handle)
		return -1;
	return _st7735_wrcmd((struct _st7735_handle*)handle, cmd);
}

int8_t st7735_wrcmd_args(st7735_handle handle, uint8_t cmd, uint8_t *args, uint16_t args_sz)
{
	if (!handle)
		return -1;
	return _st7735_wrcmd_args((struct _st7735_handle*)handle, cmd, args, args_sz);
}

int8_t st7735_rdcmd(st7735_handle handle, uint8_t cmd, uint8_t *outdata, uint8_t outdata_sz)
{
	if (!handle)
		return -1;
	return _st7735_rdcmd((struct _st7735_handle*)handle, cmd, outdata, outdata_sz);
}
