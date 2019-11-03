/* Copyright (c) 2010 - 2019, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "simple_hal.h"
#include "nrf_mesh_config_examples.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "nrf.h"
#include "nrf_error.h"
#include "boards.h"
#include "nrf_delay.h"

#include "nrf_mesh_defines.h"
#include "timer.h"
#include "app_timer.h"
#include "app_error.h"
#include "mesh_app_utils.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/
#ifdef BOARD_PCA20020
// thingy 52
#include "drv_ext_light.h"
#include "m_ui.h"
#include "support_func.h"

static const nrf_drv_twi_t     m_twi_sensors = NRF_DRV_TWI_INSTANCE(TWI_SENSOR_INSTANCE);
static drv_ext_light_color_mix_t last_light, light_to_change;


/** Initializes the LEDs. */
void hal_leds_init(void)
{

    uint32_t            err_code;
    drv_ext_gpio_init_t ext_gpio_init;


    static const nrf_drv_twi_config_t twi_config_board =
    {
        .scl                = TWI_SCL,
        .sda                = TWI_SDA,
        .frequency          = NRF_TWI_FREQ_400K,
        .interrupt_priority = APP_IRQ_PRIORITY_LOW
    };

    static const drv_sx1509_cfg_t sx1509_cfg_board =
    {
        .twi_addr       = SX1509_ADDR,
        .p_twi_instance = &m_twi_sensors,
        .p_twi_cfg      = &twi_config_board
    };

    ext_gpio_init.p_cfg = &sx1509_cfg_board;
    
    err_code = support_func_configure_io_startup(&ext_gpio_init);
    APP_ERROR_CHECK(err_code);

    drv_ext_light_init_t            led_init;
    static drv_sx1509_cfg_t         sx1509_cfg;

    static const drv_ext_light_conf_t led_conf[DRV_EXT_LIGHT_NUM] = DRV_EXT_LIGHT_CFG;
  

    static const nrf_drv_twi_config_t twi_config =
    {
        .scl                = TWI_SCL,
        .sda                = TWI_SDA,
        .frequency          = NRF_TWI_FREQ_100K,
        .interrupt_priority = APP_IRQ_PRIORITY_LOW
    };
    sx1509_cfg.twi_addr       = SX1509_ADDR;
    sx1509_cfg.p_twi_instance = &m_twi_sensors;
    sx1509_cfg.p_twi_cfg      = &twi_config;

    led_init.p_light_conf        = led_conf;
    led_init.num_lights          = DRV_EXT_LIGHT_NUM;
    led_init.clkx_div            = DRV_EXT_LIGHT_CLKX_DIV_8;
    led_init.p_twi_conf          = &sx1509_cfg;
    led_init.resync_pin          = SX_RESET;

    err_code = drv_ext_light_init(&led_init, false);
    APP_ERROR_CHECK(err_code);
    ERROR_CHECK(drv_ext_light_off(DRV_EXT_RGB_LED_SENSE));
    ERROR_CHECK(drv_ext_light_off(DRV_EXT_RGB_LED_LIGHTWELL));
}

/** Indicate with led that provision was reset **/
void hal_led_provision_reset(void)
{
  light_to_change = DRV_EXT_LIGHT_COLOR_RED;
}

/** Indicate with led that provision was done **/
void hal_led_provision_done(void)
{
  light_to_change = DRV_EXT_LIGHT_COLOR_BLUE;
}


/** Indicate with led that DFU has started **/
void hal_led_dfu_start(void)
{
}


/** Indicate with led that DFU ended **/
void hal_led_dfu_end(void)
{
}

void hal_led_dfu_for_me(void)
{
  light_to_change = DRV_EXT_LIGHT_COLOR_YELLOW;
}

void trigger_led(void)
{
  if (last_light != light_to_change) {
    drv_ext_light_rgb_sequence_t seq=SEQUENCE_DEFAULT_VALUES;
    seq.color = light_to_change;
    seq.sequence_vals.on_time_ms = 500;
    seq.sequence_vals.on_intensity = 0xFF;
    seq.sequence_vals.off_time_ms = 40;
    seq.sequence_vals.off_intensity =5;
    seq.sequence_vals.fade_in_time_ms = 400;
    seq.sequence_vals.fade_out_time_ms = 400; 
    drv_ext_light_rgb_sequence(1, &seq);
    last_light = light_to_change;
  }
}    


#endif

#ifndef BOARD_PCA20020

static bool no_prov;

/** Initializes the LEDs. */
void hal_leds_init(void)
{
    bsp_board_init(BSP_INIT_LEDS);
}

/** Indicate with led that provision was reset **/
void hal_led_provision_reset(void)
{
  bsp_board_leds_off();
  bsp_board_led_on(BSP_BOARD_LED_0);
  no_prov = true;
}

/** Indicate with led that provision was done **/
void hal_led_provision_done(void)
{
  no_prov = false;
  bsp_board_leds_off();
  bsp_board_led_on(BSP_BOARD_LED_1);
}


/** Indicate with led that DFU has started **/
void hal_led_dfu_start(void)
{
  no_prov = false;
  bsp_board_leds_off();
  bsp_board_led_on(BSP_BOARD_LED_2);
  bsp_board_led_on(BSP_BOARD_LED_3);
}

void hal_led_dfu_for_me(void)
{
  // no-op
}


/** Indicate with led that DFU ended **/
void hal_led_dfu_end(void)
{
  no_prov = false;
  bsp_board_leds_off();
}

/** Make led do some light **/
void trigger_led(void)
{
  if (no_prov) 
  {
    bsp_board_led_invert(BSP_BOARD_LED_0);
  }
}


#endif
