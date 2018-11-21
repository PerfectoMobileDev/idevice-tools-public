#ifndef COMMON_H
#define COMMON_H

#define PLIST_LITTLE_ENDIAN 0
#define PLIST_BIG_ENDIAN 1

#ifndef PLIST_BYTE_ORDER
#if __BIG_ENDIAN__ == 1
#define PLIST_BYTE_ORDER PLIST_BIG_ENDIAN
#endif
#if __LITTLE_ENDIAN__ == 1
#define PLIST_BYTE_ORDER PLIST_LITTLE_ENDIAN
#endif
#endif

#if defined(__GNUC__) && (__GNUC__ >= 4) && !defined(__CYGWIN__) && !defined(WIN32)
# define _PLIST_INTERNAL      __attribute__((visibility("hidden")))
#elif defined(__SUNPRO_C) && (__SUNPRO_C >= 0x550)
# define _PLIST_INTERNAL      __hidden
#else /* not gcc >= 4 and not Sun Studio >= 8 */
# define _PLIST_INTERNAL
#endif /* GNUC >= 4 */

#if defined(HAVE__STRDUP)
#if !defined(HAVE_STRDUP) || defined(_WIN32)
#undef strdup
#define strdup		_strdup
#endif
#endif

#ifdef WIN32
#include <time.h>

struct timeval {
	long    tv_sec;         /* seconds */
	long    tv_usec;        /* and microseconds */
};

#else
#include <sys/time.h>
#endif


#endif
