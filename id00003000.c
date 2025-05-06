#include "ID00003000.h"
#include "ID00003000_CONFIGURATION.h"
#include "aip.h" /* Change this library for the aip.h library when using microcontroller*/
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "system.h"
//#include "altera_avalon_pio_regs.h"

// change to aip.h

#define getID(IPID)                                 aip_getID(SLAVE_1_BASE, IPID)
#define start()                                     aip_start(SLAVE_1_BASE)
#define getStatus(status)                           aip_getStatus(SLAVE_1_BASE, status)
#define clearINT(i)                                 aip_clearINT(SLAVE_1_BASE, i)
#define finish()                                    aip_finish(SLAVE_1_BASE)
#define readMem(mem, data, size, offset)            aip_readMem(SLAVE_1_BASE, mem, data, size, offset)
#define writeMem(mem, data, size, offset)           aip_writeMem(SLAVE_1_BASE, mem, data, size, offset)
#define writeConfReg(config, datawr, size, offset)  aip_writeConfReg(SLAVE_1_BASE, config, datawr, size, offset)
#define enableINT(idxInt)                           aip_enableINT(SLAVE_1_BASE, idxInt)
extern volatile uint8_t NetStatus[8][8];

//#define PRINTF
#ifdef PRINTF
    #define LOG_PRINTF(...) printf(__VA_ARGS__)
#else
    #define LOG_PRINTF(...)
#endif

//NIC CONFIG VALUES
#define NIC_CCONFREG   0
#define NIC_TIMESTAMP  1

//NoC Commands
#define READ_IP_DATA    0
#define WRITE_IP_DATA   1
#define START_IP        2
#define READ_NIC_DATA   3
#define READ_IP2IP_DATA 5
#define WRITE_NIC_DATA  6

//NoC Manager Interruption Bits
#define INT_NETWORK_INTERRUPTION 3

#define INT_REG_SIZE        8
#define CONFIG_DATA_SIZE    2

//INT Mask Bits
#define DONE_BIT 0x00000001
#define NI_BIT   0x00000008
#define CE_BIT   0x00000010
#define MASK_INT 0x00FF0000

//Mandatory Configs
#define ID_CONFIG     0x1F
#define STATUS_CONFIG 0x1E
#define ZERO_CONFIG   0x00
#define ID00003000_CONFIG_AMOUNT 7

//Flit sizes and offsets
#define ZERO_FLIT_SIZE 0
#define ONE_FLIT_SIZE  1
#define TWO_FLIT_SIZE  2
#define ZERO_OFFSET    0
#define ONE_OFFSET     1

//Number of IPs per NIC
#define IP_MAX  8

//NoC Manager Interruptions Memory
#define NEW_START_INT_DATA     0x01000000
#define INT_FLAGS_IPCORE0_TO_2 0x00FFFFFF
#define INT_FLAGS_IPCORE3      0x000000FF
#define INT_FLAGS_IPCORE4_TO_5 0x00FFFF00
#define INT_FLAGS_IPCORE6_TO_7 0x0000FFFF
#define INT_SOURCE             0x00FF0000
#define INT_INFO_PER_FLIT      4
#define MPACKETINT_SIZE        3
#define MAX_SIZE_PACKET        511

/************************ Structs definition for driver *************************/

/* Interruption flags struct for each NoC Address*/
typedef  struct  {
    int     addr_key;
    uint8_t flags[8][8]; //[Port Number][INT flag]
                         //
                         //  INT flag  |  ID0  |   ID1   |   ID2   |   ID3  |  ID4  |   ID5   |  ID6  |  ID7  |
                         //    index   | (UD)  |  (UD)   |  (UD)   |  (UD)  | (UD)  |  (UD)   |  (UD) | (DONE)|
} INTflags_t;

/* Configs of each IP Core in the NoC*/
typedef struct  {
    char addr_key[7]; //Network address where the IP core is attached "Addr:Port"
    uint8_t amount;   //Number of configs
    aip_config_t *configs; //metadata of each config
} noc_configs_t;

