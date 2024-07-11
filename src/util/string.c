/* Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved. */

#include <string.h>

char* strcpy(char* s, const char* ct)
{
    char* pdest = s;
    do
    {
        *pdest = *ct;
        if (!*pdest)
            break;
    } while (++ct, ++pdest);
    return s;
}

char* strcat(char* s, const char* ct)
{
    char* pdest = s;
    while (*pdest)
        ++pdest;
    strcpy(pdest, ct);
    return s;
}

int strcmp(const char* cs, const char* ct)
{
    for (; *cs && *ct; ++cs, ++ct)
    {
        if (*cs < *ct)
            return -1;
        if (*cs > *ct)
            return 1;
    }
    if (*cs != *ct && !*cs)
        return -1;
    if (*cs != *ct && !*ct)
        return 1;
    return 0;
}

size_t strlen(const char* cs)
{
    int len;
    for (len = 0; *cs; ++cs)
        ++len;
    return len;
}

void* memcpy(void* s, const void* ct, size_t n)
{
    char*       pdest;
    const char* psrc;
    for (pdest = s, psrc = ct; n; --n, ++pdest, ++psrc)
        *pdest = *psrc;
    return s;
}

int memcmp(const void* cs, const void* ct, size_t n)
{
    const char *pct, *pcs;
    for (pcs = cs, pct = ct; n; --n, ++pcs, ++pct)
    {
        if (*pcs < *pct)
            return -1;
        if (*pcs > *pct)
            return 1;
    }
    return 0;
}

void* memset(void* s, unsigned char c, size_t n)
{
    unsigned char* pdest = (unsigned char*)s;
    while (n--)
        *pdest++ = c;
    return s;
}
