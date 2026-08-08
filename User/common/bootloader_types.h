#ifndef BOOTLOADER_TYPES_H
#define BOOTLOADER_TYPES_H

#include <stdint.h>

typedef uint32_t bootloader_addr_t;
typedef uint32_t bootloader_size_t;
typedef uint32_t bootloader_crc32_t;
typedef uint32_t bootloader_time_ms_t;

typedef struct
{
    bootloader_addr_t start_addr;
    bootloader_size_t size;
} bootloader_memory_region_t;


#endif
