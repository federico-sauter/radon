/* Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved. */

#if !defined(_RADON_SYS_STRING__H_)
#define _RADON_SYS_STRING__H_

typedef unsigned int size_t;

/* Function descriptions taken from: The C Programming Language 2nd ed, by
 * Kerningham & Ritchie
 */

/*  copy string ct to string s, including '\0'; return s  */
char* strcpy(char* s, const char* ct);

/*  concatenate string ct to end of string s; return s  */
char* strcat(char* s, const char* ct);

/*  compare string cd to string ct; return <0 if cs<ct, o if cs==ct, or >0 if
 * cs>ct */
int strcmp(const char* cs, const char* ct);

/*  return length of cs */
size_t strlen(const char* cs);

/*  copy n characters from ct to s, and return s    */
void* memcpy(void* s, const void* ct, size_t n);

#define memmove(x, y, z) memcpy(x, y, z)

/*  compare the first n characters of cs with ct; return as with strcmp */
int memcmp(const void* cs, const void* ct, size_t n);

/*  place character c into first n characters of s, return s  */
void* memset(void* s, unsigned char c, size_t n);

#endif
