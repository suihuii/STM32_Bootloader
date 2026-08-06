#include "bootloader_composition.h"

#include "crc.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

void bootloader_composition_init(void)
{
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();
    MX_CRC_Init();
}