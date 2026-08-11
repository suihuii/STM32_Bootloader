#include "bsp_system.h"

#include "bootloader_types.h"
#include "stm32f4xx_hal.h"

bootloader_time_ms_t bsp_system_get_time_ms(void)
{
    return (bootloader_time_ms_t)HAL_GetTick();
}

void bsp_system_reset(void)
{
    NVIC_SystemReset();
}
