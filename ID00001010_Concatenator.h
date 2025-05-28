#ifndef ID00001010_CONCATENATOR_H_
#define ID00001010_CONCATENATOR_H_

#include <stdint.h>

extern volatile uint32_t NetStatus[8][8];
    
#define CCONFIG     0b00000   
#define STATUS      0b11110   
#define IPID        0b11111   

int32_t ID00001010_init(uint32_t address, uint32_t port);

int32_t ID00001010_startIP(uint32_t address, uint32_t port);

int32_t ID00001010_getStatus(uint32_t address, uint32_t port, uint32_t *status);

int32_t ID00001010_waitDone(uint32_t address, uint32_t port);


#endif //ID00001010_CONCATENATOR_H_
