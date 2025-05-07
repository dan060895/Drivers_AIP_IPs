#include "ID00001010_CONCATENATOR.h"

#include <stdio.h>
#include <stdint.h>

#include "id00003000.h"

//These libraries depend on the processor type used
#include "debug.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"

#define ID00001010_STATUS_BITS 8
#define ID00001010_STATUS_BIT_DONE 0
#define ID00001010_CONFIG_AMOUNT 5

static int32_t ID00001010_clearStatus(uint32_t address, uint32_t port);
static int32_t ID00001010_disableStatus(uint32_t address, uint32_t port);

int32_t ID00001010_init(uint32_t address, uint32_t port)
{
    uint32_t id;

    //Adding configs data to the NoC Manager *MANDATORY WHEN USING NOC MANAGER*/
    id00003000_addConfigs(address, port, ID00001010_csv, ID00001010_CONFIG_AMOUNT);
    id00003000_getID(address, port, &id);
    dev_dbg("IP_CONCATENATOR","INIT: ID READ : %08X\n", id);    
    ID00001010_clearStatus(address, port);
    id00003000_enableINT(address, port, 0);

    return 0;
}

int32_t ID00001010_startIP(uint32_t address, uint32_t port)
{
    dev_dbg("IP_CONCATENATOR","========== START IP CONCATENATOR SUCCESSFULLY ==========\n");

    id00003000_start(address, port);

    return 0;
}

int32_t ID00001010_getStatus(uint32_t address, uint32_t port, uint32_t *status)
{
    id00003000_getStatus(address, port, status);

    return 0;
}

int32_t ID00001010_waitDone(uint32_t address, uint32_t port)
{


   while(NetStatus[0][7] != 1) // IP 0, Done flag (7) == 1?
    {
      	 id00003000_netINTHandler();
         id00003000_getNetINTStatus(NetStatus,address);
    }

  	id00003000_clearBitNetINTStatus(address, port, 7);
  	id00003000_getNetINTStatus(NetStatus,address);

    return 0;
}

static int32_t ID00001010_clearStatus(uint32_t address, uint32_t port)
{
    for(uint32_t i = 0; i < ID00001010_STATUS_BITS; i++)
    {
        id00003000_disableINT(address, port, i);
        id00003000_clearINT(address, port, i);
    }

    return 0;

}

static int32_t ID00001010_disableStatus(uint32_t address, uint32_t port)
{
    for(uint32_t i = 0; i < ID00001010_STATUS_BITS; i++)
    {
        id00003000_disableINT(address, port, i);
        id00003000_clearINT(address, port, i);
    }

    return 0;
}

int32_t ID00001010_Concatenator_setConf(uint32_t address, uint32_t port, GRVGC_state *GRVGC_currentState)
{
/*
	GRVG_configParam configParam;

    configParam.config_reg1 =  ;
	configParam.config_reg2 =  ;
	configParam.config_reg3 =  ;
	configParam.config_reg4 =  ;

	id00003000_writeConfReg(address, port, CCONFIG, &configParam, 4, 0);

	dev_dbg("IPGRVG","cfg1 : %x cfg2 : %x cfg3 : %x cfg4: %x \n",
		       configParam.config_reg1,
		       configParam.config_reg2,
		       configParam.config_reg3,
		       configParam.config_reg4);
*/
	return 0;
}



