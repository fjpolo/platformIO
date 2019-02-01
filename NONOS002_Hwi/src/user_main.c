/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#include "ets_sys.h"
#include "osapi.h"
//#include "spi_test.h"
#include "user_interface.h"
#include "driver/uart.h"
#include "driver/hw_timer.h"

/**** Global variables ***/
static os_timer_t hello_timer;
static os_timer_t blink_timer;
volatile int32_t counter;

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
/* user_rf_cal_sector_set */
uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void){
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

/* hello */
void ICACHE_FLASH_ATTR hello(void *arg){
  /**/
  os_printf("\r\nHello World from LJU! Counter: %d", counter);
  /**/
  counter++;
}

/* blink */
void ICACHE_FLASH_ATTR blink(void *arg){
  static uint8_t state = 0;
/**/
	if (state) {
		GPIO_OUTPUT_SET(2, 1);
	} else {
		GPIO_OUTPUT_SET(2, 0);
	}
	state ^= 1;
}

/* hw_test_timer_cb */
void hw_test_timer_cb(void){
  /**/
  os_printf("\r\nHardware interrupt!!!!!!!!!!!!!!!");
}

/* user_rf_pre_init */
void ICACHE_FLASH_ATTR user_rf_pre_init(void){
}

/**
 *
 * MAIN User Program
 *
 **/
void ICACHE_FLASH_ATTR user_init(void){

    /* Init UART */
    uart_init(115200, 115200);

    /* Init Counter */
    counter = 0;

    /* GPIO */
    gpio_init();

    /* Init Hw timer */
    hw_timer_init(FRC1_SOURCE, 1);
    hw_timer_set_func(hw_test_timer_cb);
    hw_timer_arm(100000);

    /* Init Sw timer */
    os_timer_disarm(&hello_timer);
    os_timer_setfn(&hello_timer, (os_timer_func_t *)hello, NULL);
    os_timer_arm(&hello_timer, 500, 1);

    /* Init Sw timer */
    os_timer_disarm(&blink_timer);
    os_timer_setfn(&blink_timer, (os_timer_func_t *)blink, NULL);
    os_timer_arm(&blink_timer, 500, 1);
}
