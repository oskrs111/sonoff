#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "driver/button.h"

//OSLL: sonoff push-button control.
//      When PRESSED logical level is LOW.

void initButton()
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
    gpio_output_set(0, 0, 0, BIT0);
}

bool ICACHE_FLASH_ATTR getButton() {
    if(GPIO_INPUT_GET(FUNC_GPIO0) != 0) return false;
    return true;
}