#ifndef ____CRC_16__H
#define ____CRC_16__H
//#include <stdint.h>
#ifndef _MSC_VER
#include <inttypes.h>
#endif
#include <stddef.h>
#include <stdint.h>

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string>

/** CRC table for the CRC-16. The poly is 0x8005 (x^16 + x^15 + x^2 + 1) */
/*inline unsigned short __crc16(const unsigned char* data_p, unsigned char length){
    unsigned char x;
    unsigned short crc = 0xFFFF;

    while (length--){
        x = crc >> 8 ^ *data_p++;
        x ^= x>>4;
        crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x <<5)) ^ ((unsigned short)x);
    }
    return crc;
}*/
uint16_t
__crc16(uint16_t crc, const void *buf, size_t size);

#endif
