#include "ID00002004_URVG.h"

#include <stdio.h>
#include <stdint.h>

#include "id00003000.h"

//These libraries depend on the processor type used
#include "debug.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"

#define ID00002004_STATUS_BITS 8
#define ID00002004_STATUS_BIT_DONE 0
#define ID00002004_CONFIG_AMOUNT 5

static int32_t ID00002004_clearStatus(uint32_t address, uint32_t port);
static int32_t ID00002004_disableStatus(uint32_t address, uint32_t port);

int32_t ID00002004_init(uint32_t address, uint32_t port)
{
    uint32_t id;

    //Adding configs data to the NoC Manager *MANDATORY WHEN USING NOC MANAGER*/
    id00003000_addConfigs(address, port, ID00002004_csv, ID00002004_CONFIG_AMOUNT);
    id00003000_getID(address, port, &id);
    dev_dbg("IP_URVG","INIT: ID READ : %08X\n", id);    
    ID00002004_clearStatus(address, port);
    id00003000_enableINT(address, port, 0);

    return 0;
}

int32_t ID00002004_startIP(uint32_t address, uint32_t port)
{
    dev_dbg("IP_URVG","========== START IP URVG SUCCESSFULLY ==========\n");

    id00003000_start(address, port);

    return 0;
}

int32_t ID00002004_writeData(uint32_t address, uint32_t port, uint32_t *data, uint32_t size, uint32_t offset)
{
    id00003000_writeMem(address, port, MMEMIN, data, size, offset);

    return 0;
}

int32_t ID00002004_readData(uint32_t address, uint32_t port, uint32_t *data, uint32_t size, uint32_t offset)
{
    id00003000_readMem(address, port, MMEMOUT, data, size, offset);

    return 0;
}

int32_t ID00002004_getStatus(uint32_t address, uint32_t port, uint32_t *status)
{
    id00003000_getStatus(address, port, status);

    return 0;
}

int32_t ID00002004_waitDone(uint32_t address, uint32_t port)
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

static int32_t ID00002004_clearStatus(uint32_t address, uint32_t port)
{
    for(uint32_t i = 0; i < ID00002004_STATUS_BITS; i++)
    {
        id00003000_disableINT(address, port, i);
        id00003000_clearINT(address, port, i);
    }

    return 0;

}

static int32_t ID00002004_disableStatus(uint32_t address, uint32_t port)
{
    for(uint32_t i = 0; i < ID00002004_STATUS_BITS; i++)
    {
        id00003000_disableINT(address, port, i);
        id00003000_clearINT(address, port, i);
    }

    return 0;
}

int32_t ID00002004_URVG_setConf(uint32_t address, uint32_t port, uint32_t *URVG_currentState)
{

	id00003000_writeConfReg(address, port, CCONFIG, URVG_currentState, 1, 0);

	dev_dbg("IP_URVG","cfg1 : %x  \n", configParam.config_reg1);

	return 0;
}



