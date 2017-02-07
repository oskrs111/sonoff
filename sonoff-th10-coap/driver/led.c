#include "ets_sys.h"
#include "osapi.h"
#include "driver/led.h"

//OSLL: sonoff led control.
//OSLL: Note that GREEN LED is connected to VCC-3V3. Switch operation is inverted.

static bool gLedState = false;
void initLed()
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
	setLed(gLedState);
}

void ICACHE_FLASH_ATTR setLed(bool onOff) 
{

	if(onOff)
	{ 
		gpio_output_set(0, BIT13, BIT13, 0);		
		//ets_uart_printf("LED on\r\n");
	}

	else 
	{
		 gpio_output_set(BIT13, 0, BIT13, 0);
		 //ets_uart_printf("LED off\r\n");
	}

		gLedState = onOff;
}

bool ICACHE_FLASH_ATTR getLed() {
    return gLedState;
}