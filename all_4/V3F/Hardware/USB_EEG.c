// #include "USB_EEG.h"
// #include "ch32h417.h"
// #include "ch32h417_usb.h"
// #include <string.h>

// #define EP0_SIZE  64
// #define EP1_SIZE  8
// #define EP2_SIZE  64
// #define EP3_SIZE  64

// static uint8_t EP0_Buf[EP0_SIZE] __attribute__((aligned(4)));
// static uint8_t EP1_Buf[EP1_SIZE] __attribute__((aligned(4)));
// static uint8_t EP2_Buf[EP2_SIZE] __attribute__((aligned(4)));
// static uint8_t EP3_Buf[EP3_SIZE] __attribute__((aligned(4)));

// static volatile uint8_t  SetupReqCode, SetupReqType;
// static volatile uint16_t SetupReqValue, SetupReqIndex, SetupReqLen;
// static volatile uint8_t  DevConfig, DevAddr;
// static const uint8_t     *pDescr = NULL;

// /* ---------- 描述符 ---------- */
// static const uint8_t dev_desc[] = {
//     0x12,
//     0x01,
//     0x10, 0x01,
//     0x02,
//     0x00,
//     0x00,
//     EP0_SIZE,
//     0x86, 0x1A,             // VID 0x1A86
//     0x0E, 0xFE,             // PID 0xFE0E (CDC)
//     0x01, 0x00,
//     0x01,
//     0x02,
//     0x00,
//     0x01
// };

// static const uint8_t cfg_desc[] = {
//     0x09,0x02,0x43,0x00,0x02,0x01,0x00,0x80,0x32,
//     // 接口0 (控制)
//     0x09,0x04,0x00,0x00,0x01,0x02,0x02,0x01,0x00,
//     // CDC 功能描述符
//     0x05,0x24,0x00,0x10,0x01,
//     0x05,0x24,0x01,0x00,0x01,
//     0x04,0x24,0x02,0x02,
//     0x05,0x24,0x06,0x00,0x01,
//     // EP1 IN 中断
//     0x07,0x05,0x81,0x03,EP1_SIZE,0x00,0x01,
//     // 接口1 (数据)
//     0x09,0x04,0x01,0x00,0x02,0x0A,0x00,0x00,0x00,
//     // EP2 OUT
//     0x07,0x05,0x02,0x02,EP2_SIZE,0x00,0x00,
//     // EP3 IN
//     0x07,0x05,0x83,0x02,EP3_SIZE,0x00,0x00
// };

// static const uint8_t lang_desc[]  = {0x04,0x03,0x09,0x04};
// static const uint8_t manu_desc[]  = {0x0C,0x03,'W',0,'C',0,'H',0,'.',0,'C',0,'N',0};
// static const uint8_t prod_desc[]  = {0x14,0x03,'C',0,'H',0,'3',0,'2',0,'H',0,'4',0,'1',0,'7',0};

// static const uint8_t CDC_LineCoding[7] = {
//     0x00, 0xC2, 0x01, 0x00,
//     0x00, 0x00, 0x08
// };

// static void Endp_Init(void)
// {
//     USBFSD->UEP4_1_MOD = USBFS_UEP1_TX_EN;
//     USBFSD->UEP2_3_MOD = USBFS_UEP2_RX_EN | USBFS_UEP3_TX_EN;

//     USBFSD->UEP0_DMA = (uint32_t)EP0_Buf;
//     USBFSD->UEP1_DMA = (uint32_t)EP1_Buf;
//     USBFSD->UEP2_DMA = (uint32_t)EP2_Buf;
//     USBFSD->UEP3_DMA = (uint32_t)EP3_Buf;

//     USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_RES_ACK;
//     USBFSD->UEP2_RX_CTRL = USBFS_UEP_R_RES_ACK;

//     USBFSD->UEP1_TX_LEN = 0;
//     USBFSD->UEP3_TX_LEN = 0;

//     USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_RES_NAK;
//     USBFSD->UEP1_TX_CTRL = USBFS_UEP_T_RES_NAK;
//     USBFSD->UEP3_TX_CTRL = USBFS_UEP_T_RES_NAK;
// }

// void USBFS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
// void USBFS_IRQHandler(void)
// {
//     uint8_t intflag = USBFSD->INT_FG;
//     uint8_t intst   = USBFSD->INT_ST;

//     if (intflag & USBFS_UIF_TRANSFER)
//     {
//         uint8_t token = intst & USBFS_UIS_TOKEN_MASK;

