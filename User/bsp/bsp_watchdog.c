#include "bsp_watchdog.h"

#include "bootloader_status.h"
#include "iwdg.h"

bootloader_status_t bsp_watchdog_refresh(void)
{
    HAL_StatusTypeDef hal_status = 
        HAL_IWDG_Refresh(&hiwdg);
    
    if (hal_status != HAL_OK)
    {
        return BOOTLOADER_STATUS_INTERNAL_ERROR;
    }

    return BOOTLOADER_STATUS_OK;
}