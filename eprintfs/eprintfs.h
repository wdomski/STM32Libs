/*
 * eprintfs.h
 *
 *  Created on: 24 Mar 2022
 *      Author: Wojciech Domski
 */

#ifndef EPRINTFS_HEADER
#define EPRINTFS_HEADER

int eabs(int val);

float efabs(float val);

unsigned int epower(unsigned int base, unsigned int power);

unsigned int edigits(int number);

int estrlen(const char *str);

int estrncpy(const char *str, char *out, unsigned int len);

int estrncmp(const char *str1, const char *str2, unsigned int num);

char *eitoa(long int val, char *str, int base);

int eatoi(char *str, int base, char **stop);

float eatof(char *str, char **stop);

int esprintf(char *str, const char *format, ...);

int esscanf(char *str, const char *format, ...);

const char *estrchr(const char *str, int character);

char * estrstr (char * str1, const char * str2);

/**
 * find a character from right side
 *
 * \param[in]  str      String to search in
 * \param[in]  length   String length
 * \param[in]  sign     Character to look for
 * \param[out] position Position of the character sign in the string str, 
 *                      if character is not found, it is not modified.
 *
 * \return     Returns address of first occurrence of the character in the
 *             string.
 *             Returns NULL is character was not found
 */
char *efind_char_right(char *str, int len, const char sign, int * position);

#endif
