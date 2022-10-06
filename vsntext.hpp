#ifndef _VNSTEXT_H
#define _VNSTEXT_H

#define VSNPRINT(retval, fmt) \
    const int DEFAULT_POOL_SIZE = 1024; \
    char chpool[DEFAULT_POOL_SIZE]; \
    int bigSize = DEFAULT_POOL_SIZE - 1; \
    char* pool; \
    va_list argl; \
    do { \
	pool = (bigSize < DEFAULT_POOL_SIZE) ? chpool : (new char[bigSize + 1]); \
    	va_start(argl, fmt); \
    	int status = vsnprintf(pool, bigSize + 1, fmt, argl); \
	va_end(argl); \
        if ((status == -1) || (status > bigSize)) { \
	    bigSize = ((status > 0) ? status : (bigSize * 2)) + 1; \
	    if (pool != chpool) delete[] pool; \
	    pool = nullptr; \
	} \
    } while (pool == nullptr); \
    std::string retval(pool); \
    if (pool != chpool) delete[] pool;

#endif

