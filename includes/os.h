
#ifdef _WIN32
    #define OS_WIN32
    #define strcmpi    _strcmpi
#endif

#ifndef _WIN32
    #define OS_UNIX
#endif

#ifdef __LINUX__
    #define OS_LINUX
    char strcmpi(char *a, char *b);
#endif
