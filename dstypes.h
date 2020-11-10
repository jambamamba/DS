#ifndef DSTYPES_H
#define DSTYPES_H

#ifdef UNICODE
typedef wchar_t char2_t;
typedef wchar_t char_t;
#define TXT(t) L##t
#define strlenA strlen
#define strlenW wcslen
#define strcpyW wcscpy
#define strcatW wcscat
#define strcmpW wcscmp
#define strcmpA strcmp
#define strcmpiW _wcsicmp
#define strchrW wcschr
#define atoiW _wtoi
#define atofW _wtof

#else
typedef unsigned short char2_t;
typedef char char_t;
#define TXT(t) t
#define strlenW strlen
#define strlenA strlen
#define strcpyW strcpy
#define strcatW strcat
#define strcmpA strcmp
#define strcmpW strcmp
#define strchrW strchr
#define strcmpiW strcmpi
#define atoiW atoi
#endif
typedef unsigned long long long64;

#endif
