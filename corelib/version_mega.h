
#pragma once
#ifdef KALL
#if defined(__arm__)
#define CPUTYPE "arm"
#elif defined(__avr32__)
#define CPUTYPE "avr32"
#elif defined(__i386__)
#define CPUTYPE "i386"
#elif defined(__ia64__)
#define CPUTYPE "ia64"
#elif defined(__hppa__)
#define CPUTYPE "hppa"
#elif defined(__powerpc64__)
#define CPUTYPE "powerpc64"
#elif defined(__mips__)
#elif defined(__powerpc__)
#define CPUTYPE "powerpc"
#elif defined(__s390__)
#define CPUTYPE "s390"
#elif defined(__sparc__)
#define CPUTYPE "sparc"
#elif defined(__x86_64__)
#define CPUTYPE "x86-64"
#elif defined(__alpha__)
#define CPUTYPE "alpha"
#elif defined(__sh__)
#define CPUTYPE "sh"
#else
//#error "Unknown CPU architecture."
#endif

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#if defined(__GNUC__)
#if defined(__GNUC_PATCHLEVEL__)
#define COMPILER CPUTYPE "_" "GCC" " " STRINGIFY(__GNUC__) "_" STRINGIFY(__GNUC_MINOR__) "_" STRINGIFY(__GNUC_PATCHLEVEL__)
#else
#define COMPILER CPUTYPE "_" "GCC" " " STRINGIFY(__GNUC__) "_" STRINGIFY(__GNUC_MINOR__)
#endif
#else
#define COMPILER "Unknown"
#endif
#endif
#define COREVERSION 0x00000300


