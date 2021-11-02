/***************************************************************************
 *   Copyright (C) 2021 by Chernov A.A.                                    *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef UTF8_PROC_H
#define UTF8_PROC_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief myutf8_strlen
 * @param[in] str
 * @return length of UTF-8 string
 */
size_t myutf8_strlen(const char* str);

/**
 * @brief myutf8_codePoint Retrives the code point from first characters of the null-terminated utf-8 string.
 * @param[out] codePoint
 * @param[in] str
 * @return size of retrived code point in utf-8 representaion on success, -1 on error.
 */
int myutf8_codePoint(uint32_t* codePoint, const char* str);

/**
 * @brief myutf8_codePointAt Retrives the code point at the given index of the null-terminated utf-8 string.
 * @param[out] codePoint pointer to destination
 * @param[in] str null-terminated utf-8 string
 * @param[in] index
 * @return size of retrived code point in utf-8 representaion on success, -1 on error.
 */
int myutf8_codePointAt(uint32_t* codePoint, const char* str, size_t index);

/**
 * @brief myutf8_toChars Converts the specified character (Unicode code point) to its UTF-8 representation stored in a char array
 * @param[out] dest
 * @param[in] dest_sz
 * @param[in] codePoint
 * @return count of copied bytes
 *
 * This function don't write null-terminator '\0'!
 */
int myutf8_toChars(char* dest, size_t dest_sz, uint32_t codePoint);

#ifdef __cplusplus
}
#endif

#endif // UTF8_PROC_H
