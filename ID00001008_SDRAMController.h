#ifndef ID00001008_SDRAMCONTROLLER_H_
#define ID00001008_SDRAMCONTROLLER_H_


//#define HAVE_VERBOSE_MESSAGES
//#define HAVE_DEBUG_MESSAGES
#include "stdint.h"

#define HAVE_VERBOSE_MESSAGES
#define HAVE_DEBUG_MESSAGES
/*
#if defined(HAVE_VERBOSE_MESSAGES)
#define dev_err(format, ...)		({printf(format, ## __VA_ARGS__);printf("\n"); })
#define dev_warn(format, ...)		({printf(format, ## __VA_ARGS__);printf("\n"); })
#if defined(HAVE_DEBUG_MESSAGES)
#define dev_dbg(format, ...)		({printf(format, ## __VA_ARGS__);printf("\n"); })
#else
#define dev_dbg(format, ...)	({ if (0) printf(format, ## __VA_ARGS__); })
#endif
#define printk(format, ...)			printf(format, ## __VA_ARGS__)
#else
#define dev_err(format, ...)	({ if (0) printf(format, ## __VA_ARGS__); })
#define dev_warn(format, ...)	({ if (0) printf(format, ## __VA_ARGS__); })
#define dev_dbg(format, ...)	({ if (0) printf(format, ## __VA_ARGS__); })
#define printk(format, ...)			({ if (0) printf(format, ## __VA_ARGS__); })
#endif
*/

extern volatile uint8_t NetStatus[8][8];


#include <stdint.h>



typedef volatile struct {

	uint32_t config_reg1;
	uint32_t config_reg2;
    uint32_t config_reg3;
    uint32_t config_reg4;

}sdramC_configParam;

typedef  struct {
    uint8_t readRequest;
    uint8_t writeRequest;
    uint32_t readAddr;
    uint32_t writeAddr;
    uint8_t readLenEn;
    uint8_t writeLenEn;
    uint32_t readLenSize;
    uint32_t writeLenSize;

}sdramC_state;




int32_t ID00001008_init(uint8_t address, uint8_t port);

int32_t ID00001008_startIP(uint8_t address, uint8_t port);

int32_t ID00001008_writeData(uint8_t address, uint8_t port, uint32_t *data, uint32_t size, uint32_t offset);

int32_t ID00001008_readData(uint8_t address, uint8_t port, uint32_t *data, uint32_t size, uint32_t offset);

int32_t ID00001008_getStatus(uint8_t address, uint8_t port, uint32_t *status);

int32_t ID00001008_waitDone(uint8_t address, uint8_t port);

int32_t ID00001008_waitReady(uint8_t address, uint8_t port);

int32_t ID00001008_waitDoneRead(uint8_t address, uint8_t port);

int32_t ID00001008_waitDoneWrite(uint8_t address, uint8_t port);



int32_t ID00001008_sdramC_setConf(uint8_t address, uint8_t port, sdramC_state *sdramC_currentState);

int32_t ID00001008_sdramC_writeData(uint8_t address, uint8_t port, sdramC_state *sdramC_currentState , uint32_t sdramAddr,uint32_t *data);

int32_t ID00001008_sdramC_readData(uint8_t address, uint8_t port, sdramC_state *sdramC_currentState , uint32_t sdramAddr, uint32_t *data);

int32_t ID00001008_sdramC_writeFrame(uint8_t address, uint8_t port, sdramC_state *sdramC_currentState , uint32_t sdramAddr, uint32_t *data, uint32_t frame_size);

int32_t ID00001008_sdramC_readFrame(uint8_t address, uint8_t port,  sdramC_state *sdramC_currentState , uint32_t sdramAddr, uint32_t *data, uint32_t frame_size);

int32_t ID00001008_sdramC_readFrame2(uint8_t address, uint8_t port,  sdramC_state *sdramC_currentState , uint32_t sdramAddr, uint32_t *data, uint32_t frame_size);












#endif //ID00001008_SDRAMCONTROLLER_H_