//         if (token == USBFS_UIS_TOKEN_IN)
//         {
//             if ((intst & USBFS_UIS_ENDP_MASK) == 0)
//             {
//                 if (SetupReqLen == 0)
//                 {
//                     USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_TOG | USBFS_UEP_R_RES_ACK;
//                 }
//                 if ((SetupReqType & 0x60) != 0x00)
//                 {
//                     // Non-standard IN complete, nothing
//                 }
//                 else
//                 {
//                     switch (SetupReqCode)
//                     {
//                         case 0x06: // GET_DESCRIPTOR
//                         {
//                             uint16_t len = (SetupReqLen >= EP0_SIZE) ? EP0_SIZE : SetupReqLen;
//                             memcpy(EP0_Buf, pDescr, len);
//                             USBFSD->UEP0_TX_LEN = len;
//                             SetupReqLen -= len;
//                             pDescr += len;
//                             USBFSD->UEP0_TX_CTRL ^= USBFS_UEP_T_TOG;
//                             break;
//                         }
//                         case 0x05: // SET_ADDRESS
//                             USBFSD->DEV_ADDR = (USBFSD->DEV_ADDR & 0x80) | DevAddr;
//                             break;
//                         default:
//                             break;
//                     }
//                 }
//             }
//         }
//         else if (token == USBFS_UIS_TOKEN_OUT)
//         {
//             if ((intst & USBFS_UIS_ENDP_MASK) == 0)
//             {
//                 if (intst & USBFS_UIS_TOG_OK)
//                 {
//                     if ((SetupReqType & 0x60) != 0x00)
//                     {
//                         SetupReqLen = 0;
//                     }
//                 }
//                 if (SetupReqLen == 0)
//                 {
//                     USBFSD->UEP0_TX_LEN = 0;
//                     USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_ACK;
//                 }
//             }
//         }
//         else if (token == USBFS_UIS_TOKEN_SETUP)
//         {
//             USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_NAK;
//             USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_TOG | USBFS_UEP_R_RES_NAK;

//             SetupReqType  = EP0_Buf[0];
//             SetupReqCode  = EP0_Buf[1];
//             SetupReqValue = EP0_Buf[2] | (EP0_Buf[3] << 8);
//             SetupReqIndex = EP0_Buf[4] | (EP0_Buf[5] << 8);
//             SetupReqLen   = EP0_Buf[6] | (EP0_Buf[7] << 8);

//             uint8_t errflag = 0;
//             uint16_t len = 0;

//             if ((SetupReqType & 0x60) != 0x00)
//             {
//                 if (SetupReqType & 0x20)
//                 {
//                     switch (SetupReqCode)
//                     {
//                         case 0x21: pDescr = CDC_LineCoding; len = 7; break;
//                         case 0x20: break;
//                         case 0x22: break;
//                         default: errflag = 0xFF; break;
//                     }
//                 }
//                 else errflag = 0xFF;

//                 if (len > 0)
//                 {
//                     if (SetupReqLen > len) SetupReqLen = len;
//                     len = (SetupReqLen >= EP0_SIZE) ? EP0_SIZE : SetupReqLen;
//                     memcpy(EP0_Buf, pDescr, len);
//                     pDescr += len;
//                     SetupReqLen -= len;
//                 }
//             }
//             else
//             {
//                 switch (SetupReqCode)
//                 {
//                     case 0x06:
//                         switch ((uint8_t)(SetupReqValue >> 8))
//                         {
//                             case 0x01: pDescr = dev_desc; len = sizeof(dev_desc); break;
//                             case 0x02: pDescr = cfg_desc; len = sizeof(cfg_desc); break;
//                             case 0x03:
//                                 switch (SetupReqValue & 0xFF)
//                                 {
//                                     case 0: pDescr = lang_desc; len = sizeof(lang_desc); break;
//                                     case 1: pDescr = manu_desc; len = sizeof(manu_desc); break;
//                                     case 2: pDescr = prod_desc; len = sizeof(prod_desc); break;
//                                     default: errflag = 0xFF; break;
//                                 }
//                                 break;
//                             default: errflag = 0xFF; break;
//                         }
//                         if (!errflag)
//                         {
//                             if (SetupReqLen > len) SetupReqLen = len;
//                             len = (SetupReqLen >= EP0_SIZE) ? EP0_SIZE : SetupReqLen;
//                             memcpy(EP0_Buf, pDescr, len);
//                             pDescr += len;
//                             SetupReqLen -= len;
//                         }
//                         break;

