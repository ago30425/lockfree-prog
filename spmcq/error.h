#ifndef __ERROR_H__
#define __ERROR_H__

#define ERR_HANDLE_EX(MSG, EN)	\
	do { errno = EN; perror(MSG); exit(EXIT_FAILURE); } while (0);

#define ERR_HANDLE(MSG)  \
	do { perror(MSG); exit(EXIT_FAILURE); } while (0);

#define ERR_HANDLE_PRINT(FMT, ...)	\
	do { fprintf(stderr, FMT, ##__VA_ARGS__); exit(EXIT_FAILURE); } while (0);

#endif
