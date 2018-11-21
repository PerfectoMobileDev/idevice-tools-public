#ifndef __COMMON_INCLUDE_H
#define __COMMON_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(HAVE__STRDUP)
#if !defined(HAVE_STRDUP) || defined(_WIN32)
#undef strdup
#define strdup		_strdup
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif //__COMMON_INCLUDE_H