#include "ID00001008_SDRAMController.h"
#include "debug.h"
#include "stdio.h"
#include "id00003000.h"
#include "system.h"
#include "stdint.h"

#include "altera_avalon_pio_regs.h"

#define ID00001008_STATUS_BITS 8
#define ID00001008_STATUS_BIT_DONE 0
#define ID00001008_CONFIG_AMOUNT 5
//#define SINGLE_CHIP

static aip_config_t ID00001008_csv [] = {
    {"MMEMIN", 0, 'W', 256},
    {"MMEMOUT", 2, 'R', 256},
    {"CCONFIG", 4, 'W', 4},
    {"STATUS", 30, 'B', 1},
    {"IPID", 31, 'R', 1}
};
#define PRINTF
#ifdef PRINTF
    #define LOG_PRINTF(...) printf(__VA_ARGS__)
#else
    #define LOG_PRINTF(...)
#endif

static int32_t ID00001008_clearStatus(uint8_t address, uint8_t port);
static int32_t ID00001008_disableStatus(uint8_t address, uint8_t port);
int32_t ID00001008_init(uint8_t address, uint8_t port)
{
    uint32_t id;

    //Adding configs data to the NoC Manager *MANDATORY WHEN USING NOC MANAGER*/
    id00003000_addConfigs(address, port, ID00001008_csv, ID00001008_CONFIG_AMOUNT);

    id00003000_getID(address, port, &id);
    dev_dbg("IPSDRAM","INIT: ID READ : %08X\n", id);

    //ID00001008_disableStatus(address, port);
    ID00001008_clearStatus(address, port);
    id00003000_enableINT(address, port, 0);
    id00003000_enableINT(address, port, 1);
    id00003000_enableINT(address, port, 2);
    //id00003000_enableINT(address, port, 3);
    //id00003000_enableINT(address, port, 4);
   // id00003000_enableINT(address, port, 5);
    //id00003000_enableINT(address, port, 6);
    //id00003000_enableINT(address, port, 7);


    return 0;
}

int32_t ID00001008_startIP(uint8_t address, uint8_t port)
{
    dev_dbg("IPSDRAM","========== START IP SDRAM CONTROLLER SUCCESSFULLY ==========\n");

    id00003000_start(address, port);

    return 0;
}

int32_t ID00001008_writeData(uint8_t address, uint8_t port, uint32_t *data, uint32_t size, uint32_t offset)
{
    id00003000_writeMem(address, port, MMEMIN, data, size, offset);

    return 0;
}

int32_t ID00001008_readData(uint8_t address, uint8_t port, uint32_t *data, uint32_t size, uint32_t offset)
{
    id00003000_readMem(address, port, MMEMOUT, data, size, offset);


    return 0;
}

int32_t ID00001008_getStatus(uint8_t address, uint8_t port, uint32_t *status)
{
    id00003000_getStatus(address, port, status);

    return 0;
}

int32_t ID00001008_waitDoneWrite(uint8_t address, uint8_t port)
{


   while(NetStatus[0][6] != 1) // IP 0, Done flag (7) == 1?
    {
      	 id00003000_netINTHandler();
         id00003000_getNetINTStatus(NetStatus,address);
    }

  	id00003000_clearBitNetINTStatus(address, port, 7);
  	id00003000_clearBitNetINTStatus(address, port, 6);
  	id00003000_clearBitNetINTStatus(address, port, 5);

  	id00003000_getNetINTStatus(NetStatus,address);

    return 0;
}

int32_t ID00001008_waitDoneRead(uint8_t address, uint8_t port)
{
    while(NetStatus[0][5] != 1) // IP 0, Done flag (7) == 1?
    {
      	 id00003000_netINTHandler();
         id00003000_getNetINTStatus(NetStatus,address);
    }

  	id00003000_clearBitNetINTStatus(address, port, 7);
  	id00003000_clearBitNetINTStatus(address, port, 6);
  	id00003000_clearBitNetINTStatus(address, port, 5);
  	id00003000_getNetINTStatus(NetStatus,address);
    return 0;
}


