#ifndef __GLXSS_DISPLAY_H
#define __GLXSS_DISPLAY_H

#include "glxss_me.h"

#define GLXSS_DISP_MODE_IDLE        0
#define GLXSS_DISP_MODE_SSVEP       1
#define GLXSS_DISP_MODE_ARROW       2
#define GLXSS_DISP_MODE_ARROW_TRAIN 3

void glxss_display_init(void);
void glxss_display_set_mode(uint8_t mode);
void glxss_display_set_ssvep_freq(uint32_t freq_idx);
glxss_err_t glxss_display_step(void);
uint8_t glxss_display_get_mode(void);

void glxss_display_notify_result(uint8_t pred);
void glxss_display_notify_collect(uint32_t mode);

#endif