#ifndef ERROR_H
#define ERROR_H

typedef enum {
	SUCCESS,
	ERR_ALLOC,
	ERR_WADPARSE,
	ERR_MAX
} Error;

extern const char* ErrorStrings[];

#endif
