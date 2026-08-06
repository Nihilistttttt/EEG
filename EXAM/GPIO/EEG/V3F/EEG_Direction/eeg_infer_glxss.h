#ifndef __EEG_INFER_GLXSS_H
#define __EEG_INFER_GLXSS_H

#include <stdint.h>

/* GLXSS direction-inference target sequence state machine.
 * Doctor sends CMD_INFER_CFG {mode, rounds}; MCU drives target arrows on the
 * AR glasses (via IPC_CMD_ARROW to V5F) and reports each window's inference
 * result with its ground-truth target label, keeping stimulus and acquisition
 * on the same clock. */

#define GLXSS_INFER_MODE_CYCLE   0
#define GLXSS_INFER_MODE_RANDOM  1

#define GLXSS_INFER_TARGET_FRAMES  500  /* 2s @250Hz target display  */
#define GLXSS_INFER_COLLECT_FRAMES 500  /* 2s @250Hz collect+infer   */

void GLXSS_Infer_Config(uint8_t mode, uint8_t rounds);
void GLXSS_Infer_Start(void);
void GLXSS_Infer_Stop(void);
uint8_t GLXSS_Infer_IsActive(void);
/* Call once per ADS1299 frame from Signal_Analysis main loop. */
void GLXSS_Infer_Poll(void);

#endif