/**
 * @file lv_port_disp_templ.h
 *
 */

 /*Copy this file as "lv_port_disp.h" and set this value to "1" to enable content*/
#if 1

#ifndef LV_PORT_DISP_TEMPL_H
#define LV_PORT_DISP_TEMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_port_disp_init(void);

//  Defined in lvgl.c
void put_display_px(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_PORT_DISP_TEMPL_H*/

#endif /*Disable/Enable content*/
