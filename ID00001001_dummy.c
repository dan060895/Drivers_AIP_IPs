#include "ID00001001_dummy.h"
#include <stdio.h>
#include "id00003000.h"


#define ID00001001_STATUS_BITS 8
#define ID00001001_STATUS_BIT_DONE 0

#define ID00001001_CONFIG_AMOUNT 5
static aip_config_t ID00001001_csv [] = {
    {"MDATAIN", 0, 'W', 64},
    {"MDATAOUT", 2, 'R', 64},
    {"CDELAY", 4, 'W', 1},
    {"STATUS", 30, 'B', 1},
    {"IPID", 31, 'R', 1}
};


static int32_t ID00001001_clearStatus(uint8_t address, uint8_t port);


int32_t ID00001001_init(uint8_t address, uint8_t port)
{
    uint32_t id;

    //Adding configs data to the NoC Manager *MANDATORY WHEN USING NOC MANAGER*/
    id00003000_addConfigs(address, port, ID00001001_csv, ID00001001_CONFIG_AMOUNT);

    id00003000_getID(address, port, &id);
    printf("INIT: ID read in (%d:%d) : %08X\n", address, port, id);

    ID00001001_clearStatus(address, port);

    return 0;
}

int32_t ID00001001_enableDelay(uint8_t address, uint8_t port, uint32_t msec)
{
    uint32_t delay = 0;

    delay = (msec << 1) | 1;

    id00003000_writeConfReg(address, port, "CDELAY", &delay, 1, 0);

    id00003000_enableINT(address, port, ID00001001_STATUS_BIT_DONE);

    return 0;
}

int32_t ID00001001_disableDelay(uint8_t address, uint8_t port)
{
    uint32_t delay = 0;

    id00003000_writeConfReg(address, port, "CDELAY", &delay, 1, 0);

    id00003000_disableINT(address, port, 0);

    id00003000_clearINT(address, port, 0);

    return 0;
}

int32_t ID00001001_startIP(uint8_t address, uint8_t port)
{
    id00003000_start(address, port);

    return 0;
}

int32_t ID00001001_writeData(uint8_t address, uint8_t port, uint32_t *data, uint32_t size, uint32_t offset)
{
    id00003000_writeMem(address, port, "MDATAIN", data, size, offset);

    return 0;
}

int32_t ID00001001_readData(uint8_t address, uint8_t port, uint32_t *data, uint32_t size, uint32_t offset)
{
    id00003000_readMem(address, port, "MDATAOUT", data, size, offset);

    return 0;
}

int32_t ID00001001_getStatus(uint8_t address, uint8_t port, uint32_t *status)
{
    id00003000_getStatus(address, port, status);

    return 0;
}

int32_t ID00001001_waitDone(uint8_t address, uint8_t port)
{
    uint8_t statusINT = 0;

    do
    {
        id00003000_getINT(address, port, &statusINT);
    } while (!(statusINT && 0x1));

    return 0;
}

static int32_t ID00001001_clearStatus(uint8_t address, uint8_t port)
{
    for(uint8_t i = 0; i < ID00001001_STATUS_BITS; i++)
    {
        id00003000_disableINT(address, port, i);
        printf("(%d:%d) Disabled INT %d \n", address, port, i);
        id00003000_clearINT(address, port, i);
        printf("(%d:%d) Cleared INT %d \n", address, port, i);
    }

    return 0;
}
