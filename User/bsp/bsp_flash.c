#include "bsp_flash.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

#define BSP_FLASH_SECTOR_0_BASE_ADDR    UINT32_C(0x08000000)
#define BSP_FLASH_SECTOR_1_BASE_ADDR    UINT32_C(0x08004000)
#define BSP_FLASH_SECTOR_2_BASE_ADDR    UINT32_C(0x08008000)
#define BSP_FLASH_SECTOR_3_BASE_ADDR    UINT32_C(0x0800C000)
#define BSP_FLASH_SECTOR_4_BASE_ADDR    UINT32_C(0x08010000)
#define BSP_FLASH_SECTOR_5_BASE_ADDR    UINT32_C(0x08020000)
#define BSP_FLASH_SECTOR_6_BASE_ADDR    UINT32_C(0x08040000)
#define BSP_FLASH_SECTOR_7_BASE_ADDR    UINT32_C(0x08060000)
#define BSP_FLASH_SECTOR_8_BASE_ADDR    UINT32_C(0x08080000)
#define BSP_FLASH_SECTOR_9_BASE_ADDR    UINT32_C(0x080A0000)
#define BSP_FLASH_SECTOR_10_BASE_ADDR   UINT32_C(0x080C0000)
#define BSP_FLASH_SECTOR_11_BASE_ADDR   UINT32_C(0x080E0000)

#include "bootloader_config.h"
#include "bootloader_status.h"
#include "bootloader_types.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_flash_ex.h"

static uint32_t bsp_flash_get_sector(
    bootloader_addr_t address)
{
    if (address < 0x08004000U)
    {
        return FLASH_SECTOR_0;
    }

    if (address < 0x08008000U)
    {
        return FLASH_SECTOR_1;
    }

    if (address < 0x0800C000U)
    {
        return FLASH_SECTOR_2;
    }

    if (address < 0x08010000U)
    {
        return FLASH_SECTOR_3;
    }

    if (address < 0x08020000U)
    {
        return FLASH_SECTOR_4;
    }

    if (address < 0x08040000U)
    {
        return FLASH_SECTOR_5;
    }

    if (address < 0x08060000U)
    {
        return FLASH_SECTOR_6;
    }

    if (address < 0x08080000U)
    {
        return FLASH_SECTOR_7;
    }

    if (address < 0x080A0000U)
    {
        return FLASH_SECTOR_8;
    }

    if (address < 0x080C0000U)
    {
        return FLASH_SECTOR_9;
    }

    if (address < 0x080E0000U)
    {
        return FLASH_SECTOR_10;
    }

    return FLASH_SECTOR_11;
}

static bootloader_status_t bsp_flash_validate_range(
    bootloader_addr_t address,
    bootloader_size_t length
)
{
    /*
    * 空范围没有实际 Falsh 操作意义。
    */
    if (length == 0U)
    {
        return BOOTLOADER_STATUS_INVALID_LENGTH;
    }

    /*
    * 起始地址必须位于 Application Flash 区域
    * 
    * 这样可以保护 Bootloader 自己所在区域。
    * 0x08000000 ~ 0x0800FFFF。
    */
    if ((address < APPLICATION_FLASH_BASE_ADDR) ||
        (address >= APPLICATION_FLASH_END_ADDR_EXCLUSIVE))
    {
        return BOOTLOADER_STATUS_OUT_OF_RANGE;
    }

    /*
    * 检查整个 [address, address + length) 范围。
    *
    * 不直接写：
    *   address + length > END
    * 
    * 因为无符号整形加减法存在溢出的可能
    */
    if (length > (APPLICATION_FLASH_END_ADDR_EXCLUSIVE - address))
    {
        return BOOTLOADER_STATUS_OUT_OF_RANGE;
    }

    return BOOTLOADER_STATUS_OK;
}

bootloader_status_t bsp_flash_unlock(void)
{
    HAL_StatusTypeDef hal_status = HAL_FLASH_Unlock();

    if(hal_status != HAL_OK)
    {
        return BOOTLOADER_STATUS_FLASH_ERROR;
    }

    return BOOTLOADER_STATUS_OK;
}

bootloader_status_t bsp_flash_lock(void)
{
    HAL_StatusTypeDef hal_status = HAL_FLASH_Lock();

    if(hal_status != HAL_OK)
    {
        return BOOTLOADER_STATUS_FLASH_ERROR;
    }

    return BOOTLOADER_STATUS_OK;
}

bootloader_status_t bsp_flash_erase(
    bootloader_addr_t address,
    bootloader_size_t length
)
{
    /*
    * 先检查整个擦除范围是否合法。
    * 不允许擦到 Bootloader,也不允许超过 MCU Flash。
    */
    bootloader_status_t status = 
        bsp_flash_validate_range(address,length);

    if (status != BOOTLOADER_STATUS_OK)
    {
        return status;
    }

    /*
    * length 表示字节数量。
    * 
    * 所以最后一个有效地址是：
    * address + length - 1U
    */
    bootloader_addr_t end_address =
        address + length - 1U;
    
    uint32_t first_sector = 
        bsp_flash_get_sector(address);
    
    uint32_t last_sector = 
        bsp_flash_get_sector(end_address);

    /*
    * Sector 编号连续，因此可以直接计算数量。
    *
    * 例如 Sector 4 到 Sector 6：
    * 6 - 4 + 1 = 3 个 Sector。
    */
    uint32_t sector_count = 
        last_sector - first_sector + 1U;

    FLASH_EraseInitTypeDef erase_init = {0};
    uint32_t sector_error = UINT32_MAX;

    erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase_init.Sector = first_sector;
    erase_init.NbSectors = sector_count;
    erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    HAL_StatusTypeDef hal_status = 
        HAL_FLASHEx_Erase(
            &erase_init,
            &sector_error
        );
    
    if (hal_status != HAL_OK)
    {
        return BOOTLOADER_STATUS_FLASH_ERROR;
    }

    return BOOTLOADER_STATUS_OK;

}

bootloader_status_t bsp_flash_program(
    bootloader_addr_t address,
    const uint8_t *data,
    bootloader_size_t length
)
{
    /*
    * data 必须指向有效的待写入数据。
    */
    if (data == NULL)
    {
        return BOOTLOADER_STATUS_INVALID_ARGUMENT;
    }

    /*
    * 检查整个写入范围是否合法。
    * 不允许写入 Bootloader 区域。
    * 也不允许超过 MCU Flash。
    */
    bootloader_status_t status = 
        bsp_flash_validate_range(address,length);
    
    if (status != BOOTLOADER_STATUS_OK)
    {
        return status;
    }

    /*
    * 第一版本采用 byte programming
    *
    * data[offset] 是一个 8-bit 数据，
    * address + offset 是当前 byte 对应的 Flash 地址。
    */

    for(bootloader_size_t offset = 0U;
        offset < length;
        offset++
    )
    {
        HAL_StatusTypeDef hal_status = 
            HAL_FLASH_Program(
                FLASH_TYPEPROGRAM_BYTE,
                address + offset,
                data[offset]
            );
        if (hal_status != HAL_OK)
        {
            return BOOTLOADER_STATUS_FLASH_ERROR;
        }
    }

    return BOOTLOADER_STATUS_OK;
}
