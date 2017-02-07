#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>
#include <gpio.h>
#include <user_interface.h>
#include <espconn.h>
#include <ip_addr.h>
int ets_uart_printf(const char *fmt, ...);
#include "driver/uart.h"
#include "driver/led.h"
#include "driver/button.h"
#include "user_main.h"
#include "lobaro-coap/coap.h"
#include "lobaro-coap/interface/esp8266/lobaro-coap_esp8266.h"

//internal ESP8266 Control Structures
esp_udp UdpCoAPConn;
struct espconn CoAP_conn = {.proto.udp = &UdpCoAPConn};

//"Main Loop" Speed
#define DELAY_LOOP 50 // milliseconds
#define ONE_SECOND 1000
LOCAL os_timer_t MainLoop_timer;

LOCAL void ICACHE_FLASH_ATTR mainLoopTimer_cb(void *arg)
{
	if(CoAP_ESP8266_States.TxSocketIdle) CoAP_doWork();
	_softap_monitor();
	_button_process();

}

void ICACHE_FLASH_ATTR init_done(void) {

	static uint8_t CoAP_WorkMemory[4096]; //Working memory of CoAPs internal memory allocator
	

	CoAP_Init(CoAP_WorkMemory, 4096);
	CoAP_ESP8266_CreateInterfaceSocket(0, &CoAP_conn, 5683, CoAP_onNewPacketHandler, CoAP_ESP8266_SendDatagram);

	Create_Wifi_IPs_Resource(); 		//example of simple GET resource
	//Create_About_Resource();			//example of large resource (blockwise transfers)
	Create_Wifi_Config_Resource(); 		//example of uri-query usage
	//Create_RTC_Resource(); 				//example of observable resource
	Create_Led_Resource(); 				//example of observable resource triggered by itself + uri-query usage
	Create_Relay_Resource();
	Create_Wifi_Scan_Resource(); 		//example of longer lasting "postponed" resource with two responses (1. empty ACK, 2. actual resource)

	ets_uart_printf("- CoAP init done! Used CoAP ram memory:\r\n"); //note: static ram footprint depends primary on resource count+uri lengths
	coap_mem_determinateStaticMem();
	coap_mem_stats();

	// Set up a "main-loop" timer
	os_timer_disarm(&MainLoop_timer);
	os_timer_setfn(&MainLoop_timer, (os_timer_func_t *)mainLoopTimer_cb, (void *)0);
	os_timer_arm(&MainLoop_timer, DELAY_LOOP, 1);	
}

//OSLL: Force Wifi Network by these macros at:
// 		user/lobaro-coap/interface/esp8266/lobaro-coap_esp8266.h
//#define USE_HARDCODED_CREDENTIALS (1) 
//#define EXTERNAL_AP_SSID "your-wifi-ssid"
//#define EXTERNAL_AP_PW "your-wifi-ssid-password"

void user_init(void) {
	// Configure the UART
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	
	//OSLL: Configure on board devices -> removed from CoAP resources init...
	initLed();
	initRelay();

	//OSLL: Set relay at start. This will allow to add a serial harware switch to
	//							have a manual control on the device even without Wifi connection. 
	setRelay(true);

	//Config ESP8266 network
	CoAP_ESP8266_ConfigDevice();
	system_init_done_cb(init_done);
}

//**************************** OSLL: Helper functions *****************************************
void ICACHE_FLASH_ATTR _softap_monitor(void)
{
	static int cnt = 0;
	if(ESP8266_isIn_SoftAP() == true)
	{
		if(cnt == 24)
		{
			setLed(!getLed());
			cnt = 0;
		}
		else cnt++;
	}
}

void ICACHE_FLASH_ATTR _button_process(void)
{
	static int cnt = 0;
	static int st = 0;

	switch(st)
	{
		case 0: 
			if(getButton() == true)
			{
				setLed(true);
				if(cnt >= 3) //OSLL: Press button for 150ms (...timer at 50ms...)
				{
					setRelay(!getRelay());
					st++; //OSLL: Go to wait button release...										
				}
				else cnt++;				
			}			
			break;

		case 1:
			if(getButton() == false)
			{
				setLed(false);
				cnt = 0;
				st++; //OSLL: Go to waistate								
			}
			break;

		case 2:
		//OSLL: Waitstate between button pushes...
			setLed(!getLed());
			if(cnt >= (ONE_SECOND/DELAY_LOOP))
			{				
				setLed(false);
				st = 0; 
				cnt = 0;					
			}
			else cnt++;				
			break;
	}	
}