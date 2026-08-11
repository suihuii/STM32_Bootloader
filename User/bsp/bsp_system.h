#ifndef BSP_SYSTEM_H
#define BSP_SYSTEM_H

#include "bootloader_status.h"
#include "bootloader_types.h"

/*
* 获取系统启动以来经过的毫秒数。
*/
bootloader_time_ms_t bsp_system_get_time_ms(void);

/*
* 触发 MCU 系统复位。
*
* 正常情况该函数不会返回
*/
void bsp_system_reset(void);

#endif /* BSP_SYSTEM_H */
