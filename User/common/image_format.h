#ifndef IMAGE_FORMAT_H
#define IMAGE_FORMAT_H

#include <stdint.h>

#include "bootloader_types.h"

/*
* 固件镜像格式的公共定义。
*
* bootloader 和上位机都需要按照同一格式理解固件信息，
* 例如镜像是否合法，镜像大小以及CRC32 校验值。
*/

/*
* 镜像魔数（Magic Number）
*
* 用于快速判断收到的数据是否看起来像我们的 bootloader镜像。
* 0x424C5631 对应 ASCII 字符:
*
* 0x42 = 'B'
* 0x4C = 'L'
* 0x56 = 'V'
* 0x31 = '1'
*
* 即 “BLV1”: bootloader Version 1。
*/

#define BOOTLOADER_IMAGE_MAGIC \
    UINT32_C(0x424C5631)

/*
* 当前镜像格式版本。
*
* 注意：这是“镜像文件格式”的版本，
*
* 不是Application 软件自身的业务版本。
*/
#define BOOTLOADER_IMAGE_FORMAT_VERSION \
    UINT32_C(1)

/*
* 固件镜像头。
*
* 用来描述一份Application 固件的基本信息
* Bootloader 在擦除和写入Flash 前会检查这些信息。
*/
typedef struct
{
    /* 固定魔数：确认这是我们定义的镜像格式*/
    uint32_t magic;

    /* 镜像格式版本：确认Bootloader能否解析这种格式*/
    uint32_t format_version;

    /* 固件计划写入的 Flash 起始地址*/
    bootloader_addr_t target_address;

    /* 本次Application 固件的实际大小，单位：字节*/
    bootloader_size_t image_size;

    /* Application 固件内容对应的期望 CRC32 */
    bootloader_crc32_t image_crc32;
} bootloader_image_header_t;

/*
 * 镜像头由 5 个 32 位字段组成。
 *
 * 每个字段 4 字节，因此当前 V1 镜像头必须固定为 20 字节。
 * 如果以后有人修改结构体导致尺寸变化，编译器会直接报错，
 * 避免 MCU 和 Python 上位机对镜像格式产生不同理解。
 */
_Static_assert(
    sizeof(bootloader_image_header_t) == 20U,
    "Unexpected bootloader image header size"
);

#endif
