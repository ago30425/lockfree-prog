#ifndef __ARCH_H__
#define __ARCH_H__

#if defined(__x86_64__) || defined(__i386__)
#   define CACHE_LINE_SIZE   64
#else
#   error "The architecture is not supported"
#endif

#endif
