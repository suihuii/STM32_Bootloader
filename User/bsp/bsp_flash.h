#ifndef BSP_FLASH_H
#define BSP_FLASH_H

#include <stdint.h>

#include "bootloader_status.h"
#include "bootloader_types.h"

/*
* 解锁Flash
* 
* 执行擦除或写入操作前必须先解锁。
*/
bootloader_status_t bsp_flash_unlock(void);

/*
* 锁定 Flash。
* 
* Flash 操作完成后重新锁定，
* 防止程序意外修改 Flash。
*/
bootloader_status_t bsp_flash_lock(void);

/*
* 擦除指定 Flash 地址范围。
*
* address:
*   要擦除区域的起始地址。
*
* length：
*   要擦除的数据范围，单位为字节。
*
* 注意：
* STM32F407 的 Flash 实际按照 Sector 擦除，
* BSP 内部负责把地址数据范围转换成 Sector。
*/
bootloader_status_t bsp_flash_erase(
    bootloader_addr_t address,
    bootloader_size_t length
);

/*
* 向 Flash 写入一段数据。
*
* address：
*   待写入地址。
*
* data：
*   待写入的数据
* 
* length：
*   数据长度，单位为字节
*/
bootloader_status_t bsp_flash_program(
    bootloader_addr_t address,
    const uint8_t *data,
    bootloader_size_t length
);

#endif /* BSP_FLASH_H*/