/*-----CONFIGS of the NoC Manager-----*/
static aip_config_t ID00003000_csv [] = {
    {"CCONFREG"      ,  0, 'W', 2},
    {"MPACKETIN"     ,  2, 'W', NOC_MANAGER_MEM_SIZE},
    {"MDATAOUT"      ,  4, 'R', NOC_MANAGER_MEM_SIZE},
    {"MINTINDICATION",  7, 'B', 1},
    {"MINT"          ,  9, 'R', 16},
    {"STATUS"        , 30, 'B', 1},
    {"IPID"          , 31, 'R', 1}
};
/*----------------------------------*/
#define CCONFREG        0
#define MPACKETIN       2
#define MDATAOUT        4
#define MINTINDICATION  7
#define MINT            9
#define STATUS         30
#define IPID           31
/******************** Global variables (private) *****************************/
//static aip_t     *gAIP;                  //Pointer to aip
static uint32_t    gIPDI         = 0;     //ID read from NoC Manager
static uint8_t     gINT_mask     = 0x00;  //Interruption mask of the NoC Manager
static uint32_t    gActual_INT_data[MPACKETINT_SIZE]= {0x00000000};//For processing INT Memory
static uint8_t     gCounter_fifo = 0 ;
static uint8_t     gNoC_Configs_index = 0 ; //index for NoC_Configs array
static uint8_t     gNoC_INTflags_index = 0 ; //index for gNoC_INTflags array
static noc_configs_t gNoC_Configs  [NOC_IP_CORES]; //Configs of each IP core
static INTflags_t    gNoC_INTflags [NOC_NODES];    //NoC INT flags
/******************************************************************************/

/************************ Private functions declaration ***************************************/
int32_t  clearStatus(void);
static int32_t  clearStatusFlags(uint32_t read_flags);
static bool     wait_process(void);
static int32_t  processMEMINT(uint32_t elements);
static int32_t  writeNIc(uint8_t config, uint32_t *data,uint32_t data_size, uint8_t destAddr);
static int32_t  write (uint8_t config, uint32_t *data,uint32_t data_size, uint8_t destAddr, uint8_t destPort);
static int32_t  read  (uint8_t config, uint32_t *data,uint32_t data_size, uint8_t destAddr, uint8_t destPort);
static int32_t  write_packet(uint8_t command, uint8_t config, uint32_t *data, uint32_t data_size, uint8_t destAddr, uint8_t destPort);
static int32_t  readIP2IP(uint8_t rdAddr, uint8_t rdPort, uint8_t rdConfig, uint8_t wrAddr, uint8_t wrPort, uint8_t wrConfig, uint16_t size);
static uint8_t  getConfig(char *mnemonic, uint8_t addr, uint8_t port, uint8_t *configID);
static INTflags_t *find_INTflags(uint8_t addr);
/************************************************************************************************/

/***************************PUBLIC FUNCTIONS IMPLEMENTATION*******************************/
int32_t id00003000_init(uint8_t nicAddr, uint8_t port)
{
    //aip_init(port, ID00003000_csv, ID00003000_CONFIG_AMOUNT);

    /*
    gAIP = caip_init(connector, nicAddr, port, csv);

    if(gAIP == NULL){
        LOG_PRINTF("CAIP Object not created");
        return -1;
    }
    */

    getID(&gIPDI);
    clearStatus();

    //------------------------------------------
    LOG_PRINTF("\nNoC Manager created with IP ID %08X\n", gIPDI);
    return 0;
}

int32_t id00003000_config(uint32_t mask_value, uint8_t newLocalAddr, uint8_t retransmissions, uint32_t timestamp)
{
    uint32_t enable, start_config_reg;
    uint32_t conf_data[CONFIG_DATA_SIZE];
    bool comm_err;

    if(newLocalAddr < 0 || newLocalAddr > 255){
        LOG_PRINTF("Invalid local address: %d", newLocalAddr);
        //exit (EXIT_FAILURE);
    }

    //set mask value
    LOG_PRINTF("Setting mask value: %08x\n", mask_value);
    gINT_mask = mask_value;

    for(uint8_t i=0; i < 8; i++)
        if(gINT_mask&(1<<i))
            enableINT(i);


    //Feeder Collector configuration
    if(retransmissions >= 4 || retransmissions < 0){
        LOG_PRINTF("Invalid number of retransmission (0,1,2,3): %d", retransmissions);
        //exit (EXIT_FAILURE);
    }

    //Assembly start config register
    start_config_reg = newLocalAddr << 2;
    start_config_reg = start_config_reg | retransmissions;
	start_config_reg = start_config_reg<<1;

    conf_data[0]=start_config_reg;
	conf_data[1]=timestamp;

    writeConfReg(CCONFREG, conf_data, CONFIG_DATA_SIZE, ZERO_OFFSET);

    LOG_PRINTF("Configuration data: \n");
    LOG_PRINTF("Start Config Reg: %08x\n", conf_data[0]);
    LOG_PRINTF("Timestamps: %08x\n", conf_data[1]);

    start();

    comm_err=wait_process();

    if(comm_err)
    {
        LOG_PRINTF("Timeout, Feeder Collector did not enable done flag\n");
        //exit (EXIT_FAILURE);
    }

    // Feeder Collector enable to send packets
    enable = 0x00000001;
    //aip_writeConfReg(0, CCONFREG, &enable, ONE_FLIT_SIZE, ZERO_OFFSET);

    writeConfReg(CCONFREG, &enable, ONE_FLIT_SIZE, ZERO_OFFSET);
   /*
    LOG_PRINTF("Feeder Collector Configured\n");
*/
    return 0;
}



