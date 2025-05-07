#ifndef ID00001009_P2SConverter_H_
#define ID00001009_P2SConverter_H_

#include <stdint.h>

extern volatile uint32_t NetStatus[8][8];

#define MMEMIN      0b00000   
#define CCONFIG     0b00100   
#define STATUS      0b11110   
#define IPID        0b11111   


int32_t ID00001009_init(uint32_t address, uint32_t port);

int32_t ID00001009_startIP(uint32_t address, uint32_t port);

int32_t ID00001009_writeData(uint32_t address, uint32_t port, uint32_t *data, uint32_t size, uint32_t offset);

int32_t ID00001009_readData(uint32_t address, uint32_t port, uint32_t *data, uint32_t size, uint32_t offset);

int32_t ID00001009_getStatus(uint32_t address, uint32_t port, uint32_t *status);

int32_t ID00001009_waitDone(uint32_t address, uint32_t port);

int32_t ID00001009_waitReady(uint32_t address, uint32_t port);

int32_t ID00001009_waitDoneRead(uint32_t address, uint32_t port);

int32_t ID00001009_waitDoneWrite(uint32_t address, uint32_t port);

#endif //ID00001009_P2SConverter_H_
