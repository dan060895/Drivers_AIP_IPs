#ifndef __ID00003000_H__
#define __ID00003000_H__

#include <stdint.h>
#include "aip.h" /* Uncomment when using the aip.h library for microcontroller*/


extern volatile uint8_t wait_flag;



/* Remove this structure when including the aip.h library for microcontroller*/

/*#define MNEMONIC_MAX_LEN 16
typedef struct aip_config
{
    char mnemonic[MNEMONIC_MAX_LEN];
    uint8_t config;
    uint8_t mode;
    uint16_t size;
} aip_config_t;
*/



/* ************************************************************************ */

int32_t clearStatus(void);
/** Public functions declaration */

/* Driver initialization*/
// @param csv           NoC Manager CSV path (id00003000_config.csv)
// @param destAddr      Network address IP
// @param destPort      Network port
// @param connector     USB Bridge device
// @param aip_mem_size  Data Memory size of the NoC Manager
int32_t id00003000_init( uint8_t destAddr, uint8_t destPort);

/* Close connection with the USB Bridge*/
//int32_t id00003000_finish (void);

/* Associate an IP Core Configs Structure to an NoC address*/
//
// @param addr          IP core address
// @param port          IP core port
// @param aip_configs   Pointer to the configs struct
int32_t id00003000_addConfigs(uint8_t addr, uint8_t port,aip_config_t *aip_configs, uint8_t configAmount);

/* Initialize INT flags register for a specific NoC address*/
//
// @param addr  NoC Address
int32_t id00003000_allocateINTinfo(uint8_t addr);

/* Config NoC manager*/
// @param mask_value        New Mask for Interrupt flags of the Status Register
// @param newLocalAddr      New Local Address of the NoC Manager
// @param retransmissions   Number of retransmissions to send if errors ocurr
// @param timestamp         New timestamp value (in clock cycles) to wait for an acknowledge or response packet according to the NoC protocol
int32_t id00003000_config (uint32_t mask_value, uint8_t const newLocalAddr, uint8_t const retransmissions, uint32_t const timestamp);

/* Extract NoC interrupt flags from NoC Manager INT memory
   This method must be used when the interrupt flag NI (network interruption) is detected.
   This function process the interruptions memory (INT Memory) to extract INT flags issued from an NoC Address.
*/
int32_t id00003000_netINTHandler (void);

/* Polling NoC interruption (NI flag)*/
int32_t id00003000_waitNoCInterruption (void);

/*Enable interruptions on a specific NoC address*/
// @param destAddr            Destination Address where the interruptions will be attended
// @param nic_timestamp_INT   NIC timestamp in clock cycles for polling interruptions
// @param nic_timestamp_ack   NIC timestamp in clock cycles for waiting acknowledge from interrupt packets
int32_t id00003000_enableNoCInterrupts (uint8_t destAddr, uint32_t nic_timestamp_INT, uint32_t nic_timestamp_ack);

/* Get interrupt flags from an specific NoC address*/
//
// @param intStatus  2D array with Interrupt flags of an specific NoC Address
// @param addrs      Network Address where the interrupt flags will be read
int32_t id00003000_getNetINTStatus (uint8_t intStatus[][8],uint8_t addrs);

/* Clear interrupt flags from an specific IP core
   This function must be called to clear a specific INT flag in order to detect new interrupts from the bit.*/
// @param addrs   Network Address where the IP core is connected
// @param port    Network Port where the IP core is connected
// @param index   INT flag index to clearing the bit:
//                        ________________________________________________________________________
//                 index  |  ID0  |   ID1   |   ID2   |   ID3  |  ID4  |   ID5   |  ID6  |  ID7  |
//                        | (UD)  |  (UD)   |  (UD)   |  (UD)  | (UD)  |  (UD)   |  (UD) | (DONE)|
int32_t id00003000_clearBitNetINTStatus (uint8_t addr, uint8_t port, uint8_t index);

/*AIP Functions*/
int32_t id00003000_getID(uint8_t destAddr, uint8_t destPort, uint32_t* id);
int32_t id00003000_getStatus(uint8_t destAddr, uint8_t destPort, uint32_t* status);
int32_t id00003000_start(uint8_t destAddr, uint8_t destPort);
int32_t id00003000_writeMem(uint8_t destAddr, uint8_t destPort, uint8_t configMem, uint32_t* dataWrite, uint32_t amountData, uint16_t offset);
int32_t id00003000_readMem(uint8_t destAddr, uint8_t destPort, uint8_t configMem, uint32_t* dataRead, uint32_t amountData, uint16_t offset);
int32_t id00003000_writeConfReg( uint8_t destAddr, uint8_t destPort, uint8_t configConfReg, uint32_t* dataWrite, uint32_t amountData, uint8_t offset);
int32_t id00003000_enableINT(uint8_t destAddr, uint8_t destPort, uint8_t idxInt);
int32_t id00003000_disableINT(uint8_t destAddr, uint8_t destPort, uint8_t idxInt);
int32_t id00003000_getINT(uint8_t destAddr, uint8_t destPort, uint8_t *int_flags);
int32_t id00003000_clearINT(uint8_t destAddr, uint8_t destPort, uint8_t idxInt);

/* Read data from an IP core and transfer it to another IP core*/
// @param rdAddr     Destination Address for read data
// @param rdConfig   Destination configuration for read data
// @param rdOffset   Destination offset for read data
// @param wrAddr     Destination Address for write data
// @param wrConfig   Destination configuration for write data
// @param wrOffset   Destination offset for write data
// @param size       Number of flits to be read/write
int32_t id00003000_transferDataIP2IP(uint8_t rdAddr, uint8_t rdPort, char*  rdConfig, uint16_t rdOffset, uint8_t wrAddr, uint8_t wrPort, char*  wrConfig, uint16_t wrOffset, uint32_t sizeData);

#endif // __ID00003000_H__