int32_t id00003000_addConfigs(uint8_t addr, uint8_t port,aip_config_t *aip_configs, uint8_t configAmount)
{
    char addr_string[4];
    char port_string[2];
    char key_value[6] ="";

    /*Generate key addr:port*/
    itoa(addr, addr_string, 10);
    itoa(port, port_string, 10);
    strcat(key_value,addr_string);
    strcat(key_value,":");
    strcat(key_value,port_string);

    strcpy(gNoC_Configs[gNoC_Configs_index].addr_key, key_value);
    gNoC_Configs[gNoC_Configs_index].amount = configAmount;
    gNoC_Configs[gNoC_Configs_index].configs = aip_configs;

    gNoC_Configs_index++;

    return 0;
}

int32_t id00003000_writeMem(uint8_t destAddr, uint8_t destPort, uint8_t configMem, uint32_t* dataWrite, uint32_t amountData, uint16_t offset)
{
    uint8_t ptr_config;
    uint32_t dwOffset;

    //getConfig(configMem, destAddr, destPort, &mem_config);
    ptr_config= configMem+1;

    //set pointer according offset value
    dwOffset = (uint32_t)offset;
    write(ptr_config, &dwOffset ,ONE_FLIT_SIZE, destAddr, destPort);
    //write data in the memory
    write(configMem, dataWrite ,amountData, destAddr, destPort);

    return 0;
}

int32_t id00003000_writeConfReg(uint8_t destAddr, uint8_t destPort, uint8_t configConfReg, uint32_t* dataWrite, uint32_t amountData, uint8_t offset)
{

    uint8_t ptr_config;
    uint32_t dwOffset;

    //getConfig(configConfReg,destAddr, destPort, &mem_config);
    ptr_config= configConfReg+1;

    //set pointer according offset value
    dwOffset = (uint32_t)offset;

    write(ptr_config, &dwOffset ,ONE_FLIT_SIZE, destAddr, destPort);
    //write data in the memory
    write(configConfReg, dataWrite ,amountData, destAddr, destPort);

    return 0;
}

int32_t id00003000_readMem(uint8_t destAddr, uint8_t destPort, uint8_t configMem, uint32_t* dataRead, uint32_t amountData, uint16_t offset)
{
    uint8_t ptr_config;
    uint32_t dwOffset;

    //getConfig(configMem,destAddr, destPort, &mem_config);
    ptr_config= configMem+1;

    //set pointer according offset value
    dwOffset = (uint32_t)offset;
    write(ptr_config, &dwOffset ,ONE_FLIT_SIZE, destAddr, destPort);
    //read data in the memory
    read(configMem, dataRead ,amountData, destAddr, destPort);
    return 0;
}

int32_t id00003000_start(uint8_t destAddr, uint8_t destPort)
{

    bool comm_err;
    //write packet in memory in
    write_packet(START_IP, ZERO_CONFIG, NULL, ZERO_FLIT_SIZE, destAddr, destPort);

    start();

    comm_err=wait_process();

    //LOG_PRINTF("(START) Sending Start Packet...\n");

    if(comm_err)
    {
        LOG_PRINTF("Timeout, Feeder Collector did not enable done flag\n");
        //exit (EXIT_FAILURE);
    }
    return 0;
}

int32_t id00003000_getID(uint8_t destAddr, uint8_t destPort, uint32_t* id)
{
    uint32_t tmp[1];
    uint32_t size_one[1];
    bool comm_err;

    size_one[0] = ONE_FLIT_SIZE;
    //write packet in memory in
    write_packet(READ_IP_DATA, ID_CONFIG, size_one, ONE_FLIT_SIZE, destAddr, destPort);

    start();

    LOG_PRINTF("(START) Sending Read Packet...\n");

    comm_err=wait_process();

    if(comm_err)
    {
        LOG_PRINTF("Timeout, Feeder Collector did not enable done flag\n");
        //exit (EXIT_FAILURE);
    }

    readMem(MDATAOUT, tmp, ONE_FLIT_SIZE, ZERO_OFFSET);

    //LOG_PRINTF("\nIDFeeder - Data obtained from MemOut\n");
    memcpy(id, tmp, 1*sizeof(uint32_t));

    return 0;
}

