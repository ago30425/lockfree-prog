#ifndef __COMPILER_H__
#define __COMPILER_H__

#ifdef __GNUC__
# define ATTRIBUTE(a) __attribute__ ((a))
#else
# define ATTRIBUTE(a)
#endif

#endif