//                     case 0x05: DevAddr = (uint8_t)(SetupReqValue & 0xFF); break;
//                     case 0x08:
//                         EP0_Buf[0] = DevConfig;
//                         if (SetupReqLen > 1) SetupReqLen = 1;
//                         len = 1;
//                         break;
//                     case 0x09: DevConfig = (uint8_t)(SetupReqValue & 0xFF); Endp_Init(); break;
//                     case 0x00:
//                         EP0_Buf[0] = 0x00; EP0_Buf[1] = 0x00;
//                         if (SetupReqLen > 2) SetupReqLen = 2;
//                         len = 2;
//                         break;
//                     case 0x03: break;
//                     case 0x01: break;
//                     default: errflag = 0xFF; break;
//                 }
//             }

//             if (errflag == 0xFF)
//             {
//                 USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_STALL;
//                 USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_TOG | USBFS_UEP_R_RES_STALL;
//             }
//             else
//             {
//                 if (SetupReqType & 0x80)
//                 {
//                     if (len == 0) len = (SetupReqLen >= EP0_SIZE) ? EP0_SIZE : SetupReqLen;
//                     if (len > 0)
//                     {
//                         USBFSD->UEP0_TX_LEN = len;
//                         SetupReqLen -= len;
//                         USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_ACK;
//                     }
//                     else
//                     {
//                         USBFSD->UEP0_TX_LEN = 0;
//                         USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_ACK;
//                     }
//                 }
//                 else
//                 {
//                     if (SetupReqLen == 0)
//                     {
//                         USBFSD->UEP0_TX_LEN = 0;
//                         USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_ACK;
//                     }
//                     else
//                     {
//                         USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_TOG | USBFS_UEP_R_RES_ACK;
//                     }
//                 }
//             }
//         }

//         USBFSD->INT_FG = USBFS_UIF_TRANSFER;
//     }
//     else if (intflag & USBFS_UIF_BUS_RST)
//     {
//         USBFSD->DEV_ADDR = 0;
//         Endp_Init();
//         USBFSD->INT_FG = USBFS_UIF_BUS_RST;
//     }
//     else if (intflag & USBFS_UIF_SUSPEND)
//     {
//         USBFSD->INT_FG = USBFS_UIF_SUSPEND;
//     }
//     else
//     {
//         USBFSD->INT_FG = intflag;
//     }
// }

// void USB_EEG_CDC_Init(void)
// {
//     if ((RCC->PLLCFGR & RCC_SYSPLL_SEL) != RCC_SYSPLL_USBHS)
//     {
//         RCC_USBHS_PLLCmd(DISABLE);
//         RCC_USBHSPLLCLKConfig((RCC->CTLR & RCC_HSERDY) ? RCC_USBHSPLLSource_HSE : RCC_USBHSPLLSource_HSI);
//         RCC_USBHSPLLReferConfig(RCC_USBHSPLLRefer_25M);
//         RCC_USBHSPLLClockSourceDivConfig(RCC_USBHSPLL_IN_Div1);
//         RCC_USBHS_PLLCmd(ENABLE);
//         while (!(RCC->CTLR & RCC_USBHS_PLLRDY));
//     }
//     RCC_USBFSCLKConfig(RCC_USBFSCLKSource_USBHSPLL);
//     RCC_USBFS48ClockSourceDivConfig(RCC_USBFS_Div10);
//     RCC_HBPeriphClockCmd(RCC_HBPeriph_OTG_FS, ENABLE);
//     RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);

//     USBFSH->BASE_CTRL = USBFS_UC_RESET_SIE | USBFS_UC_CLR_ALL;
//     Delay_Us(10);
//     USBFSH->BASE_CTRL = 0x00;

//     USBFSD->INT_EN = USBFS_UIE_SUSPEND | USBFS_UIE_BUS_RST | USBFS_UIE_TRANSFER;
//     USBFSD->BASE_CTRL = USBFS_UC_DEV_PU_EN | USBFS_UC_INT_BUSY | USBFS_UC_DMA_EN;
//     Endp_Init();
//     USBFSD->UDEV_CTRL = USBFS_UD_PD_DIS | USBFS_UD_PORT_EN;
//     NVIC_EnableIRQ(USBFS_IRQn);
// }

// void USB_EEG_CDC_SendPacket(uint8_t *data, uint16_t len)
// {
//     while ((USBFSD->UEP3_TX_CTRL & USBFS_UEP_T_RES_MASK) != USBFS_UEP_T_RES_NAK);
//     memcpy(EP3_Buf, data, len);
//     USBFSD->UEP3_TX_LEN = len;
//     USBFSD->UEP3_TX_CTRL ^= USBFS_UEP_T_TOG;
//     USBFSD->UEP3_TX_CTRL = (USBFSD->UEP3_TX_CTRL & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_ACK;
// }