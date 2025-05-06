#ifndef ID00001001_DUMMY_H_
#define ID00001001_DUMMY_H_

#include <stdint.h>

int32_t ID00001001_init(uint8_t address, uint8_t port);

int32_t ID00001001_enableDelay(uint8_t address, uint8_t port, uint32_t msec);

int32_t ID00001001_disableDelay(uint8_t address, uint8_t port);

int32_t ID00001001_startIP(uint8_t address, uint8_t port);

int32_t ID00001001_writeData(uint8_t address, uint8_t port, uint32_t *data, uint32_t size, uint32_t offset);

int32_t ID00001001_readData(uint8_t address, uint8_t port, uint32_t *data, uint32_t size, uint32_t offset);

int32_t ID00001001_getStatus(uint8_t address, uint8_t port, uint32_t *status);

int32_t ID00001001_waitDone(uint8_t address, uint8_t port);

#endif //ID00001001_DUMMY_H_