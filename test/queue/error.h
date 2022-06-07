#ifndef __ERROR_H__
#define __ERROR_H__
#include <spmcq/spmcq.h>

#define ERR_HANDLE_EX(msg, en)	\
	do { fprintf(stderr, "[Error] "); errno = en; perror(msg); exit(EXIT_FAILURE); } while (0);

#define ERR_HANDLE(msg)  \
	do { fprintf(stderr, "[Error] "); perror(msg); exit(EXIT_FAILURE); } while (0);

#define ERR_HANDLE_PRINT(fmt, ...)	\
	do { fprintf(stderr, "[Error] "); fprintf(stderr, fmt, ##__VA_ARGS__); exit(EXIT_FAILURE); } while (0);

/* Error handling for parsing command */
#define CHECK_NUM_OUT_OF_RANGE(opt, val, min, max)                                    \
        if (val < min || val > max) {                                                 \
            ERR_HANDLE_PRINT("Value of \"%c\" is out of range (max: %d, min: %d)\n",  \
                             (int)opt, max, min);                                     \
        }

#define CHECK_NUM_IS_POWER_OF_2(opt, val)                                             \
        if (!IS_POWER_OF_2(val)) {                                                    \
            ERR_HANDLE_PRINT("Value of \"%c\" is not power of 2\n", (int)opt)         \
        }

#endif
