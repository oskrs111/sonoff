#include "ets_sys.h"
#include "osapi.h"
#include "driver/relay.h"

//OSLL: sonoff relay control.

static bool gRelayState = false;
void initRelay()
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	setRelay(gRelayState);
}

void ICACHE_FLASH_ATTR setRelay(bool onOff) 
{

	if(onOff)
	{ 		
		gpio_output_set(BIT12, 0, BIT12, 0);
		ets_uart_printf("RELAY on\r\n");
	}

	else 
	{		 		 
         gpio_output_set(0, BIT12, BIT12, 0);
		 ets_uart_printf("RELAY off\r\n");
	}

		gRelayState = onOff;
}

bool ICACHE_FLASH_ATTR getRelay() {
    return gRelayState;
}