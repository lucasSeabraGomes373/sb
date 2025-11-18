#ifndef BYTE_TYPES_H
#define BYTE_TYPES_H

#include <stdint.h>

typedef uint8_t byte1;     // 1 byte (8 bits)
typedef uint16_t byte2;    // 2 bytes (16 bits)
typedef uint32_t byte4;    // 4 bytes (32 bits)

#define MAX_BYTE1 255           // Maximum value for uint8_t
#define MAX_BYTE2 65535         // Maximum value for uint16_t
#define MAX_BYTE4 2147483647    // Maximum value for uint32_t

#endif // BYTE_TYPES_H