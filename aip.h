#ifndef AIP_H_
#define AIP_H_
#include "stdint.h"

#define AIP_PORT_1 1
#define AIP_PORT_2 2
#define AIP_PORT_3 3

#define MNEMONIC_MAX_LEN 16

typedef struct aip_config
{
    char mnemonic[MNEMONIC_MAX_LEN];
    uint32_t config;
    uint32_t mode;
    uint32_t size;
} aip_config_t;


typedef volatile struct {

	uint32_t config_reg1;
	uint32_t config_reg2;
    uint32_t config_reg3;
    uint32_t config_reg4;

}aip_configParam;

typedef struct aip_regs
{
    uint32_t aip_dataOut;
    uint32_t aip_dataIn;
    uint32_t aip_config;
    uint32_t aip_start;
} aip_regs_t;

#define read_a      0
#define write_a     4
#define config_a    8
#define start_a     12

int8_t aip_init (void *aipBaseAddr, aip_config_t *aip_configs, uint32_t configAmount);

int8_t aip_readMem (void *aipBaseAddr, uint8_t configMem, uint32_t* dataRead, uint16_t amountData, uint32_t offset);

int8_t aip_writeMem (void *aipBaseAddr, uint8_t configMem, uint32_t* dataWrite, uint16_t amountData, uint32_t offset);

int8_t aip_writeConfReg (void *aipBaseAddr, uint8_t configConfReg, uint32_t* dataWrite, uint16_t amountData, uint32_t offset);

int8_t aip_start (void *aipBaseAddr);

//int8_t aip_getID (void *aipBaseAddr, uint32_t *id);
int8_t aip_getID (void *aipBaseAddr, uint32_t *id);


int8_t aip_getStatus (void *aipBaseAddr, uint32_t* status);

int8_t aip_enableINT (void *aipBaseAddr, uint8_t idxInt);
//int8_t aip_enableINT (void *aipBaseAddr, uint32_t idxInt, void (*callback)());

int8_t aip_disableINT (void *aipBaseAddr, uint8_t idxInt);

int8_t aip_clearINT (void *aipBaseAddr, uint8_t idxInt);

int8_t aip_getINT (void *aipBaseAddr, uint8_t* intVector);

int8_t aip_getNotifications(void *aipBaseAddr, uint8_t* notificationsVector);

#endif /* AIP_H_ */
