#ifndef BOOTLOADER_STATUS_H
#define BOOTLOADER_STATUS_H

/*
* bootloader 公共状态码。
*
* 作用：
* 所有BSP / service / app 层统一用这一套返回值
* 避免每个模块自己定义 0, -1, 1 之类的含糊错误码。
*/

typedef enum
{
    /* 操作成功 */
    BOOTLOADER_STATUS_OK = 0,

    /* 当前资源或模块正忙，暂时不能执行 */
    BOOTLOADER_STATUS_BUSY,

    /* 操作等待时间超过限制 */
    BOOTLOADER_STATUS_TIMEOUT,

    /* 传入参数本身不合法，例如空指针 */
    BOOTLOADER_STATUS_INVALID_ARGUMENT,

    /* 参数合法，但当前 Bootloader 状态不允许执行该操作 */
    BOOTLOADER_STATUS_INVALID_STATE,

    /* 数据长度不符合要求 */
    BOOTLOADER_STATUS_INVALID_LENGTH,

    /* 地址或长度超出了允许范围 */
    BOOTLOADER_STATUS_OUT_OF_RANGE,

    /* Flash 地址或长度不满足写入对齐要求 */
    BOOTLOADER_STATUS_ALIGNMENT_ERROR,

    /* 实际 CRC 与期望 CRC 不一致 */
    BOOTLOADER_STATUS_CRC_MISMATCH,

    /* 固件镜像格式、向量表等检查失败 */
    BOOTLOADER_STATUS_INVALID_IMAGE,

    /* UART 底层操作失败 */
    BOOTLOADER_STATUS_UART_ERROR,

    /* Flash 擦除或写入失败 */
    BOOTLOADER_STATUS_FLASH_ERROR,

    /* 无法归类的内部错误 */
    BOOTLOADER_STATUS_INTERNAL_ERROR

} bootloader_status_t;

#endif
