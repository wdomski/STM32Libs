/*
 * eprintfs.h
 *
 *  Created on: 24 Mar 2022
 *      Author: Wojciech Domski
 */

#ifndef EPRINTFS_HEADER
#define EPRINTFS_HEADER
/**
 * @brief Compute integer exponentiation
 *
 * @param base Base value
 * @param power Exponent
 * @return @p base raised to the power of @p power
 */
unsigned int epower(unsigned int base, unsigned int power);

/**
 * @brief Count the number of decimal digits in an integer
 *
 * @param number Input value (sign is ignored)
 * @return Number of decimal digits (at least 1)
 */
unsigned int edigits(int number);

/**
 * @brief Compute length of a null-terminated string
 *
 * @param str Input string (must not be NULL)
 * @return Number of characters before the terminating null byte
 */
int estrlen(const char *str);

/**
 * @brief Copy up to @p len characters from @p str into @p out
 *
 * The output buffer is always null-terminated.
 *
 * @param str Source string (must not be NULL)
 * @param out Destination buffer (must not be NULL)
 * @param len Maximum number of characters to copy
 * @return Number of characters actually copied (excluding null terminator)
 */
int estrncpy(const char *str, char *out, unsigned int len);

/**
 * @brief Copy a null-terminated string from @p str into @p out
 *
 * The output buffer is always null-terminated.
 *
 * @param str Source string (must not be NULL)
 * @param out Destination buffer (must not be NULL, must be large enough)
 * @return Number of characters copied (excluding null terminator)
 */
int estrcpy(const char *str, char *out);

/**
 * @brief Compare at most @p num characters of two strings
 *
 * @param str1 First string (must not be NULL)
 * @param str2 Second string (must not be NULL)
 * @param num Maximum number of characters to compare
 * @return 0 if equal, positive if @p str1 > @p str2, negative otherwise
 */
int estrncmp(const char *str1, const char *str2, unsigned int num);

/**
 * @brief Convert a long integer to a null-terminated string
 *
 * Supports bases 2-16. A leading '-' is prepended for negative values only
 * when @p base is 10, otherwise the value is treated as unsigned.
 *
 * @param val Value to convert
 * @param str Output buffer (must be large enough, up to 65 chars + null)
 * @param base Numeric base for conversion (2-16)
 * @return Pointer to the beginning of @p str
 */
char *eitoa(long int val, char *str, int base);

/**
 * @brief Convert a string to an integer in the given base
 *
 * Parses an optional leading '+' or '-' followed by digits in the given
 * base. Supported bases: 2-10 and 16. For base 16, digits a-f / A-F are
 * accepted.
 *
 * @param str Input string (must not be NULL)
 * @param base Numeric base (2-10 or 16)
 * @param stop If not NULL, set to point past the last parsed character
 * @return Parsed integer value
 */
int eatoi(char *str, int base, char **stop);

/**
 * @brief Convert a string to a floating-point number
 *
 * Parses an optional sign, integer part, and optional fractional part
 * separated by '.'. Uses @c eatoi internally for each part.
 *
 * @param str Input string (must not be NULL)
 * @param stop If not NULL, set to point past the last parsed character
 * @return Parsed float value
 */
float eatof(char *str, char **stop);

/**
 * @brief Formatted output to a string (subset of standard sprintf)
 *
 * Supported format specifiers: %d/%i, %f, %x/%X, %c, %s, %%.
 * Supports '0' and space padding, width (including '*'), precision
 * (including '*'), and length modifiers h/hh/l/ll.
 *
 * @param str Output buffer (must be large enough for the result)
 * @param format Format string
 * @param ... Additional arguments matching format specifiers
 * @return Number of characters written (excluding null terminator)
 */
int esprintf(char *str, const char *format, ...);

/**
 * @brief Formatted input from a string (subset of standard sscanf)
 *
 * Supported format specifiers: %d/%i, %u, %f, %x/%X, %c, %%.
 * Supports length modifiers h/hh/l/ll.
 *
 * @param str Input string to parse
 * @param format Format string
 * @param ... Pointers to variables to receive parsed values
 * @return Number of input items successfully matched and assigned
 */
int esscanf(char *str, const char *format, ...);

/**
 * @brief Locate the first occurrence of a character in a string
 *
 * @param str Input string
 * @param character Character to find (converted to char)
 * @return Pointer to the first occurrence, or NULL if not found
 */
const char *estrchr(const char *str, int character);

/**
 * @brief Find the n-th occurrence of a character in a string
 *
 * @param str Input buffer
 * @param character Character to find
 * @param n Occurrence number to find (1-based)
 * @return const char*
 *  NULL if the character was not found @p n times.
 *  Pointer to the n-th occurrence of @p character in @p str.
 */
const char *estrnchr(const char *str, int character, int n);

/**
 * @brief Find the first occurrence of a substring within a string
 *
 * @param str1 String to search in
 * @param str2 Substring to search for
 * @return Pointer to the beginning of the match in @p str1, or NULL if
 *         @p str2 is not found
 */
char *estrstr(char *str1, const char *str2);

/**
 * @brief Find a character searching from the right (end) of a string
 *
 * @param[in]  str      String to search in
 * @param[in]  len      String length
 * @param[in]  sign     Character to look for
 * @param[out] position Position of the character in the string;
 *                      not modified if the character is not found.
 *
 * @return Pointer to the rightmost (last) occurrence of @p sign in @p str.
 *         Returns NULL if the character was not found.
 */
char *efind_char_right(char *str, int len, const char sign, int *position);

#endif
