/*
 * st7735_hwspi.h
 *
 *  Created on: Oct 26, 2019
 *      Author: alex
 */

#ifndef SRC_ST7735_HALSPI_H_
#define SRC_ST7735_HALSPI_H_

#include <stdint.h>

#include "stm32f4xx_hal.h"
#include "rgbcolor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* st7735_handle;


/**
 * @brief Initialize LCD display based on ST7735 controller via HAL.
 * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
 *               the configuration information for SPI module.
 * @param gpio_cs pointer to a GPIO_TypeDef structure that contains
 *               information about GPIO port with CS connection.
 * @param gpio_cs_pin_msk Mask for CS pin.
 * @param gpio_dc pointer to a GPIO_TypeDef structure that contains
 *               information about GPIO port with D/CX (A0 or RS) connection.
 * @param gpio_dc_pin_msk Mask for D/CX (A0 or RS) pin.
 * @param width Screen width.
 * @param height Screen height.
 * @param ipf Interface Pixel Format, one of ST7735_IPF_12BIT, ST7735_IPF_16BIT, ST7735_IPF_18BIT.
 * @return handle for display connection if successful, 0 if error occurred.
 */
st7735_handle st7735_init_hal(SPI_HandleTypeDef* hspi, GPIO_TypeDef* gpio_cs, uint16_t gpio_cs_pin_msk,
		GPIO_TypeDef* gpio_dc, uint16_t gpio_dc_pin_msk, uint8_t width, uint8_t height, uint8_t ipf);

st7735_handle st7735_init_hal_defs(SPI_HandleTypeDef* hspi, GPIO_TypeDef* gpio_cs, uint16_t gpio_cs_pin_msk,
		GPIO_TypeDef* gpio_dc, uint16_t gpio_dc_pin_msk);

/**
 * @brief Free resources for LCD connection.
 * @param  handle Handle to ST7735 connection.
 * @return None.
 */
void st7735_free(st7735_handle handle);

/**
 * @brief Clear screen by filling each pixel with color color
 * @param  handle Handle to ST7735 connection.
 * @param color array with color data.
 * @return 0 if operation completed successfully, -1 if error occurred.
 */
int8_t st7735_cls(st7735_handle handle, RGBColor color);

/**
 * @brief Return current display pixel format.
 * @param  handle Handle to ST7735 connection.
 * @return display pixel format, one of ST7735_IPF_12BIT, ST7735_IPF_16BIT, ST7735_IPF_16BIT.
 */
int8_t st7735_getipf(st7735_handle handle);

/**
 * @brief Set display pixel format.
 * @param  handle Handle to ST7735 connection.
 * @param ipf display pixel format, one of ST7735_IPF_12BIT, ST7735_IPF_16BIT, ST7735_IPF_16BIT.
 * @return 0 if operation completed successfully, -1 if error occurred.
 */
int8_t st7735_setipf(st7735_handle handle, uint8_t ipf);

/**
 * @brief Set graphical area for memory write operation.
 * @param  handle Handle to ST7735 connection.
 * @param xs x address start
 * @param ys y address start
 * @param xe x address end
 * @param ye y address end
 * @return 0 if operation completed successfully, -1 if error occurred.
 */
int8_t st7735_setWindow(st7735_handle handle, int16_t xs, int16_t ys, int16_t xe, int16_t ye);

/**
 * @brief Put pixel into given coordinates.
 */
int8_t st7735_setPixel(st7735_handle handle, int16_t x, int16_t y, RGBColor color);

/**
 * @brief Draw black and white (1bpp) bitmap.
 * @param handle
 * @param x
 * @param y
 * @param w
 * @param h
 * @param bitmap_data - bitmap's data, one bit per pixel.
 * @param bitmap_sz size of bitmap data.
 * @param palette array with minimum two elements: index 0 - for black color, index 1 - for white color of bitmap_data.
 */
int8_t st7735_drawBitmap_1bpp(st7735_handle handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz, const RGBColor palette[]);

/**
 * @brief Draw 4bpp bitmap.
 * @param handle
 * @param x
 * @param y
 * @param w
 * @param h
 * @param bitmap_data bitmap's data, one bit per pixel.
 * @param bitmap_sz size of bitmap data.
 * @param palette array with palette colors with size 16.
 */
int8_t st7735_drawBitmap_4bpp(st7735_handle handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz, const RGBColor palette[]);

/**
 * @brief Draw 8bpp bitmap.
 * @param handle
 * @param x
 * @param y
 * @param w
 * @param h
 * @param bitmap_data bitmap's data, one bit per pixel.
 * @param bitmap_sz size of bitmap data.
 * @param palette array with palette colors with size 256.
 */
int8_t st7735_drawBitmap_8bpp(st7735_handle handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz, const RGBColor palette[]);

/**
 * @brief Draw 16bpp bitmap.
 * @param handle
 * @param x
 * @param y
 * @param w
 * @param h
 * @param bitmap_data bitmap's data, one bit per pixel.
 * @param bitmap_sz size of bitmap data.
 */
int8_t st7735_drawBitmap_16bpp(st7735_handle handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz);

/**
 * @brief Draw 24bpp bitmap.
 * @param handle
 * @param x
 * @param y
 * @param w
 * @param h
 * @param bitmap_data bitmap's data, one bit per pixel.
 * @param bitmap_sz size of bitmap data.
 */
int8_t st7735_drawBitmap_24bpp(st7735_handle handle, int16_t x, int16_t y, uint16_t w, uint16_t h,
		const uint8_t* bitmap_data, uint16_t bitmap_sz);

/**
 * @brief Write command without arguments to display controller.
 * @param  handle Handle to ST7735 connection.
 * @param cmd Command to controller that not have any arguments.
 * @return 0 if operation completed successfully, -1 if error occurred.
 */
int8_t st7735_wrcmd(st7735_handle handle, uint8_t cmd);

/**
 * @brief Write command without arguments to display controller.
 * @param  handle Handle to ST7735 connection.
 * @param cmd Command to controller
 * @param args Pointer to array with command's arguments.
 * @param args_sz size of arguments args in bytes.
 * @return 0 if operation completed successfully, -1 if error occurred.
 */
int8_t st7735_wrcmd_args(st7735_handle handle, uint8_t cmd, uint8_t *args, uint16_t args_sz);

/**
 * @brief Write command to read data from display controller.
 * @param  handle Handle to ST7735 connection.
 * @param cmd Command to controller
 * @param outdata Pointer to array where returned data must be saved.
 * @param outdata_sz Size of outdata.
 * @return Number of bytes received from controller if operation completed successfully, -1 if error occurred.
 */
int8_t st7735_rdcmd(st7735_handle handle, uint8_t cmd, uint8_t *outdata, uint8_t outdata_sz);

#ifdef __cplusplus
}
#endif

#endif /* SRC_ST7735_HALSPI_H_ */
