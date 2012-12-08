#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

// copied from opening section of ckstandalone.C

/* CkPrintf/CkAbort support: */
extern "C" void CmiAbort(const char *why) {
        fprintf(stderr,"Fatal error> %s\n",why);
        abort();
}

extern "C" void CmiPrintf(const char *fmt, ...) {
        va_list p; va_start(p, fmt);
        vfprintf(stdout,fmt,p);
        va_end(p);
}
extern "C" void CmiError(const char *fmt, ...) {
        va_list p; va_start(p, fmt);
        vfprintf(stderr,fmt,p);
        va_end(p);
}

extern "C" void *CmiTmpAlloc(int size) {return malloc(size);}
extern "C" void CmiTmpFree(void *p) {free(p);}
extern "C" void __cmi_assert(const char *expr, const char *file, int line) { CmiAbort(expr);}

