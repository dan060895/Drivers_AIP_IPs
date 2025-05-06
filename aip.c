#include "stdlib.h"
#include "stdint.h"
#include "altera_avalon_pio_regs.h"
#include "aip.h"
#include <system.h>
#include "altera_avalon_timer_regs.h"

#define AIP_CONFIG_STATUS             30
#define AIP_CONFIG_ID                 31
#define AIP_STATUS_MASK_NU            0xff000000
#define AIP_STATUS_MASK_MASK          0x00ff0000
#define AIP_STATUS_MASK_NOTIFICATION  0x0000ff00
#define AIP_STATUS_MASK_INT           0x000000ff
#define AIP_STATUS_SHIFT_NU           24
#define AIP_STATUS_SHIFT_MASK         16
#define AIP_STATUS_SHIFT_NOTIFICATION 80
#define AIP_STATUS_SHIFT_INT          0

#define AIP_DATAOUT  0
#define AIP_DATAIN   4
#define AIP_CONFIG   8
#define AIP_START    12
#define AIP_STATUS   30
#define AIP_IPID     31
#define AIP_PORTS 3
typedef struct aip_portConfig
{
    uint8_t amount;
    aip_config_t *configs;
} aip_portConfig_t;


static const uint32_t AIP_PORT_BASE[] =
{
        SLAVE_1_BASE,
		SLAVE_2_BASE,
		AIP_UP_0_BASE,

};

static aip_portConfig_t aip_portConfigs [AIP_PORTS];

static uint8_t aip_aipRead (void *baseAddr, uint32_t config, uint32_t *data, uint32_t size);

static uint8_t aip_aipWrite (void *baseAddr, uint32_t config, uint32_t *data, uint32_t size);

static uint8_t aip_aipStart (void *baseAddr);
/*
static uint8_t aip_getPortAdders (void *aipBaseAddr, uint32_t *aipBaseAddr);

int8_t aip_init (void *aipBaseAddr, aip_config_t *aip_configs, uint8_t configAmount)
{
    aip_portConfigs[aipPort].amount = configAmount;

    aip_portConfigs[aipPort].configs = aip_configs;

    return 0;
}
*/
int8_t aip_readMem (void *aipBaseAddr, uint8_t configMem, uint32_t* dataRead, uint16_t amountData, uint32_t offset)
{
    /* set addrs */
    aip_aipWrite((void *)aipBaseAddr, configMem+1, &offset, 1);

    /* write data */
    aip_aipRead((void *)aipBaseAddr, configMem, dataRead, amountData);

    return 0;
}

int8_t aip_writeMem (void *aipBaseAddr, uint8_t configMem, uint32_t* dataWrite, uint16_t amountData, uint32_t offset)
{
    /* set addrs */
    aip_aipWrite((void *)aipBaseAddr, configMem+1, &offset, 1);

    /* write data */
    aip_aipWrite((void *)aipBaseAddr, configMem, dataWrite, amountData);

    return 0;
}

int8_t aip_writeConfReg (void *aipBaseAddr, uint8_t configConfReg, uint32_t* dataWrite, uint16_t amountData, uint32_t offset)
{
    /* set addrs */
    aip_aipWrite((void *)aipBaseAddr, configConfReg+1, &offset, 1);

    /* write data */
    aip_aipWrite((void *)aipBaseAddr, configConfReg, dataWrite, amountData);

    return 0;
}

int8_t aip_start (void *aipBaseAddr)
{
    aip_aipStart((void *)aipBaseAddr);

    return 0;
}

int8_t aip_getID (void *aipBaseAddr, uint32_t *id)
{
    aip_aipRead((void *)aipBaseAddr, AIP_IPID, id, 1);

    return 0;
}

int8_t aip_getStatus (void *aipBaseAddr, uint32_t* status)
{
    aip_aipRead((void *)aipBaseAddr, AIP_STATUS, status, 1);

    return 0;
}

int8_t aip_enableINT (void *aipBaseAddr, uint8_t idxInt)
{
    uint32_t status = 0;

    aip_aipRead ((void *)aipBaseAddr, AIP_STATUS, &status, 1);

    status &= AIP_STATUS_MASK_MASK;

    status |= (1 << (idxInt+AIP_STATUS_SHIFT_MASK));

    aip_aipWrite((void *)aipBaseAddr, AIP_STATUS, &status, 1);

    return 0;
}

int8_t aip_disableINT (void *aipBaseAddr, uint8_t idxInt)
{
    uint32_t status = 0;

    aip_aipRead ((void *)aipBaseAddr, AIP_STATUS, &status, 1);

    status &= AIP_STATUS_MASK_MASK;

    status &= ~(uint32_t)(1 << (idxInt+AIP_STATUS_SHIFT_MASK));

    aip_aipWrite((void *)aipBaseAddr, AIP_STATUS, &status, 1);

    return 0;
}

int8_t aip_clearINT (void *aipBaseAddr, uint8_t idxInt)
{
    uint32_t status = 0;

    aip_aipRead((void *)aipBaseAddr, AIP_STATUS, &status, 1);

    status = (status & (AIP_STATUS_MASK_NU | AIP_STATUS_MASK_MASK | AIP_STATUS_MASK_NOTIFICATION)) | (uint32_t)(1 << idxInt);

    aip_aipWrite((void *)aipBaseAddr, AIP_STATUS, &status, 1);

    return 0;
}

int8_t aip_getINT (void *aipBaseAddr, uint8_t* intVector)
{
    uint32_t status = 0;

    aip_aipRead ((void *)aipBaseAddr, AIP_STATUS, &status, 1);

    *intVector = (uint8_t)(status & AIP_STATUS_MASK_INT);

    return 0;
}

int8_t aip_getNotifications(void *aipBaseAddr, uint8_t* notificationsVector)
{
    uint32_t status = 0;

    aip_aipRead ((void *)aipBaseAddr, AIP_STATUS, &status, 1);

    *notificationsVector = (uint8_t)((status & AIP_STATUS_MASK_NOTIFICATION) >> AIP_STATUS_SHIFT_NOTIFICATION);

    return 0;
}

static uint8_t aip_aipRead (void *aipBaseAddr, uint32_t config, uint32_t *data, uint32_t size )
{

    volatile uint32_t *reg32 = (volatile uint32_t *)aipBaseAddr;

    reg32[AIP_CONFIG] = config;

    for (uint32_t i = 0; i < size; i++)
	{
		data[i] = reg32[AIP_DATAOUT];
	}

	return 0 ;
};

static uint8_t aip_aipWrite (void *aipBaseAddr, uint32_t config, uint32_t *data, uint32_t size)
{

    volatile uint32_t *reg32 = (volatile uint32_t *)aipBaseAddr;

    reg32[AIP_CONFIG] = config;

    for (uint32_t i = 0; i < size; i++) {
        reg32[AIP_DATAIN] = data[i];
    }

	return 0 ;
};

static uint8_t aip_aipStart (void *aipBaseAddr)
{
    volatile uint32_t *reg32 = (volatile uint32_t *)aipBaseAddr;

    reg32[AIP_START] = 0x1;
	return 0 ;
};
/*
static uint8_t aip_getPortAdders (void *aipBaseAddr, uint32_t *aipBaseAddr)
{
    *aipBaseAddr = AIP_PORT_BASE[aipBaseAddr];

    return 0;
}
*/
