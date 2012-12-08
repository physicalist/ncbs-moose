#ifndef MY_SUNDRY_DECLS_H
#define MY_SUNDRY_DECLS_H

extern "C" void CmiAbort(const char *why);
extern "C" void CmiPrintf(const char *fmt, ...);
extern "C" void CmiError(const char *fmt, ...);
extern "C" void *CmiTmpAlloc(int size);
extern "C" void CmiTmpFree(void *p);
extern "C" void __cmi_assert(const char *expr, const char *file, int line);

#endif // MY_SUNDRY_DECLS_H
