#ifndef BSP_WATCHDOG_H
#define BSP_WATCHDOG_H

#include "bootloader_status.h"

/*
* 刷新独立看门狗计数器。
* 
* 必须在看门狗超时周期性调用，
* 否则 MCU 会自动复位。
*/
bootloader_status_t bsp_watchdog_refresh(void);

#endif
