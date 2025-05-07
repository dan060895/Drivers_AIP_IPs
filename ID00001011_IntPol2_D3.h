#ifndef ID00001011_INTPOL2_D3_H_
#define ID00001011_INTPOL2_D3_H_

#include <stdint.h>

extern volatile uint32_t NetStatus[8][8];

#define MMEMIN      0b00000   
#define MMEMOUT     0b00010      
#define CCONFIG     0b00100   
#define STATUS      0b11110   
#define IPID        0b11111   


int32_t ID00001011_init(uint32_t address, uint32_t port);

int32_t ID00001011_startIP(uint32_t address, uint32_t port);

int32_t ID00001011_writeData(uint32_t address, uint32_t port, uint32_t *data, uint32_t size, uint32_t offset);

int32_t ID00001011_readData(uint32_t address, uint32_t port, uint32_t *data, uint32_t size, uint32_t offset);

int32_t ID00001011_getStatus(uint32_t address, uint32_t port, uint32_t *status);

int32_t ID00001011_waitDone(uint32_t address, uint32_t port);

#endif //ID00001011_INTPOL2_D3_H_
