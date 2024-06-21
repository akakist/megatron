#cmake_minimum_required (VERSION 2.6)

add_library(zlib STATIC

..//adler32.c
..//compress.c
..//crc32.c
..//deflate.c
..//gzclose.c
..//gzlib.c
..//gzread.c
..//gzwrite.c
..//infback.c
..//inffast.c
..//inflate.c
..//inftrees.c
..//trees.c
..//uncompr.c
..//zutil.c
)
#install(TARGETS madison_lib_wz DESTINATION ${LIB_TARGET_DIR})