int32_t id00003000_getStatus(uint8_t destAddr, uint8_t destPort, uint32_t* status)
{
    uint32_t tmp[1];
    read(STATUS_CONFIG,tmp, ONE_FLIT_SIZE, destAddr, destPort);
    memcpy(status, tmp, ONE_FLIT_SIZE*sizeof(uint32_t));

    return 0;
}

int32_t id00003000_getINT(uint8_t destAddr, uint8_t destPort, uint8_t *int_flags)
{
    uint32_t tmp[1];
    uint8_t  int_tmp[1];
    read(STATUS_CONFIG,tmp, ONE_FLIT_SIZE, destAddr, destPort);

    int_tmp[0] = (uint8_t)tmp[0]&0x000000FF;
    memcpy(int_flags, int_tmp, sizeof(uint8_t));

    return 0;
}

int32_t id00003000_enableINT(uint8_t destAddr, uint8_t destPort, uint8_t idxInt)
{
   uint32_t status;
   uint32_t old_mask;
   uint32_t new_mask;
   uint32_t new_status;

   read(STATUS_CONFIG, &status, ONE_FLIT_SIZE, destAddr, destPort);
   old_mask = (status & MASK_INT) >> 16; // get mask
   new_mask = old_mask | (0x01 << idxInt);
   new_status = new_mask << 16;
   LOG_PRINTF("Status to be written: %08X\n", new_status);
   write(STATUS_CONFIG, &new_status ,ONE_FLIT_SIZE, destAddr, destPort);

   return 0;
}

int32_t id00003000_disableINT(uint8_t destAddr, uint8_t destPort, uint8_t idxInt)
{
   uint32_t status;
   uint32_t old_mask;
   uint32_t new_mask;
   uint32_t new_status;

   read(STATUS_CONFIG, &status, ONE_FLIT_SIZE, destAddr, destPort);
   old_mask = (status & MASK_INT) >> 16; // get mask
   new_mask = old_mask & (~(0x01 << idxInt));
   new_status = new_mask << 16;
   write(STATUS_CONFIG, &new_status ,ONE_FLIT_SIZE, destAddr, destPort);

   return 0;
}

int32_t id00003000_clearINT(uint8_t destAddr, uint8_t destPort, uint8_t idxInt)
{
    uint32_t status;
    uint32_t clearValue;
    uint32_t clear_bits;

    if(idxInt > 7)
    {
        LOG_PRINTF("Invalid interruption ID (0-7), selection: %d\n",idxInt);
        //exit (EXIT_FAILURE);
    }

    read(STATUS_CONFIG, &status, ONE_FLIT_SIZE, destAddr, destPort);

    clearValue = status & MASK_INT;
    clear_bits = 0x00000001 << idxInt;
    clearValue = clear_bits |clearValue;

    write(STATUS_CONFIG, &clearValue, ONE_FLIT_SIZE, destAddr, destPort);

    return 0;
}
/*
int32_t id00003000_waitNoCInterruption (void)
{
    uint32_t status;

    getStatus(&status);

    while(!(status&NI_BIT))
        getStatus(&status);

    clearINT(INT_NETWORK_INTERRUPTION);

    return 0;
}*/

int32_t id00003000_waitNoCInterruption (void)
{

    while (wait_flag==0) {
    	//printf("wait process\n");
    }

    wait_flag = 0;

    return 0;
}


/*
int32_t id00003000_finish (void)
{
   //finish();
   return 0;
}
*/

int32_t id00003000_allocateINTinfo(uint8_t addr)
{
    gNoC_INTflags[gNoC_INTflags_index].addr_key = addr;
    //init INT flags to zero
    for(uint8_t j=0; j< IP_MAX; j++)
        for(uint8_t k=0;k<INT_REG_SIZE;k++)
            gNoC_INTflags[gNoC_INTflags_index].flags[j][k]=0;


    gNoC_INTflags_index++;
    return 0;
}

int32_t id00003000_netINTHandler (void)
{
    uint32_t elements;
    uint32_t update;
    //LOG_PRINTF("-----netINTHandler----\n");
    // Read number of elements stored in the Memory INT of the NoC Manager
    readMem(MINTINDICATION, &elements, ONE_FLIT_SIZE, ZERO_OFFSET);

    //LOG_PRINTF("Number of elements stored in MEMINT: %d\n", elements);
    // Read and Processing data acquired
    processMEMINT(elements);
    // Update rdPtr of MEMINT
    update = 0x00000000;
    writeMem(MINTINDICATION, &update, ONE_FLIT_SIZE, ZERO_OFFSET);
    return 0;
}

