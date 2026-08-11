#ifndef BSP_UART_H
#define BSP_UART_H

#include <stdint.h>

#include "bootloader_status.h"
#include "bootloader_types.h"

/*
 * 启动一次 DMA 非阻塞发送。
 */
bootloader_status_t bsp_uart_transmit(
    const uint8_t *data,
    bootloader_size_t length
);

/*
 * 启动一次 Receive-to-IDLE DMA 非阻塞接收。
 *
 * length 表示接收缓冲区最大容量，
 * 不代表必须收到这么多字节才算有效。
 *
 * 收到接收事件以后，上层应先调用
 * bsp_uart_poll_receive() 取走事件，
 * 再启动下一轮接收。
 */
bootloader_status_t bsp_uart_receive(
    uint8_t *data,
    bootloader_size_t length
);

/*
 * 查询并取走一次 DMA 接收事件。
 *
 * 有事件：
 *     返回 BOOTLOADER_STATUS_OK，
 *     out_length 返回实际接收长度。
 *
 * 没有事件：
 *     返回 BOOTLOADER_STATUS_BUSY。
 */
bootloader_status_t bsp_uart_poll_receive(
    bootloader_size_t *out_length
);

#endif /* BSP_UART_H */
