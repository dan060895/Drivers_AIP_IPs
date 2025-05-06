#ifndef ID00001007_MXV_H_
#define ID00001007_MXV_H_

#include "ID00001008_SDRAMController.h"
//#define MEM_SHARED
typedef struct {
    uint8_t mem_separated;
    uint8_t mem_intercalated;
    uint16_t max_cols;
    uint16_t max_rows;
    uint16_t complete_rows;
    uint16_t complete_columns;
    uint32_t subdiagonals;
    uint32_t corner_subdiagonals_on;
	uint16_t rows;
	uint16_t columns;
    uint16_t numPEs;
    uint16_t op_mode;
    uint16_t residual_cols;
    uint16_t quotient_cols;
    uint16_t residual_rows;
    uint16_t quotient_rows;
    uint16_t quotient;
    uint16_t residue;
    uint32_t mem_out_temp[256];
    uint32_t mem_out_temp2[256];
    uint32_t mem_out2[256];
    uint32_t mem_out[256];
    //mxv_configParam config_param;

}mxv_state;
typedef volatile struct {

	uint32_t config_reg1;
	uint32_t config_reg2;
    uint32_t config_reg3;
    uint32_t config_reg4;

}mxv_configParam;

int32_t ID00001007_init(uint8_t address, uint8_t port);

int32_t ID00001007_enableDelay(uint8_t address, uint8_t port, uint32_t msec);

int32_t ID00001007_disableDelay(uint8_t address, uint8_t port);

int32_t ID00001007_startIP(uint8_t address, uint8_t port);

int32_t ID00001007_writeData(uint8_t address,uint8_t port, uint8_t memory, uint32_t *data, uint32_t size, uint32_t offset);

int32_t ID00001007_readData(uint8_t address,uint8_t port, uint8_t memory, uint32_t *data, uint32_t size, uint32_t offset);

int32_t ID00001007_getStatus(uint8_t address, uint8_t port, uint32_t *status);

int32_t ID00001007_waitDone(uint8_t address, uint8_t port);


int32_t ID00001007_mxv_setConfig(uint8_t address, uint8_t port, mxv_state *init_param);

int32_t ID00001007_mxv_setRows(uint8_t address, uint8_t port, mxv_state *mxv_currentState, uint16_t rows);

int32_t ID00001007_mxv_setColumns(uint8_t address, uint8_t port, mxv_state *mxv_currentState,uint16_t columns);

int32_t ID00001007_mxv_do_mxv(uint8_t address, uint8_t port, mxv_state *mxv_currentState, sdramC_state *sdramC_currentState);

int32_t ID00001007_mxv_writeMatrixMems(uint8_t address, uint8_t port, mxv_state *mxv_currentState, sdramC_state *sdramC_currentState,  uint32_t initial, uint32_t real_offset);

int32_t ID00001007_mxv_writeVectorsMems(uint8_t address, uint8_t port, mxv_state *mxv_currentState, sdramC_state *sdramC_currentState, uint32_t vec_offset);

int32_t ID00001007_mxv_readData(uint8_t address, uint8_t port, mxv_state *mxv_currentState, sdramC_state *sdramC_currentState, uint8_t refill, uint8_t rewrite, uint32_t append, uint32_t accData, uint32_t backupFile, uint32_t offset);

int32_t ID00001007_mxv_setParam(uint8_t address, uint8_t port, mxv_state *mxv_currentState,uint16_t complete_rows, uint16_t complete_columns, uint16_t max_rows, uint16_t max_cols, uint16_t op_mode, uint16_t numPEs, uint32_t subdiagonals, uint32_t corner_subdiagonals_on);

int32_t ID00001007_mxv_writeDataMem(uint8_t address, uint8_t port, mxv_state *mxv_currentState,sdramC_state *sdramC_currentState, uint32_t punter, uint32_t add_mem_ip,uint32_t *dataOutSDRAM, uint32_t frame_size);








#endif //ID00001007_MXV_H_