int32_t id00003000_enableNoCInterrupts (uint8_t destAddr, uint32_t nic_timestamp_INT, uint32_t nic_timestamp_ack)
{
    uint32_t config_data[TWO_FLIT_SIZE];

    config_data[0]=nic_timestamp_INT;
    config_data[1]=nic_timestamp_ack;

    // =========== NIC CONFIGURATION==============
    // Set timestamps of the NIC
    writeNIc(NIC_TIMESTAMP, config_data, TWO_FLIT_SIZE, destAddr);
    //LOG_PRINTF("==========SET NIC TIMESTAMP SUCCESSFULLY==========\n");
    // Set Configuration Register of  the NIC (enabling interruptions)
    config_data[0]=0x00000001;
    writeNIc(NIC_CCONFREG, config_data, ONE_FLIT_SIZE, destAddr);
    //LOG_PRINTF("=======SET NIC CONFIGURATION REGISTER SUCCESSFULLY=\n");

    return 0;
}

int32_t id00003000_getNetINTStatus (uint8_t intStatus[][8],uint8_t addrs)
{
    INTflags_t *ptr=NULL;

    ptr = find_INTflags(addrs);

    if(ptr==NULL)
    {
        LOG_PRINTF("Error, address not found\n");
        //exit (EXIT_FAILURE);
    }

    for(uint8_t i=0; i < IP_MAX; i++)
        memcpy(&intStatus[i], &ptr->flags[i], INT_REG_SIZE*sizeof(uint8_t));

    return 0;
}

int32_t id00003000_clearBitNetINTStatus (uint8_t addr, uint8_t port, uint8_t index)
{
    INTflags_t *ptr=NULL;

    ptr = find_INTflags(addr);

    if(ptr==NULL)
    {
        LOG_PRINTF("Error, address not found\n");
        //exit (EXIT_FAILURE);
    }

    ptr->flags[port][index]=0;

    return 0;
}

int32_t id00003000_transferDataIP2IP(uint8_t rdAddr, uint8_t rdPort, char*  rdConfig, uint16_t rdOffset, uint8_t wrAddr, uint8_t wrPort, char*  wrConfig, uint16_t wrOffset, uint32_t sizeData)
{
    uint8_t ptr_RDconfig;
    uint8_t ptr_WRconfig;

    uint32_t dwOffset;
    uint32_t num_transactions;
    uint32_t remaining_flits;

    // Setting RD pointer
    //getConfig(rdConfig,rdAddr, rdPort, &mem_RDconfig);
    ptr_RDconfig= rdConfig+1;

    //set pointer according offset value
    dwOffset = (uint32_t)rdOffset;
    write(ptr_RDconfig, &dwOffset ,ONE_FLIT_SIZE, rdAddr, rdPort);

    // Setting WR pointer
    //getConfig(wrConfig,wrAddr, wrPort, &mem_WRconfig);
    ptr_WRconfig= wrConfig+1;

    //set pointer according offset value
    dwOffset = (uint32_t)wrOffset;
    write(ptr_WRconfig, &dwOffset ,ONE_FLIT_SIZE, wrAddr, wrPort);

    // readIP2IP instruction
    if(sizeData < MAX_SIZE_PACKET)// single transfer
    {
        readIP2IP(rdAddr, rdPort, rdConfig, wrAddr, wrPort, wrConfig, sizeData);
    }
    else //multiple transfers
    {
        num_transactions = (uint32_t) ceil((double)sizeData / (double)MAX_SIZE_PACKET);
 	    /*num_transactions = sizeData/((MAX_SIZE_PACKET));
 	    if (sizeData % MAX_SIZE_PACKET != 0)
 	    {
 	    	num_transactions++;
 	    }
        */
        for(uint32_t i=0; i < num_transactions; i++)
        {
            if (i < num_transactions - 1){ // first transactions
                readIP2IP(rdAddr, rdPort, rdConfig, wrAddr, wrPort, wrConfig, MAX_SIZE_PACKET);
            }
            else //last transaction
            {
                remaining_flits = sizeData - (MAX_SIZE_PACKET * i);
                readIP2IP(rdAddr, rdPort, rdConfig, wrAddr, wrPort, wrConfig, remaining_flits);
            }
        }
    }
    return 0;
}

