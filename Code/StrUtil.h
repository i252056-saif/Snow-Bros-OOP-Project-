#pragma once
#include <ctime>
#include "Constants.h"


//Basic string operations

inline int strLen(const char* s) {
    int n = 0;
    while (s[n])
        ++n;
    return n;
}

inline void strCopy(char* dst, const char* src, int maxLen = MAX_STR) {
    int i = 0;
    while (src[i] && i < maxLen - 1)
    {
        dst[i] = src[i];
        ++i;
    }
    dst[i] = '\0';
}

inline void strAppend(char* dst, const char* src, int maxLen = MAX_STR) {
    int dlen = strLen(dst);
    int i = 0;
    while (src[i] && dlen + i < maxLen - 1)
    {
        dst[dlen + i] = src[i]; 
        ++i; 
    }
    dst[dlen + i] = '\0';
}

inline bool strEq(const char* a, const char* b) {
    int i = 0;
    while (a[i] && b[i]) 
    { if (a[i] != b[i]) return false; ++i; }
    return a[i] == b[i];
}

inline void strClear(char* s, int len = MAX_STR) {
    for (int i = 0; i < len; ++i) s[i] = '\0';
}

// Integer to string

inline void intToStr(char* dst, int val, int maxLen = MAX_STR) {
    int neg = 0;
    if (val < 0) 
    {
        neg = 1; 
        val = -val; 
    }
    char temp[32];
    int len = 0;
    if (val == 0)
    {
        temp[0] = '0';
        len = 1; 
    }
    else 
    {
        while (val > 0) 
        {
        temp[len++] = (char)('0' + val % 10);
        val /= 10; 
        }
    }
    int pos = 0;
    if (neg && pos < maxLen - 1) dst[pos++] = '-';
    for (int i = len - 1; i >= 0 && pos < maxLen - 1; --i) dst[pos++] = temp[i];
    dst[pos] = '\0';
}

inline int strToInt(const char* s) {
    int val = 0, neg = 0, i = 0;
    if (s[0] == '-') 
    { 
        neg = 1; i = 1;
    }
    for (; s[i] >= '0' && s[i] <= '9'; ++i)
    {
        val = val * 10 + (s[i] - '0');
    }
    return neg ? -val : val;
}

inline void fmtInt(char* dst, const char* prefix, int val, int maxLen = MAX_STR) {
    strCopy(dst, prefix, maxLen);
    char num[32]; intToStr(num, val);
    strAppend(dst, num, maxLen);
}

inline void fmtFraction(char* dst, const char* prefix, int a, int b, int maxLen = MAX_STR)
{
    strCopy(dst, prefix, maxLen);
    char tmp[32];
    intToStr(tmp, a);
    strAppend(dst, tmp, maxLen);
    strAppend(dst, "/", maxLen);
    intToStr(tmp, b); strAppend(dst, tmp, maxLen);
}

inline void padNumber(char* dst, int width, int val) 
{
    char temp[32];
    int len = 0;
    if (val == 0)
    {
        temp[0] = '0'; len = 1;
    }
    else 
    { 
        while (val > 0) 
        {
            temp[len++] = (char)('0' + val % 10);
            val /= 10;
        }
    }
    int pos = 0;
    for (int i = len; i < width; ++i) dst[pos++] = '0';
    for (int i = len - 1; i >= 0; --i) dst[pos++] = temp[i];
    dst[pos] = '\0';
}

inline void currentDate(char* out, int maxLen = MAX_STR) {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    int year = lt->tm_year + 1900, month = lt->tm_mon + 1, day = lt->tm_mday;
    char ys[5], ms[3], ds[3];
    padNumber(ys, 4, year); padNumber(ms, 2, month); padNumber(ds, 2, day);
    strCopy(out, ys, maxLen);
    strAppend(out, "-", maxLen);
    strAppend(out, ms, maxLen);
    strAppend(out, "-", maxLen);
    strAppend(out, ds, maxLen);
}

inline void padRight(char* dst, const char* src, int width, int maxLen = MAX_STR) {
    strCopy(dst, src, maxLen);
    int len = strLen(dst);
    while (len < width && len < maxLen - 1) dst[len++] = ' ';
    dst[len] = '\0';
}

//  zero count bytes at ptr
inline void zeroBytes(void* ptr, int count) {
    char* p = (char*)ptr;
    for (int i = 0; i < count; ++i) p[i] = 0;
}
