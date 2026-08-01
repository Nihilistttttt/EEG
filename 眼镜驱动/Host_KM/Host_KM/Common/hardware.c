/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : GLXSS ME 眼镜 USBHS 检测
*********************************************************************************/
#include "hardware.h"
#include "usb_host_config.h"

#define GLXSS_BL_VID     0x03E7
#define GLXSS_BL_PID     0x2150
#define GLXSS_FW_VID     0x2E09
#define GLXSS_FW_PID     0x0030
#define GLXSS_DETECT_TO  15000
#define POLL_MS          10

/*********************************************************************
 * 检测 USBHS 端口是否有设备连接
 *********************************************************************/
static int GLXSS_Detect(void)
{
    uint32_t tick = GLXSS_DETECT_TO / POLL_MS;

    printf("\r\n========== GLXSS ME Detection ==========\r\n");
    printf("Plug glasses into USB3.0 Type-A port (timeout=%dms)\r\n", GLXSS_DETECT_TO);
    printf("-----------------------------------------\r\n");

    while (tick--) {
        if (USBHSH->PORT_STATUS & USBHS_UHIS_PORT_CONNECT) {
            printf("[DETECT] Device connected!\r\n");
            return 0;
        }
        uint8_t s = USBHSH_CheckRootHubPortStatus(ROOT_DEV_DISCONNECT);
        if (s == ROOT_DEV_CONNECTED) {
            printf("[DETECT] Device hot-plugged!\r\n");
            return 0;
        }
        if (tick % 100 == 0) printf(".");
        Delay_Ms(POLL_MS);
    }
    printf("\r\n[TIMEOUT] No device detected\r\n");
    return 2;
}

/*********************************************************************
 * 检查 KM 枚举结果是否为 GLXSS
 *********************************************************************/
static void GLXSS_CheckAndHandle(void)
{
    if (RootHubDev[DEF_USBHS_PORT_INDEX].bStatus != ROOT_DEV_SUCCESS)
        return;

    uint16_t vid = ((PUSB_DEV_DESCR)DevDesc_Buf)->idVendor;
    uint16_t pid = ((PUSB_DEV_DESCR)DevDesc_Buf)->idProduct;

    if (vid == GLXSS_BL_VID && pid == GLXSS_BL_PID) {
        printf("\r\n========== ✅ GLXSS BOOTLOADER ==========\r\n");
        printf("VID=0x%04X  PID=0x%04X\r\n", vid, pid);
        printf("========================================\r\n\r\n");
    }
    if (vid == GLXSS_FW_VID && pid == GLXSS_FW_PID) {
        printf("\r\n========== ✅ GLXSS FIRMWARE MODE ==========\r\n");
        printf("VID=0x%04X  PID=0x%04X\r\n", vid, pid);
        printf("============================================\r\n\r\n");
    }
}

/*********************************************************************
 * 主初始化
 *********************************************************************/
void Hardware(void)
{
    printf("Build Time: %s %s\n", __DATE__, __TIME__);
    TIM3_Init();

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

#if DEF_USBHS_PORT_EN
    printf("USBHS Host Init\r\n");
    USBHS_Host_Init(ENABLE);
    memset(&RootHubDev[DEF_USBHS_PORT_INDEX].bStatus, 0, sizeof(ROOT_HUB_DEVICE));
    memset(&HostCtl[DEF_USBHS_PORT_INDEX * DEF_ONE_USB_SUP_DEV_TOTAL].InterfaceNum,
           0, DEF_ONE_USB_SUP_DEV_TOTAL * sizeof(HOST_CTL));

    GLXSS_Detect();
#endif

    while (1) {
        USBH_MainDeal();
        GLXSS_CheckAndHandle();
        Delay_Ms(100);
    }
}