/***************************PRIVATE FUNCTIONS IMPLEMENTATION*******************************/
/*Wait DONE flag after start processing*/
static bool wait_process(void)
{
    bool error   = false;
    uint32_t status;


    getStatus(&status);
    id00003000_waitNoCInterruption();

    while(!(status&DONE_BIT) && !(status&CE_BIT)){
    	//printf("hiccccccccccccccccccccccc\n");
        getStatus(&status);
    }
    //getStatus(&status);

    clearStatusFlags(status);
    //clearStatus();

    if(status&CE_BIT)
       error = true;

    return error;

/*
	bool error   = false;
    uint32_t status;

    while (!wait_flag) {
    	printf("\nwait process\n");
    }
    wait_flag = 0;

    getStatus(&status);
    clearStatusFlags(status);

    if(status&CE_BIT)
       error = true;

    return error;
*/
}

int32_t clearStatus(void)
{
    for (uint8_t i=0; i < INT_REG_SIZE; i++)
      clearINT(i);
    return 0;
}

static int32_t clearStatusFlags(uint32_t read_flags)
{
    for (uint8_t i=0; i < 5; i++)
    {
       if((read_flags & (0x1 << i)) > 0)
       {
          //LOG_PRINTF("Clearing INT Bit %d:\n", i);
          clearINT(i);
       }
    }

    return 0;
}

static int32_t write_packet(uint8_t command, uint8_t config, uint32_t *data, uint32_t data_size, uint8_t destAddr, uint8_t destPort)
{
    uint32_t header;

    if (config < 0 || config > 31){
        LOG_PRINTF("Invalid config value: %d", config);
        //exit (EXIT_FAILURE);
    }

    if(data_size > 511 || data_size < 0){
        LOG_PRINTF("Invalid data size value: %d",data_size);
        //exit (EXIT_FAILURE);
    }

    //assembly header

    header=destAddr;
	header=header << 3;
	header=header | destPort;
	header=header << 5;
	header=header | config;
	header=header << 4;
	header=header | command;
	header=header << 12;
	header=header | data_size;

    /*LOG_PRINTF("\nWriting packet with the following fields: \n");
    LOG_PRINTF("Addr: %02x \nPort: %1x \n", dest_addr,dest_port);
    LOG_PRINTF("Command: %02x \nConfig: %05x \nData Size: %09x\n\n", command,config, data_size);*/

    /*LOG_PRINTF("Packet to write:\n");
    LOG_PRINTF("Header: %08x \n",header);
    for(uint32_t i = 0; i < data_size ; i++)
    LOG_PRINTF("Body[%d]: %08x \n",i,data[i]);*/

    //write header
    writeMem(MPACKETIN, &header, ONE_FLIT_SIZE, ZERO_OFFSET); //CHANGE_d

    //write payload
    if(data_size > 0){
        writeMem(MPACKETIN, data, data_size, ONE_OFFSET);
        //aip_writeMem(0, MPACKETIN, data, data_size, ONE_OFFSET);
    }

    return 0;
}


static INTflags_t *find_INTflags(uint8_t addr)
{
    INTflags_t *ptr=NULL;

    for(uint8_t i=0; i < gNoC_INTflags_index; i++)
    {
        if(gNoC_INTflags[i].addr_key == addr)
          ptr = &gNoC_INTflags[i];
    }

    return ptr;
}

