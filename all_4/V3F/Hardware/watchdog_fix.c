#include "watchdog_fix.h"
#include "debug.h"

/*
 * 独立看门狗修复：
 * 如果 IWDG 已经被硬件启动，程序必须定期喂狗，否则会自动复位。
 */

void Watchdog_Fix_Init(void)
{
    /* 允许写 IWDG 配置 */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* 尽量把看门狗超时时间拉长 */
    IWDG_SetPrescaler(IWDG_Prescaler_256);
    IWDG_SetReload(0x0FFF);

    /* 立即喂一次狗 */
    IWDG_ReloadCounter();

    /*
     * 如果 IWDG 已经硬件启动，这里只是继续使能；
     * 如果原本没有启动，这句会启动 IWDG。
     */
    IWDG_Enable();
}

void Watchdog_Feed(void)
{
    IWDG_ReloadCounter();
}

/*
 * 带喂狗的延时函数。
 * 以后长延时尽量不要直接用 Delay_Ms(500)，改用 Delay_Ms(500)。
 */
void Delay_Ms(uint32_t ms)
{
    while (ms--)
    {
        Delay_Ms(1);
        Watchdog_Feed();
    }
}   