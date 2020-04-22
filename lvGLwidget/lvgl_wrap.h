#ifndef LVGL_WRAP_H
#define LVGL_WRAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

void LVGLWrap_Init();
uint32_t LVGLWrap_Handler(uint32_t * apPBO);
void LVGLWrap_MouseMoveEvent(int16_t x, int16_t y);
void LVGLWrap_MousePressEvent(bool isPressed);
void LVGLWrap_SetMillis(uint32_t aCurrentMiliseconds);
uint32_t LVGLWrap_GetMillis();
#ifdef __cplusplus
}
#endif

#endif