static int32_t processMEMINT(uint32_t elements)
{
    uint32_t r_data[elements];
    int addr_key;
    INTflags_t *INTarray;

    readMem(MINT, r_data, elements, ZERO_OFFSET);
    //LOG_PRINTF("Data read from MEMINT: \n");
    for(uint32_t i=0; i < elements; i++){
        //LOG_PRINTF("%08X\n",r_data[i]);
    }
    for(uint32_t i=0; i < elements; i++)
    {
        //LOG_PRINTF("Data to check : %08X (hex)\n", r_data[i]);

        if(r_data[i] & NEW_START_INT_DATA){  //new start of INT?
            gCounter_fifo = 0;
        }
        /*Storing INT Flags in gActual_INT_data as follows:

                                  [31:24]         [23:16]         [15:8]         [7:0]
            gActual_INT_data[0] FlagsIpCore3   FlagsIpCore2   FlagsIpCore1    FlagsIpCore0
            gActual_INT_data[1] FlagsIpCore7   FlagsIpCore6   FlagsIpCore5    FlagsIpCore4
            gActual_INT_data[2]                                                INT Source

        */
        // check counter value
        if(gCounter_fifo == 0)
        {
            gActual_INT_data[0] = r_data[i]&INT_FLAGS_IPCORE0_TO_2 ;//extract INT IPcore2 to INT IPcore0
        }
        else if(gCounter_fifo == 1)
        {
            gActual_INT_data[0] = gActual_INT_data[0] | ((r_data[i] & INT_FLAGS_IPCORE3)<<24);//Extracting INT IPcore 3
            gActual_INT_data[1] = (r_data[i]&INT_FLAGS_IPCORE4_TO_5)>>8;//Extracting INT IPcore 4 to INT IPCore5
        }
        else
        {
            gActual_INT_data[1] = gActual_INT_data[1] | ((r_data[i] & INT_FLAGS_IPCORE6_TO_7)<<16);//Extracting INT IPcore 6 to 7
            gActual_INT_data[2] = (r_data[i]&INT_SOURCE) >> 16; //extracting source of the interrupt

            addr_key = (int)gActual_INT_data[2];
            LOG_PRINTF("INT detected in NIC addr: %d\n", addr_key);
            /*LOG_PRINTF("New Interruption data: \n");

            for(uint8_t i=0; i < MPACKETINT_SIZE; i++)
                LOG_PRINTF("%08X: \n",gActual_INT_data[i]);

            LOG_PRINTF("... Updating status IPs Reg \n");*/

            INTarray = find_INTflags(addr_key);

            if (INTarray == NULL)
            {
                LOG_PRINTF("Wrong destination source %d:", addr_key);
                //exit (EXIT_FAILURE);
            }

            for(uint8_t i=0; i< IP_MAX; i++)
            {
                if(i<4){//data from gActual_INT_data[0]
                    for(uint8_t j=0; j < INT_REG_SIZE; j++){
                        INTarray->flags[i][INT_REG_SIZE-1-j]= gActual_INT_data[0]&(0x00000001 << (i*8+j)) ? 1:0;
                    }
                }
                else
                {//data from gActual_INT_data[1]
                    for(uint8_t j=0; j < INT_REG_SIZE; j++){
                        INTarray->flags[i][INT_REG_SIZE-1-j]= (gActual_INT_data[1]&(0x00000001 << (((i-4)*8)+j))) ? 1:0;
                    }
                }
            }

            LOG_PRINTF("New Status in NIC with address: %d\n", addr_key);
            for(uint8_t i=0; i< IP_MAX; i++)//WARN:change IP_MAX to 1
            {
               for(uint8_t j=0; j < INT_REG_SIZE; j++){
                    LOG_PRINTF("%d, ",INTarray->flags[i][j]);
               }
               LOG_PRINTF("\n");
            }
        }

        gCounter_fifo = gCounter_fifo + 1;
    }

    return 0;
}

static int32_t writeNIc(uint8_t config, uint32_t *data,uint32_t data_size, uint8_t destAddr)
{
    bool comm_err;

    write_packet(WRITE_NIC_DATA, config, data, data_size, destAddr, 0);
	clearStatus();
    start();
    //LOG_PRINTF("(START) Sending Read Packet...\n");

    comm_err=wait_process();

    if(comm_err)
    {
        LOG_PRINTF("Timeout, Feeder Collector did not enable done flag\n");
        //exit (EXIT_FAILURE);
    }
    //LOG_PRINTF("WRITE NIC SUCCESFULLY...\n");
    return 0;
}

static int32_t write(uint8_t config, uint32_t *data, uint32_t data_size, uint8_t destAddr, uint8_t destPort)
{
    bool comm_err;
    uint32_t num_packets;
    uint32_t index;
    uint32_t remaining_flits;

    if(data_size < NOC_MANAGER_MEM_SIZE) // data segmentation no required
    {
       //write packet in memory in
       write_packet(WRITE_IP_DATA, config, data, data_size, destAddr, destPort);

       start();
       //LOG_PRINTF("(START) Sending Write Packet...\n");

       comm_err=wait_process();

       if(comm_err)
       {
           LOG_PRINTF("Timeout, Feeder Collector did not enable done flag\n");
           //exit (EXIT_FAILURE);
       }
    }
    else //data segmentation
    {

       num_packets = (uint32_t) ceil((double)data_size/((double)(NOC_MANAGER_MEM_SIZE-1)));
 	   /*num_packets = data_size/((NOC_MANAGER_MEM_SIZE));
 	   if (data_size % NOC_MANAGER_MEM_SIZE != 0)
 	   {
 	      num_packets++;
 	   }*/
       //printf("Numero de paquetes = %i\n",num_packets);
       index = 0;

       for(uint32_t i=0; i < num_packets; i++)
       {
          if(i == num_packets-1) //last packet
          {
             remaining_flits = data_size - index;
             write_packet(WRITE_IP_DATA, config, &data[index], remaining_flits, destAddr, destPort);
          }
          else //first packets
             write_packet(WRITE_IP_DATA, config, &data[index], NOC_MANAGER_MEM_SIZE-1, destAddr, destPort);

          start();
          //LOG_PRINTF("(START) Sending Write Packet...\n");

          comm_err=wait_process();
          index = index + NOC_MANAGER_MEM_SIZE - 1;

          if(comm_err)
          {
              LOG_PRINTF("Timeout, Feeder Collector did not enable done flag\n");
              //exit (EXIT_FAILURE);
          }

       }
    }
    return 0;
}

