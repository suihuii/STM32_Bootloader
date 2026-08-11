#include <stddef.h>
#include <stdint.h>

#include "bsp_crc.h"
#include "crc.h"


static uint32_t bsp_crc_pack_word(const uint8_t *data)
{
    return ((uint32_t)data[0] << 24U) |
           ((uint32_t)data[1] << 16U) |
           ((uint32_t)data[2] << 8U)  |
           ((uint32_t)data[3]);
}

bootloader_status_t bsp_crc_calculate(
    const uint8_t *data,
    bootloader_size_t length_bytes,
    bootloader_crc32_t *out_crc)
{
    if ((data == NULL) || (out_crc == NULL))
    {
        return BOOTLOADER_STATUS_INVALID_ARGUMENT;
    }

    if (length_bytes == 0U)
    {
        return BOOTLOADER_STATUS_INVALID_LENGTH;
    }

    bootloader_crc32_t crc_value = 0U;

    /*
    * 先处理所有完整的 4-byte word。
    *
    * 例如 length_bytes = 10：
    *
    * 0~3   -> 第一个 word
    * 4~7   -> 第二个 word
    * 8~9   -> 留到后面补齐
    */
    bootloader_size_t full_length =
        length_bytes - (length_bytes % 4U);

    for (bootloader_size_t offset = 0U;
        offset < full_length;
        offset += 4U)
    {
        uint32_t word =
            bsp_crc_pack_word(&data[offset]);

        if (offset == 0U)
        {
            /*
            * 第一个 word：
            * HAL_CRC_Calculate 会重新开始一次 CRC 计算。
            */
            crc_value =
                HAL_CRC_Calculate(&hcrc, &word, 1U);
        }
        else
        {
            /*
            * 后续 word：
            * 在前一个 CRC 结果基础上继续计算。
            */
            crc_value =
                HAL_CRC_Accumulate(&hcrc, &word, 1U);
        }
    }

    /*
    * 如果最后还剩 1~3 byte，
    * 使用 0xFF 补齐成一个完整的 4-byte word。
    */
    bootloader_size_t remaining =
        length_bytes - full_length;

    if (remaining != 0U)
    {
        uint8_t tail[4] =
        {
            0xFFU,
            0xFFU,
            0xFFU,
            0xFFU
        };

        for (bootloader_size_t index = 0U;
            index < remaining;
            index++)
        {
            tail[index] = data[full_length + index];
        }

        uint32_t word =
            bsp_crc_pack_word(tail);

        /*
        * 如果总数据连 4 byte 都不到，
        * 那这个补齐后的 word 就是第一个 word，
        * 必须使用 Calculate。
        *
        * 否则前面已经计算过完整 word，
        * 这里使用 Accumulate。
        */
        if (full_length == 0U)
        {
            crc_value =
                HAL_CRC_Calculate(&hcrc, &word, 1U);
        }
        else
        {
            crc_value =
                HAL_CRC_Accumulate(&hcrc, &word, 1U);
        }
    }

    *out_crc = crc_value;

    return BOOTLOADER_STATUS_OK;
}