int32_t ID00001008_waitReady(uint8_t address, uint8_t port)
{
    uint32_t statusNot = 0;

    do
    {
        id00003000_getStatus(address, port, &statusNot);
        printf("THE VALUE IN STATUS: %x \n",statusNot);
    } while (!(statusNot && 0x0100));

    return 0;
}

static int32_t ID00001008_clearStatus(uint8_t address, uint8_t port)
{
    for(uint8_t i = 0; i < ID00001008_STATUS_BITS; i++)
    {
        //id00003000_disableINT(address, port, i);
        //LOG_PRINTF("(%d:%d) Disabled INT %d \n", address, port, i);
        id00003000_clearINT(address, port, i);
        //LOG_PRINTF("(%d:%d) Cleared INT %d \n", address, port, i);
    }

    return 0;

}

static int32_t ID00001008_disableStatus(uint8_t address, uint8_t port)
{
    for(uint8_t i = 0; i < ID00001008_STATUS_BITS; i++)
    {
        id00003000_disableINT(address, port, i);
        //LOG_PRINTF("(%d:%d) Disabled INT %d \n", address, port, i);
        //id00003000_clearINT(address, port, i);
        //LOG_PRINTF("(%d:%d) Cleared INT %d \n", address, port, i);
    }

    return 0;
}

int32_t ID00001008_sdramC_setConf(uint8_t address, uint8_t port, sdramC_state *sdramC_currentState)
{

	sdramC_configParam configParam;

    configParam.config_reg1 = sdramC_currentState->readAddr;
	configParam.config_reg2 = sdramC_currentState->writeAddr;
	configParam.config_reg3 = ((sdramC_currentState->readLenSize) << 8) | ((sdramC_currentState->readLenEn) << 4) | (sdramC_currentState->readRequest);
	configParam.config_reg4 = ((sdramC_currentState->writeLenSize << 8)) | ((sdramC_currentState->writeLenEn) << 4) | (sdramC_currentState->writeRequest);

	id00003000_writeConfReg(address, port, CCONFIG, &configParam, 4, 0);

	dev_dbg("IPSDRAM","cfg1 : %x cfg2 : %x cfg3 : %x cfg4: %x \n",
		       configParam.config_reg1,
		       configParam.config_reg2,
		       configParam.config_reg3,
		       configParam.config_reg4);

	return 0;
}


int32_t ID00001008_sdramC_writeData(uint8_t address, uint8_t port, sdramC_state *sdramC_current_state , uint32_t sdramAddr, uint32_t *data)
{

	sdramC_current_state-> readRequest = 0;
	sdramC_current_state-> writeRequest = 1;
	sdramC_current_state-> readAddr = 0;
#ifdef SINGLE_CHIP
	sdramC_current_state-> writeAddr = 2*sdramAddr;
#else
    sdramC_current_state-> writeAddr = sdramAddr;
#endif	
	sdramC_current_state-> readLenEn = 0;
	sdramC_current_state-> writeLenEn = 0;
	sdramC_current_state-> readLenSize = 0;
	sdramC_current_state-> writeLenSize = 0;

    ID00001008_sdramC_setConf(address, port, sdramC_current_state);

    ID00001008_writeData(address, port, data, 128, 0);
    printf("IRQ IN Data is before start %d\n",data);

    ID00001008_startIP(address,port);
    printf("IRQ IN Data is after %d\n",data);

   // ID00001008_clearStatus(address, port);

    //usleep(100);
    return 0;


}

int32_t ID00001008_sdramC_writeFrame(uint8_t address, uint8_t port, sdramC_state *sdramC_current_state , uint32_t sdramAddr, uint32_t *data, uint32_t frame_size)
{

	sdramC_current_state-> readRequest = 0;
	sdramC_current_state-> writeRequest = 1;
	sdramC_current_state-> readAddr = 0;
#ifdef SINGLE_CHIP
	sdramC_current_state-> writeAddr = 2*sdramAddr;
#else
    sdramC_current_state-> writeAddr = sdramAddr;
#endif	
    sdramC_current_state-> readLenEn = 0;
	sdramC_current_state-> writeLenEn = 1;
	sdramC_current_state-> readLenSize = 0;
	sdramC_current_state-> writeLenSize = frame_size;

    ID00001008_sdramC_setConf(address, port, sdramC_current_state);
    ID00001008_writeData(address, port, data, frame_size , 0);

    ID00001008_startIP(address,port);
    ID00001008_waitDoneWrite(address, port);







    //ID00001008_clearStatus(address, port);

	return 0;
}




