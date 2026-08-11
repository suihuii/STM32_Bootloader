#include <stddef.h>
#include <stdint.h>

#include "bsp_uart.h"
#include "bootloader_status.h"
#include "usart.h"

/*
* 把 STM32 HAL 的返回值类型转换成 Bootloader 自己的状态码。
* 
* 这样上层以后不需要知道 HAL_StatusTypeDef
*/

/*
* UART 中断回调和主循环都会访问这两个变量，
* 因此必须声明 volatile
*/
static volatile bootloader_size_t g_uart_rx_length = 0U;
static volatile uint8_t g_uart_rx_event_pending = 0U;

static bootloader_status_t bsp_uart_map_hal_status(
    HAL_StatusTypeDef hal_status)
{
    if (hal_status == HAL_OK)
    {
        return BOOTLOADER_STATUS_OK;
    }

    if (hal_status == HAL_BUSY)
    {
        return BOOTLOADER_STATUS_BUSY;
    }

    if (hal_status == HAL_TIMEOUT)
    {
        return BOOTLOADER_STATUS_TIMEOUT;
    }

    return BOOTLOADER_STATUS_UART_ERROR;
}

bootloader_status_t bsp_uart_transmit(
    const uint8_t *data,
    bootloader_size_t length
)
{
    if(data == NULL)
    {
        return BOOTLOADER_STATUS_INVALID_ARGUMENT;
    }

    if((length == 0U) || (length > UINT16_MAX))
    {
        return BOOTLOADER_STATUS_INVALID_LENGTH;
    }

    HAL_StatusTypeDef hal_status = HAL_UART_Transmit_DMA(
        &huart1,
        data,
        (uint16_t)length
    );

    return bsp_uart_map_hal_status(hal_status);
}

bootloader_status_t bsp_uart_receive(
    uint8_t *data,
    bootloader_size_t length
)
{
    if(data == NULL)
    {
        return BOOTLOADER_STATUS_INVALID_ARGUMENT;
    }

    if((length == 0U) || (length > UINT16_MAX))
    {
        return BOOTLOADER_STATUS_INVALID_LENGTH;
    }

    /*
    * 上一次接收事件还没有被主循环取走时，
    * 不允许启动下一轮 DMA 接收。
    */
    if (g_uart_rx_event_pending != 0U)
    {
        return BOOTLOADER_STATUS_BUSY;
    }
    HAL_StatusTypeDef hal_status = HAL_UARTEx_ReceiveToIdle_DMA(
        &huart1,
        data,
        (uint16_t)length
    );
    if(hal_status == HAL_OK)
    {
        /*
        * Receive-to-IDLE DMA 默认还会产生 Half Transfer 事件。
        *
        * 当前 Bootloader 只关系：
        * IDLE：本批数据暂时发送完成了
        * TC：缓冲区已经收满
        * 
        * 因此关闭 DMA Half Transfer 事件。
        */
        __HAL_DMA_DISABLE_IT(huart1.hdmarx,DMA_IT_HT);
    }

    return bsp_uart_map_hal_status(hal_status);
}

void HAL_UARTEx_RxEventCallback(
    UART_HandleTypeDef *huart,
    uint16_t Size)
{
    /*
    * 系统以后可能还有其他 UART。
    * BSP 这里只处理 USART1。
    */
    if (huart->Instance != USART1)
    {
        return;
    }
    /*
    * 记录本次 DMA 实际收到的字节数。
    *
    * 注意：
    * 中断回调里不做协议解析，
    * 只记录最小必要信息。
    */
    g_uart_rx_length = (bootloader_size_t)Size;
    g_uart_rx_event_pending = 1U;
}

bootloader_status_t bsp_uart_poll_receive(
    bootloader_size_t *out_length
)
{
    if(out_length == NULL)
    {
        return BOOTLOADER_STATUS_INVALID_ARGUMENT;
    }
    /*
    * 没有新的 UART 接收事件。
    */
    if (g_uart_rx_event_pending == 0U)
    {
        return BOOTLOADER_STATUS_BUSY;
    }

    /*
    * 把 ISR 记录的实际接收长度交给主循环。
    */
    *out_length = g_uart_rx_length;

    /*
    * 当前事件已经被上层取走。
    */
    g_uart_rx_event_pending = 0U;
    
    return BOOTLOADER_STATUS_OK;
}
