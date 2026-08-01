/**
 * glxss_w25q64.c - GLXSS 固件 W25Q64 烧录 + 读取
 *
 * 两种模式:
 *   1. 烧录模式: 从 USART1 (PC) 接收固件数据, 写入 W25Q64
 *   2. 读取模式: 从 W25Q64 读固件, 供 glxss_init() 发送到眼镜
 *
 * 烧录协议 (PC → CH32 → W25Q64):
 *   PC → 4字节固件大小(LE) → CH32
 *   CH32 → 0xBB (已收到大小)
 *   CH32 擦除 Flash, → 0xDD (擦除完成)
 *   循环每页:
 *     PC → 256字节 → CH32
 *     CH32 写入 W25Q64, → 0xEE (页写入完成)
 *   CH32 → 0xFF (全部完成)
 */

#include "glxss_me.h"
#include "W25Q64.h"
#include "debug.h"
#include <string.h>

/* ====================================================================
 * 常量
 * ==================================================================== */

/* 固件在 W25Q64 中的起始地址 */
#define FW_FLASH_ADDR       0x000000

/* W25Q64 页大小 */
#define PAGE_SIZE           256

/* 烧录等待超时 (ms) */
#define FLASH_TIMEOUT_MS    30000

/* ====================================================================
 * 内部: USART1 字节收发 (polling)
 *
 * 注: Host_KM 工程用 USART_Printf_Init() 初始化了 USART1 TX,
 *     但可能没开 RX。这里确保 RX 已使能。
 * ==================================================================== */

static void uart1_rx_enable(void)
{
    USART1->CTLR1 |= USART_CTLR1_RE;  /* 使能接收器 */
}

static void uart1_send_byte(uint8_t b)
{
    while (!(USART1->STATR & USART_FLAG_TXE));
    USART1->DATAR = b;
}

static uint8_t uart1_recv_byte(void)
{
    while (!(USART1->STATR & USART_FLAG_RXNE));
    return (uint8_t)(USART1->DATAR & 0xFF);
}

/* ====================================================================
 * 初始化 W25Q64 (SPI2)
 * ==================================================================== */
void glxss_w25q64_init(void)
{
    printf("W25Q64: Init...\r\n");
    W25Q64_Init();

    uint8_t  mid;
    uint16_t did;
    W25Q64_ReadID(&mid, &did);
    printf("W25Q64: MID=0x%02X DID=0x%04X\r\n", mid, did);
}

/* ====================================================================
 * fw_read 回调 — 从 W25Q64 读取固件
 *
 * 给 glxss_init() 使用, 类似 QSPI Flash 读取。
 * ==================================================================== */
uint32_t glxss_fw_read_w25q64(uint32_t offset, uint8_t *buf, uint32_t len)
{
    W25Q64_ReadData(FW_FLASH_ADDR + offset, buf, len);
    return len;
}

/* ====================================================================
 * 烧录模式: 从 USART1 接收固件 → 写入 W25Q64
 *
 * 调用前需要 USART1 已初始化 (USART_Printf_Init), 
 * 此函数会打开 RX 接收器。
 *
 * 流程:
 *   1. 等待 PC 发 4 字节固件大小
 *   2. 回复 0xBB
 *   3. 擦除 W25Q64
 *   4. 回复 0xDD
 *   5. 循环接收 256 字节页 → 写入 W25Q64 → 回复 0xEE
 *   6. 回复 0xFF
 * ==================================================================== */
void glxss_flash_fw_from_serial(void)
{
    uint32_t fw_size, i;
    uint32_t addr, total_pages;
    uint8_t  page_buf[PAGE_SIZE];

    /* 确保 USART1 RX 已使能 */
    uart1_rx_enable();

    printf("\r\n===== GLXSS FW Flasher =====\r\n");
    printf("Waiting for PC to send firmware...\r\n");

    /* ── 1. 接收固件大小 (4 bytes LE) ── */
    uint8_t size_buf[4];
    for (i = 0; i < 4; i++)
        size_buf[i] = uart1_recv_byte();

    fw_size = (uint32_t)size_buf[0]
            | ((uint32_t)size_buf[1] << 8)
            | ((uint32_t)size_buf[2] << 16)
            | ((uint32_t)size_buf[3] << 24);

    printf("FW size: %lu bytes (%lu KB)\r\n",
           (unsigned long)fw_size,
           (unsigned long)(fw_size / 1024));

    if (fw_size > 2 * 1024 * 1024 || fw_size == 0) {
        printf("ERROR: Invalid firmware size!\r\n");
        return;
    }

    /* ── 2. ACK 大小 ── */
    uart1_send_byte(0xBB);

    /* ── 3. 擦除 Flash ── */
    printf("Erasing W25Q64...\r\n");
    {
        uint32_t erase_bytes = ((fw_size + 4095) / 4096) * 4096;
        uint32_t addr_e = 0;
        while (addr_e < erase_bytes) {
            W25Q64_SectorErase(addr_e);
            addr_e += 4096;
            if (addr_e % (64 * 4096) == 0)
                printf("  Erased %lu KB\r\n", (unsigned long)(addr_e / 1024));
        }
    }
    printf("Erase done\r\n");

    /* ── 4. ACK 擦除完成 ── */
    uart1_send_byte(0xDD);

    /* ── 5. 逐页写入 ── */
    total_pages = (fw_size + PAGE_SIZE - 1) / PAGE_SIZE;
    printf("Writing %lu pages...\r\n", (unsigned long)total_pages);

    for (uint32_t page = 0; page < total_pages; page++) {
        /* 接收 256 字节 */
        for (i = 0; i < PAGE_SIZE; i++)
            page_buf[i] = uart1_recv_byte();

        /* 写入 W25Q64 */
        addr = FW_FLASH_ADDR + page * PAGE_SIZE;
        W25Q64_PageProgram(addr, page_buf, PAGE_SIZE);

        /* ACK */
        uart1_send_byte(0xEE);

        if ((page + 1) % 128 == 0 || page == total_pages - 1)
            printf("  %lu/%lu pages\r\n",
                   (unsigned long)(page + 1),
                   (unsigned long)total_pages);
    }

    /* ── 6. 完成 ── */
    uart1_send_byte(0xFF);
    printf("===== GLXSS FW flash DONE! =====\r\n");
    printf("Reset the board and run GLXSS init.\r\n");
}