int32_t ID00001008_sdramC_readFrame(uint8_t address, uint8_t port, sdramC_state *sdramC_current_state , uint32_t sdramAddr, uint32_t *data, uint32_t frame_size)
{
	dev_dbg("IPSDRAM","FrameSize:%i, sdramAddr: %i \n", frame_size, sdramAddr);

	sdramC_current_state-> readRequest = 1;
	sdramC_current_state-> writeRequest = 0;
#ifdef SINGLE_CHIP
	sdramC_current_state-> readAddr = 2*sdramAddr;
#else
    sdramC_current_state-> readAddr = sdramAddr;
#endif
	sdramC_current_state-> writeAddr = 0;
	sdramC_current_state-> readLenEn = 1;
	sdramC_current_state-> writeLenEn = 0;
	sdramC_current_state-> readLenSize = frame_size;
	sdramC_current_state-> writeLenSize = 0;
    //ID00001008_clearStatus(address, port);

    ID00001008_sdramC_setConf(address, port, sdramC_current_state);
    ID00001008_startIP(address, port);

    //TO FIX: Change to INT
    ID00001008_waitDoneRead(address, port);

#define IPTOIP
#ifndef IPTOIP
    ID00001008_readData(address, port, data, frame_size, 0);
#endif
    /*
	printf("----------SDRAM READ FRAME. [%d]%d-------\n");
	for (uint8_t j = 0; j <128; j++){
		printf("[%03d]%08d ", j, data[j]);
	}
	printf("\n\n");
*/

   // ID00001008_clearStatus(address, port);

    return 0;

}
int32_t ID00001008_sdramC_readFrame2(uint8_t address, uint8_t port, sdramC_state *sdramC_current_state , uint32_t sdramAddr, uint32_t *data, uint32_t frame_size)
{
	dev_dbg("IPSDRAM","FrameSize:%i, sdramAddr: %i \n", frame_size, sdramAddr);

	sdramC_current_state-> readRequest = 1;
	sdramC_current_state-> writeRequest = 0;
#ifdef SINGLE_CHIP
	sdramC_current_state-> readAddr = 2*sdramAddr;
#else
    sdramC_current_state-> readAddr = sdramAddr;
#endif
	sdramC_current_state-> writeAddr = 0;
	sdramC_current_state-> readLenEn = 1;
	sdramC_current_state-> writeLenEn = 0;
	sdramC_current_state-> readLenSize = frame_size;
	sdramC_current_state-> writeLenSize = 0;
    ID00001008_clearStatus(address, port);

    ID00001008_sdramC_setConf(address, port, sdramC_current_state);
    ID00001008_startIP(address, port);

    //TO FIX: Change to INT
    ID00001008_waitDoneRead(address, port);
    ID00001008_readData(address, port, data, frame_size, 0);
    ID00001008_clearStatus(address, port);

    return 0;

}

/*
int32_t ID00001008_enableDelay(uint8_t address, uint8_t port, uint32_t msec)
{
    uint32_t delay = 0;

    delay = (msec << 1) | 1;

    id00003000_writeConfReg(address, port, "CDELAY", &delay, 1, 0);

    id00003000_enableINT(address, port, ID00001008_STATUS_BIT_DONE);

    return 0;
}

int32_t ID00001008_disableDelay(uint8_t address, uint8_t port)
{
    uint32_t delay = 0;

    id00003000_writeConfReg(address, port, "CDELAY", &delay, 1, 0);

    id00003000_disableINT(address, port, 0);

    id00003000_clearINT(address, port, 0);

    return 0;
}*/