static int32_t read(uint8_t config, uint32_t *data,uint32_t data_size, uint8_t destAddr, uint8_t destPort)
{
    uint32_t tmp[data_size];
    uint32_t segment[NOC_MANAGER_MEM_SIZE];
    bool comm_err;
    uint32_t num_packets;
    uint32_t index;
    uint32_t aux_size;

    if(data_size < NOC_MANAGER_MEM_SIZE ) // data segmentation no required
    {
       //write packet in memory in
       write_packet(READ_IP_DATA, config, &data_size, ONE_FLIT_SIZE, destAddr, destPort);

       start();

       //LOG_PRINTF("(START) Sending Read Packet...\n");

       comm_err=wait_process();
       if(comm_err)
       {
           LOG_PRINTF("Timeout, Feeder Collector did not enable done flag\n");
           //exit (EXIT_FAILURE);
       }

       readMem(MDATAOUT, tmp, data_size, ZERO_OFFSET);
       //LOG_PRINTF("\nIDFeeder - Data obtained from MemOut\n");
       memcpy(data, tmp, data_size * sizeof(uint32_t));
       //for(uint32_t i=0; i<data_size; i++){
           //dataout[i] = i;
           //LOG_PRINTF("%03d %08X\n", i, tmp[i]);
       //}
    }
    else // data segmentation required
    {
       num_packets = (uint32_t) ceil((double)data_size/((double)(NOC_MANAGER_MEM_SIZE)));
	   /*num_packets = data_size/((NOC_MANAGER_MEM_SIZE));
	   if (data_size % NOC_MANAGER_MEM_SIZE != 0)
	   {
	      num_packets++;
	   }*/
	  // printf("Numero de paquetes to read= %i\n",num_packets);

	   index = 0;

       for(uint32_t i=0; i < num_packets; i++)
       {
          if(i == num_packets-1)//last packet
            aux_size = data_size - index;
          else //first packet
            aux_size = NOC_MANAGER_MEM_SIZE;

          //write packet in memory in
          write_packet(READ_IP_DATA, config, &aux_size, ONE_FLIT_SIZE, destAddr, destPort);

          start();
          //LOG_PRINTF("(START) Sending Read Packet...\n");

          comm_err=wait_process();
          if(comm_err)
          {
              LOG_PRINTF("Timeout, Feeder Collector did not enable done flag\n");
              //exit (EXIT_FAILURE);
          }

          // Get processed data
          readMem(MDATAOUT, segment, aux_size, ZERO_OFFSET);
          //LOG_PRINTF("\nIDFeeder - Data obtained from MemOut\n");
          memcpy(&tmp[index], segment, aux_size * sizeof(uint32_t));
          /*for(uint32_t i=0; i<data_size; i++){
              //dataout[i] = i;
              LOG_PRINTF("%03d %08X\n", i, tmp[i]);
          }*/
          index = index + aux_size;
       }
       memcpy(data, tmp, data_size * sizeof(uint32_t));

    }

   return 0;
}

static int32_t readIP2IP(uint8_t rdAddr, uint8_t rdPort, uint8_t rdConfig, uint8_t wrAddr, uint8_t wrPort, uint8_t wrConfig, uint16_t size)
{
   uint32_t write_header;
   uint32_t body_flits[2];
   bool comm_err;

   //generating information about data to write
	write_header = wrAddr;
	write_header = write_header << 3;
	write_header = write_header | wrPort;
	write_header = write_header << 5;
	write_header = write_header | wrConfig;
	write_header = write_header << 4;
	write_header = write_header | WRITE_IP_DATA;
	write_header = write_header << 12;
	write_header = write_header | size;

   body_flits[0] = size;
   body_flits[1] = write_header;

   //write packet in memory in
   write_packet(READ_IP2IP_DATA, rdConfig, body_flits, TWO_FLIT_SIZE, rdAddr, rdPort);

   start();

   comm_err=wait_process();

   if(comm_err)
   {
       LOG_PRINTF("Timeout, Feeder Collector did not enable done flag\n");
       //exit (EXIT_FAILURE);
   }

   return 0;
}
