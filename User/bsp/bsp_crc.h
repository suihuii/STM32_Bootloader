#ifndef BSP_CRC_H
#define BSP_CRC_H

#include <stdint.h>

#include "bootloader_types.h"
#include "bootloader_status.h"

/*
* CRC BSP 对外接口。
* 上层模块以后只能通过这里使用 CRC 功能。
* 不直接访问 STM32 HAL 的 CRC 句柄。
*/

/*
* 计算一段字节数据的 CRC32。
*
* data：           输入数据
* length_bytes：   数据长度，单位为字节
* out_crc          返回计算得到的 CRC32   
*/

bootloader_status_t bsp_crc_calculate(
    const uint8_t *data,
    bootloader_size_t length_bytes,
    bootloader_crc32_t *out_crc
);

#endif /* BSP_CRC_H*/
