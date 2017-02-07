/*******************************************************************************

 *******************************************************************************/
#include <os_type.h>
#include <ets_sys.h>

#include "../lobaro-coap/coap.h"
#include "driver/relay.h"
#include "relay_res.h"

CoAP_Res_t* pRelay_Res = NULL;

static void ICACHE_FLASH_ATTR SetRelaystatePayload(CoAP_Message_t* pReq, CoAP_Message_t* pResp){
	if(getRelay()) CoAP_SetPayload(pReq, pResp, "Relay is on!", coap_strlen("Relay is on!"), true);
	else CoAP_SetPayload(pReq, pResp, "Relay is off!", coap_strlen("Relay is off!"), true);
}

static CoAP_HandlerResult_t ICACHE_FLASH_ATTR Res_ReqHandler(CoAP_Message_t* pReq, CoAP_Message_t* pResp) {
	if(pReq->Code == REQ_POST) {
		CoAP_option_t* pOpt;
		bool Found = false;
		bool RelayState = getLed();

		for(pOpt =pReq->pOptionsList ; pOpt != NULL; pOpt = pOpt->next) {
			switch(CoAP_FindUriQueryVal(pOpt,"",3, "on","off", "tgl")) { 				
				case 0: break; //not found
				case 1: setRelay(true); Found=true; break; //found "on"
				case 2: setRelay(false); Found=true; break; //found "off"
				case 3: setRelay(!RelayState); Found=true; break; //found "tgl"
			}
			if(Found) {
				SetRelaystatePayload(pReq, pResp);
				break;
			}
		}

		if(!Found){
			char info[] = {"usage: coap://.../relay_gpio13?on (or \"off\", \"tgl\")"};
			CoAP_SetPayload(pReq, pResp, info, coap_strlen(info), true);
			pResp->Code=RESP_ERROR_BAD_REQUEST_4_00;
		}
		else if(pRelay_Res)
		{
			CoAP_NotifyResourceObservers(pRelay_Res); //Notify all observers of change
		}

	}else if(pReq->Code == REQ_GET){
		SetRelaystatePayload(pReq, pResp);
	}

	return HANDLER_OK;
}

static CoAP_HandlerResult_t ICACHE_FLASH_ATTR Res_NotifyHandler(CoAP_Observer_t* pObserver, CoAP_Message_t* pResp) {
	SetRelaystatePayload(NULL, pResp);
	return HANDLER_OK;
}

CoAP_Res_t* ICACHE_FLASH_ATTR Create_Relay_Resource() {
	CoAP_ResOpts_t Options = {.Cf = COAP_CF_TEXT_PLAIN, .Flags = RES_OPT_POST | RES_OPT_GET};

	setRelay(false);
	return (pRelay_Res=CoAP_CreateResource("esp8266/relay_gpio13", "RELAY at GPIO13",Options, Res_ReqHandler, Res_NotifyHandler));
}

