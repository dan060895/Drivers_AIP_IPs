#include "id00003000.h"
#include "ID00001007_MxV.h"
#include "ID00001008_SDRAMController.h"
#include "debug.h"
#include <sys/alt_irq.h>
#include <time.h>
#include <sys/alt_alarm.h>

#include "io.h"




#include "altera_avalon_timer_regs.h"

/*
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
*/
#define ID00001007_STATUS_BITS 8
#define ID00001007_STATUS_BIT_DONE 0
#define ID00001007_CONFIG_AMOUNT 17

#define ADDR0 0
#define ADDR1 1
#define PORT0 0
#define PORT1 1
#define PORT2 2

static aip_config_t ID00001007_csv [] = {
    {"MMEMIN0", 0, 'W', 256},
    {"MMEMIN1", 2, 'W', 16384},
    {"MMEMIN2", 4, 'W', 16384},
    {"MMEMIN3", 6, 'W', 16384},
    {"MMEMIN4", 8, 'W', 16384},
    {"MMEMIN5", 10, 'W', 16384},
    {"MMEMIN6", 12, 'W', 16384},
    {"MMEMIN7", 14, 'W', 16384},
    {"MMEMIN8", 16, 'W', 16384},
    {"MMEMIN9", 18, 'W', 16384},
    {"MMEMIN10", 20, 'W', 16384},
    {"MMEMIN11", 22, 'W', 16384},
    {"MMEMOUT0", 24, 'R', 256},
    {"MMEMOUT1", 26, 'R', 256},
    {"CCONFREGMXV", 28, 'W', 4},
    {"STATUS", 30, 'B', 1},
    {"IPID", 31, 'R', 1}
};

#define MEM_SHARED
//#define SPARSE_MATRIX

#define DEBUG_LEVEL 1
#define DEBUG_LEVEL2 2


static int32_t ID00001007_clearStatus(uint8_t address, uint8_t port);


int32_t ID00001007_init(uint8_t address, uint8_t port)
{
    uint32_t id;

    //Adding configs data to the NoC Manager *MANDATORY WHEN USING NOC MANAGER*/
    id00003000_addConfigs(address, port, ID00001007_csv, ID00001007_CONFIG_AMOUNT);

    id00003000_getID(address, port, &id);
    dev_dbg("IPMXV","INIT: ID READ : %08X\n", id);

    ID00001007_clearStatus(address, port);
    //ID00001008_disableStatus(address, port);
    id00003000_enableINT(address, port, 0);
   // id00003000_enableINT(address, port, 1);
   // id00003000_enableINT(address, port, 2);
    //id00003000_enableINT(address, port, 3);
    //id00003000_enableINT(address, port, 4);
   // id00003000_enableINT(address, port, 5);
    //id00003000_enableINT(address, port, 6);
    //id00003000_enableINT(address, port, 7);
    return 0;
}


int32_t ID00001007_startIP(uint8_t address, uint8_t port)
{
	id00003000_start(address, port);

    return 0;
}

int32_t ID00001007_writeData(uint8_t address, uint8_t port, uint8_t memory, uint32_t *data, uint32_t size, uint32_t offset)
{
	//const char *mnemonic = ID00001007_csv[memory].mnemonic;
    dev_dbg("IPMXV","MemIn: %d | Size: %d | Add_mem:%d \n",  memory, size, offset);

    id00003000_writeMem(address, port, memory*2, data, size, offset);

    return 0;
}

int32_t ID00001007_readData(uint8_t address, uint8_t port, uint8_t memory, uint32_t *data, uint32_t size, uint32_t offset)
{
	//const char *mnemonic = ID00001007_csv[memory + 12].mnemonic;
   
    id00003000_readMem(address, port, memory*2 + 24 , data, size, offset);

    return 0;
}

int32_t ID00001007_getStatus(uint8_t address, uint8_t port, uint32_t *status)
{
    id00003000_getStatus(address, port, status);

    return 0;
}

int32_t ID00001007_waitDone(uint8_t address, uint8_t port)
{
    while(NetStatus[1][7] != 1) // IP 0, Done flag (7) == 1?
    {
      	 id00003000_netINTHandler();
         id00003000_getNetINTStatus(NetStatus,address);
    }

  	id00003000_clearBitNetINTStatus(address, port, 7);
  	//id00003000_clearBitNetINTStatus(address, port, 6);
  	//id00003000_clearBitNetINTStatus(address, port, 5);
  	id00003000_getNetINTStatus(NetStatus,address);
    return 0;



}

static int32_t ID00001007_clearStatus(uint8_t address, uint8_t port)
{
    for(uint8_t i = 0; i < ID00001007_STATUS_BITS; i++)
    {
        id00003000_disableINT(address, port, i);
        dev_dbg("IPMXV", "Disabled INT %d \n", i);
        id00003000_clearINT(address, port, i);
        dev_dbg("IPMXV", "Cleared INT %d \n", i);
    }

    return 0;
}


int32_t ID00001007_mxv_setParam(uint8_t address, uint8_t port, mxv_state *mxv_currentState, uint16_t complete_rows, uint16_t complete_columns, uint16_t max_rows, uint16_t max_cols, uint16_t op_mode, uint16_t numPEs, uint32_t subdiagonals, uint32_t corner_subdiagonals_on){

	mxv_currentState->complete_rows = complete_rows;
	mxv_currentState->complete_columns = complete_columns;
	mxv_currentState->max_cols = max_cols;
	mxv_currentState->max_rows = max_rows;
	mxv_currentState->op_mode = op_mode;
	mxv_currentState->numPEs = numPEs;
    mxv_currentState->subdiagonals = subdiagonals;
    mxv_currentState->corner_subdiagonals_on = corner_subdiagonals_on;

	return 0;
}


int32_t  ID00001007_mxv_setConfig(uint8_t address, uint8_t port, mxv_state *mxv_currentState) {

	mxv_configParam configParam;
	mxv_currentState->quotient = (mxv_currentState->columns)/(mxv_currentState->numPEs);
	mxv_currentState-> residue = (mxv_currentState->columns)%(mxv_currentState->numPEs);

	if (mxv_currentState->columns >= mxv_currentState->numPEs){
		configParam.config_reg1 = mxv_currentState-> residue;
	} else {
		configParam.config_reg1 = 0;
	}

	if (mxv_currentState->op_mode == 1) {
		configParam.config_reg1 = configParam.config_reg1+16;
	} else if(mxv_currentState->op_mode == 2) {
		configParam.config_reg1 = configParam.config_reg1+32;
	} else if(mxv_currentState->op_mode == 3) {
		configParam.config_reg1 = configParam.config_reg1+48;
	}

	if (mxv_currentState->rows >= mxv_currentState->numPEs){
		configParam.config_reg2 = mxv_currentState->rows;
	} else {
		configParam.config_reg2 = mxv_currentState->numPEs;
	}

	if (mxv_currentState->columns >= mxv_currentState->numPEs){
		configParam.config_reg3 = mxv_currentState->columns;
	} else {
		configParam.config_reg3 = mxv_currentState->numPEs;
	}

	if (mxv_currentState->columns >= mxv_currentState->numPEs){
		configParam.config_reg4 = mxv_currentState-> quotient;
	} else {
		configParam.config_reg4 = 1;
	}

	id00003000_writeConfReg(address, port, CCONFREGMXV, &configParam, 4, 0);
	dev_dbg("IPMXV","ConfReg1: %d | ConfReg2: %d | ConfReg3: %d | ConfReg4: %d\n",
			configParam.config_reg1, configParam.config_reg2, configParam.config_reg3, configParam.config_reg4);

	return 0;
}

int32_t ID00001007_mxv_setRows(uint8_t address, uint8_t port, mxv_state *mxv_currentState, uint16_t rows) {

#ifndef SPARSE_MATRIX 

	if (mxv_currentState->complete_rows > mxv_currentState-> max_rows){
		mxv_currentState->residual_rows = (mxv_currentState->complete_rows) % (mxv_currentState->max_rows);
		mxv_currentState->quotient_rows = (mxv_currentState->complete_rows) / (mxv_currentState->max_rows);

		if (rows==0)
			mxv_currentState->rows = mxv_currentState->max_rows;
		else
			mxv_currentState->rows = rows;
	}else{
		mxv_currentState->residual_rows = 0;
		mxv_currentState->quotient_rows = 1;
		mxv_currentState->rows = mxv_currentState->complete_rows;
	}
#else
		mxv_currentState->rows = rows;
#endif
	return 0;
}

int32_t ID00001007_mxv_setColumns(uint8_t address, uint8_t port, mxv_state *mxv_currentState, uint16_t columns) {
#ifndef SPARSE_MATRIX 

	if (mxv_currentState->complete_columns > mxv_currentState-> max_cols){
		mxv_currentState->residual_cols = (mxv_currentState->complete_columns) % (mxv_currentState->max_cols);
		mxv_currentState->quotient_cols = (mxv_currentState->complete_columns) / (mxv_currentState->max_cols);

		if (columns==0)
			mxv_currentState->columns = mxv_currentState->max_cols;
		else
			mxv_currentState->columns = columns;
	}else{
		mxv_currentState->residual_cols = 0;
		mxv_currentState->quotient_cols = 1;
		mxv_currentState->columns = mxv_currentState->complete_columns;
	}
#else
		mxv_currentState->columns = columns;
#endif
	return 0;
}



int32_t ID00001007_mxv_do_mxv(uint8_t address, uint8_t port, mxv_state *mxv_currentState, sdramC_state *sdramC_currentState){

    ID00001007_mxv_setRows(address, port, mxv_currentState, 0);
	ID00001007_mxv_setColumns(address, port, mxv_currentState, 0);

	uint32_t adjCol = 0;
	uint32_t adjRow = 0;
	uint32_t adjColEnd = 0;
	uint32_t adjRowEnd = 0;
	uint32_t limitIterRows = 0;
	uint32_t limitIterCols = 0;
	uint32_t init = 0;
	uint32_t vec_offset = 0;
	uint32_t idx2 = 0;
	uint32_t idx1 = 0;
	uint32_t offset = 0;
	uint8_t rewrite = 0;
	uint8_t append = 0;
	uint8_t backupFile = 0;
	uint8_t accData = 0;
	uint8_t refillData = 0;
	int32_t ret = 0;


#ifndef SPARSE_MATRIX

	if (mxv_currentState->residual_rows > 0){
		limitIterRows =  mxv_currentState-> quotient_rows + 1;
	}else{
		limitIterRows = mxv_currentState-> quotient_rows;
	}

    if (mxv_currentState->residual_cols > 0){
		limitIterCols =  mxv_currentState-> quotient_cols + 1;
	}else{
		limitIterCols = mxv_currentState-> quotient_cols;
	}

    for (uint32_t idx2 = 0; idx2 < limitIterCols; idx2++ ){
		if (idx2 == limitIterCols -2){
			if(mxv_currentState-> residual_cols < mxv_currentState->numPEs){
				adjCol = mxv_currentState-> numPEs - mxv_currentState-> residual_cols;
			}
			adjColEnd = 0;
			ID00001007_mxv_setColumns(address, port, mxv_currentState, mxv_currentState-> max_cols - adjCol);
		}else if(idx2 == limitIterCols -1){
			adjColEnd = adjCol;
			ID00001007_mxv_setColumns(address, port, mxv_currentState, mxv_currentState-> residual_cols + adjCol);
		}else{
			adjColEnd = 0;
			ID00001007_mxv_setColumns(address, port, mxv_currentState, 0);
		}

		init = (mxv_currentState-> complete_rows * mxv_currentState-> max_cols*idx2) - (adjColEnd * mxv_currentState-> complete_rows);
		vec_offset = (mxv_currentState-> max_cols * idx2) - adjColEnd;

	    for(idx1 = 0; idx1 < limitIterRows; idx1++){
			if (idx1 == (limitIterRows -2)){
				if(mxv_currentState-> residual_rows < mxv_currentState->numPEs){
					adjRow = mxv_currentState-> numPEs - mxv_currentState-> residual_rows;
				}
				adjRowEnd = 0;
				ID00001007_mxv_setRows(address, port, mxv_currentState, (mxv_currentState-> max_cols - adjRow));
			}else if(idx1 == (limitIterRows -1)){
				adjRowEnd = adjRow;
				ID00001007_mxv_setRows(address, port, mxv_currentState, (mxv_currentState-> residual_rows +adjRow));

			}else{
				ID00001007_mxv_setRows(address, port, mxv_currentState,0);
				adjRowEnd = 0;
			}

			ID00001007_mxv_setConfig(address, port, mxv_currentState);
			offset =  (mxv_currentState-> max_rows * idx1) - adjRowEnd;




            if(idx1==0){
				ID00001007_mxv_writeVectorsMems(address, port, mxv_currentState, sdramC_currentState, vec_offset);
            }


			ID00001007_mxv_writeMatrixMems(address, port, mxv_currentState, sdramC_currentState, init,offset);
			printf("INICIANDO MEDICION\n");
		    alt_u64 elapsed_time,start, end;
		    uint32_t snap_0, snap_1, snap_2, snap_3;
		    IOWR_ALTERA_AVALON_TIMER_SNAP_0 (TIMER_0_BASE, 0);
		    snap_0 = IORD(TIMER_0_BASE, 6) & ALTERA_AVALON_TIMER_SNAP_0_MSK;
		    snap_1 = IORD(TIMER_0_BASE, 7) & ALTERA_AVALON_TIMER_SNAP_0_MSK;
		    snap_2 = IORD(TIMER_0_BASE, 8) & ALTERA_AVALON_TIMER_SNAP_0_MSK;
		    snap_3 = IORD(TIMER_0_BASE, 9) & ALTERA_AVALON_TIMER_SNAP_0_MSK;
		    start = (0xFFFFFFFFFFFFFFFFULL - ( (snap_3 << 48) | (snap_2 << 32) | (snap_1 << 16) | (snap_0) ));

		    ID00001007_startIP(address, port);

			//id00003000_start(address, port);

		    IOWR_ALTERA_AVALON_TIMER_SNAP_0 (TIMER_0_BASE, 0);
		    snap_0 = IORD(TIMER_0_BASE, 6) & ALTERA_AVALON_TIMER_SNAP_0_MSK;
		    snap_1 = IORD(TIMER_0_BASE, 7) & ALTERA_AVALON_TIMER_SNAP_0_MSK;
		    snap_2 = IORD(TIMER_0_BASE, 8) & ALTERA_AVALON_TIMER_SNAP_0_MSK;
		    snap_3 = IORD(TIMER_0_BASE, 9) & ALTERA_AVALON_TIMER_SNAP_0_MSK;
		    end = (0xFFFFFFFFFFFFFFFFULL - ( (snap_3 << 48) | (snap_2 << 32) | (snap_1 << 16) | (snap_0) ));
		    elapsed_time = end - start;
		    printf("Tiempodeejecucioooon : %llu ticks\n", elapsed_time);

		    ID00001007_waitDone(address,port);

            ID00001007_readData(address, port, 0, mxv_currentState-> mem_out , mxv_currentState-> columns, 0);


			if (idx1 == 0){
				rewrite = 1;
				append = 0;
			}else{
				rewrite = 0;
				append = 1;
			}

			if (idx1 == 0 & idx2 > 0){
				backupFile = 1 ;
			}else{
				backupFile=0;
			}

			if (idx2 > 0){
                accData = 1;
			}else{
				accData = 0;
			}

			ret = ID00001007_mxv_readData(address, port, mxv_currentState,  sdramC_currentState,refillData, rewrite, append, accData,  backupFile, offset);

			if (ret != 0)
			{
				dev_dbg("IPMXV", "Error reading data\n");
			}

		}

	}
#endif

#ifdef SPARSE_MATRIX

	//Obtener cantidad de submatrices diagonales (CSMD)
	uint32_t CSMD = mxv_currentState-> complete_rows/(mxv_currentState-> subdiagonals + 1);  //floor division //
	uint32_t CSMS;
	uint32_t CSMSI;
	uint32_t CSMSM;
	uint32_t CSMSF;
	uint32_t CSMF;
	uint32_t ffloor;
	uint32_t fceil;
	uint32_t fcut;
	uint32_t ccut;

	//Obtener cantidad de submatrices secundarias (CSMS)
	if (CSMD > 2)
	{
		CSMS = CSMD - 2;
	}
	else
	{
		CSMS = 0;
	}

	//Obtener cantidad de submatrices secundarias iniciales (CSMSI)
	if (CSMS > 0)
	{
		CSMSI = 1;
	}
	else
	{
		CSMSI = 0;
	}

	//Obtener cantidad de submatrices secundarias medias (CSMSM)
	if (CSMS > 2)
	{
		CSMSM = CSMS - 2;
	}
	else
	{
		CSMSM = 0;
	}

	//Obtener cantidad de submatrices secundarias finales (CSMSF)
	if (CSMS > 1)
	{
		CSMSF = 1;
	}
	else
	{
		CSMSF = 0;
	}

	//Obtener cantidad de submatrices finales (CSMF)
	if (CSMD > 1)
	{
		CSMF = 1;
	}
	else
	{
		CSMF = 0;
	}

	uint32_t filas;
	uint32_t columnas;

	//Hacer submatriz inicial
	if (CSMD > 1)
	{
		filas = mxv_currentState-> subdiagonals + 1;
		columnas = (mxv_currentState-> subdiagonals* 2) + 1;
	}
	else
	{
		filas = mxv_currentState-> complete_rows;
		columnas = mxv_currentState-> complete_columns;
	}

	dev_dbg("IPMXV","CSMD = %i, CSMS = %i, CSMSI = %i, CSMSM = %i, CSMSF = %i, CSMF = %i \n", CSMD, CSMS, CSMSI, CSMSM, CSMSF, CSMF);
	init = 0;
	vec_offset = 0;
	offset = 0;
	rewrite = 1;
	append = 0;
	accData = 0;
	backupFile = 0;
	refillData = 0;

	ID00001007_mxv_setRows(address, port, mxv_currentState, filas );
	ID00001007_mxv_setColumns(address, port, mxv_currentState, columnas);

	ID00001007_mxv_setConfig(address, port, mxv_currentState);
	ID00001007_mxv_writeVectorsMems(address, port, mxv_currentState, sdramC_currentState, vec_offset);
	ID00001007_mxv_writeMatrixMems(address, port, mxv_currentState, sdramC_currentState, init, offset);
    ID00001007_startIP(address, port);
	usleep(100);
	ID00001007_mxv_readData(address, port, mxv_currentState, sdramC_currentState, refillData, rewrite, append, accData, backupFile, offset);
	// ADD_CODE : En esta seccion se espera limpiar la interrupcion

	// Hacer submatrices secundarias

	/* Hacer submatriz secundaria inicial*/
	if (CSMS >= 1)
	{
		filas = mxv_currentState-> subdiagonals + 1;
		columnas = (mxv_currentState-> subdiagonals * 2) + 1 + mxv_currentState-> subdiagonals;
	}


	init = 1 * mxv_currentState-> complete_rows;
	vec_offset = 1;
	offset = (mxv_currentState-> subdiagonals + 1);

	if (filas > 3)
	{
		for (uint8_t jj = 0; jj < 2; jj++)
		{
			ffloor = filas / 2;
			fceil = (filas / 2);
			if (filas % 2 != 0)
			{
				fceil++;
			}

			if (jj == 0)
			{
				fcut = ffloor;
			}
			else
			{
				fcut = fceil;
				init = init + (ffloor * mxv_currentState->complete_rows);
				vec_offset = vec_offset + ffloor;
				offset = offset + ffloor;
			}

			ccut = (mxv_currentState-> subdiagonals * 2) + fcut;
			ID00001007_mxv_setRows(address, port, mxv_currentState, fcut);
			ID00001007_mxv_setColumns(address, port, mxv_currentState, ccut);
			ID00001007_mxv_setConfig(address, port, mxv_currentState);

			//tic = timeit.default_timer()

			rewrite = 0;
			append = 1;
			accData = 0;
			backupFile = 0;
			refillData = 0;

			ID00001007_mxv_writeVectorsMems(address, port, mxv_currentState, sdramC_currentState, vec_offset);
			ID00001007_mxv_writeMatrixMems(address, port, mxv_currentState, sdramC_currentState, init, offset);
			ID00001007_startIP(address, port);
			/// ADD_CODE : En esta seccion se espera el done lo que requiere de la interrupcion
			usleep(100);
	        ID00001007_mxv_readData(address, port, mxv_currentState, sdramC_currentState, refillData, rewrite, append, accData, backupFile, offset);
			// ADD_CODE : En esta seccion se espera limpiar la interrupcion
		}
    }

	else
	{
		ID00001007_mxv_setRows(address, port, mxv_currentState, filas);
		ID00001007_mxv_setColumns(address, port, mxv_currentState, columnas);
		ID00001007_mxv_setConfig(address, port, mxv_currentState);

		rewrite = 0;
		append = 1;
		accData = 0;
		backupFile = 0;
		refillData = 0;

		ID00001007_mxv_writeVectorsMems(address, port, mxv_currentState, sdramC_currentState, vec_offset);
		ID00001007_mxv_writeMatrixMems(address, port, mxv_currentState, sdramC_currentState, init, offset);
		ID00001007_startIP(address, port);
		/// ADD_CODE : En esta seccion se espera el done lo que requiere de la interrupcion
		usleep(100);
    	ID00001007_mxv_readData(address, port, mxv_currentState, sdramC_currentState, refillData, rewrite, append, accData, backupFile, offset);
		// ADD_CODE : En esta seccion se espera limpiar la interrupcion
	}

	//Hacer submatrices secundarias medias
	if (CSMSM > 0){
		filas = mxv_currentState-> subdiagonals + 1;
		columnas = (mxv_currentState-> subdiagonals * 2) + 1 + mxv_currentState-> subdiagonals;
	}

	rewrite = 0;
	append = 1;
	accData = 0;
	backupFile = 0;
	refillData = 0;

	if (filas > 3)
	{
		for (uint8_t ii = 0; ii < CSMSM; ii++)
		{
			init = (1 + ((ii + 1) * (mxv_currentState-> subdiagonals + 1))) * mxv_currentState->complete_rows;
			vec_offset = 1 + ((ii + 1) * (mxv_currentState->subdiagonals + 1));
			offset = (2 + ii) * (mxv_currentState-> subdiagonals + 1);

		    for (uint8_t jj = 0; jj < 2; jj++)
		    {
				ffloor = filas/2;
				fceil = (filas / 2);
				if (filas % 2 != 0)
				{
					fceil++;
				}

				if (jj == 0)
				{
					fcut = ffloor;
				}
				else
				{
					fcut = fceil;
					init = init + (ffloor * mxv_currentState->complete_rows);
					vec_offset = vec_offset + ffloor;
					offset = offset + ffloor;
				}

				ccut = (mxv_currentState-> subdiagonals * 2) + fcut;
				ID00001007_mxv_setRows(address, port, mxv_currentState, fcut);
				ID00001007_mxv_setColumns(address, port, mxv_currentState, ccut);
				ID00001007_mxv_setConfig(address, port, mxv_currentState);

				ID00001007_mxv_writeVectorsMems(address, port, mxv_currentState, sdramC_currentState, vec_offset);
				ID00001007_mxv_writeMatrixMems(address, port, mxv_currentState, sdramC_currentState, init, offset);
				ID00001007_startIP(address, port);
				/// ADD_CODE : En esta seccion se espera el done lo que requiere de la interrupcion
				usleep(100);
            	ID00001007_mxv_readData(address, port, mxv_currentState, sdramC_currentState, refillData, rewrite, append, accData, backupFile, offset);
				// ADD_CODE : En esta seccion se espera limpiar la interrupcion

			}
		}
	}
	else
	{
		for ( uint8_t ii = 0; ii < CSMSM; ii++)
		{
			init = (1 + ((ii + 1) * (mxv_currentState-> subdiagonals + 1))) * mxv_currentState->complete_rows;
			vec_offset = 1 + ((ii + 1) * (mxv_currentState-> subdiagonals + 1));
			offset = (2 + ii) * (mxv_currentState-> subdiagonals + 1);

			ID00001007_mxv_setRows(address, port, mxv_currentState, filas);
			ID00001007_mxv_setColumns(address, port, mxv_currentState, columnas);
			ID00001007_mxv_setConfig(address, port, mxv_currentState);

			ID00001007_mxv_writeVectorsMems(address, port, mxv_currentState, sdramC_currentState, vec_offset);
			ID00001007_mxv_writeMatrixMems(address, port, mxv_currentState, sdramC_currentState, init, offset);
			ID00001007_startIP(address, port);
			/// ADD_CODE : En esta seccion se espera el done lo que requiere de la interrupcion
			usleep(100);
        	ID00001007_mxv_readData(address, port, mxv_currentState, sdramC_currentState, refillData, rewrite, append, accData, backupFile, offset);
			// ADD_CODE : En esta seccion se espera limpiar la interrupcion
		}
	}

	//Hacer submatriz secundaria final
	if (CSMSF == 1)
	{
		filas = mxv_currentState-> subdiagonals + 1;
		columnas = (mxv_currentState-> subdiagonals * 2) + 1 + mxv_currentState-> subdiagonals;
	}

	init = (1 + ((CSMSM + 1) * (mxv_currentState-> subdiagonals + 1))) * mxv_currentState-> complete_rows;
	vec_offset = 1 + ((CSMSM + 1) * (mxv_currentState-> subdiagonals + 1));
	offset = (2 + CSMSM) * (mxv_currentState-> subdiagonals + 1);
	rewrite = 0;
	append = 1;
	accData = 0;
	backupFile = 0;
	refillData = 0;

	if (filas > 3)
	{
		for (uint8_t jj = 0; jj < 2; jj++)
		{
			ffloor = filas/2;
			fceil = (filas / 2);
			if (filas % 2 != 0)
			{
				fceil++;
			}

			if (jj == 0)
			{
				fcut = ffloor;
			}
			else
			{
				fcut = fceil;
				init = init + (ffloor * mxv_currentState->complete_rows);
				vec_offset = vec_offset + ffloor;
				offset = offset + ffloor;
			}

			ccut = (mxv_currentState-> subdiagonals * 2) + fcut;
			ID00001007_mxv_setRows(address, port, mxv_currentState, fcut);
			ID00001007_mxv_setColumns(address, port, mxv_currentState, ccut);
			ID00001007_mxv_setConfig(address, port, mxv_currentState);

			ID00001007_mxv_writeVectorsMems(address, port, mxv_currentState, sdramC_currentState, vec_offset);
			ID00001007_mxv_writeMatrixMems(address, port, mxv_currentState, sdramC_currentState, init, offset);
			ID00001007_startIP(address, port);
			/// ADD_CODE : En esta seccion se espera el done lo que requiere de la interrupcion
			usleep(100);
        	ID00001007_mxv_readData(address, port, mxv_currentState, sdramC_currentState, refillData, rewrite, append, accData, backupFile, offset);
			// ADD_CODE : En esta seccion se espera limpiar la interrupcion
		}
	}
	else
	{
		ID00001007_mxv_setRows(address, port, mxv_currentState, filas);
		ID00001007_mxv_setColumns(address, port, mxv_currentState, columnas);
		ID00001007_mxv_setConfig(address, port, mxv_currentState);

		ID00001007_mxv_writeVectorsMems(address, port, mxv_currentState, sdramC_currentState, vec_offset);
		ID00001007_mxv_writeMatrixMems(address, port, mxv_currentState, sdramC_currentState, init, offset);
		ID00001007_startIP(address, port);
		/// ADD_CODE : En esta seccion se espera el done lo que requiere de la interrupcion
		usleep(100);
    	ID00001007_mxv_readData(address, port, mxv_currentState, sdramC_currentState, refillData, rewrite, append, accData, backupFile, offset);
		// ADD_CODE : En esta seccion se espera limpiar la interrupcion
	}

	// Hacer submatriz final
	if (CSMF == 1)
	{
		filas = mxv_currentState-> subdiagonals + 1 + mxv_currentState-> complete_rows % (mxv_currentState-> subdiagonals + 1);
		columnas = (mxv_currentState-> subdiagonals * 2) + 1 + mxv_currentState-> complete_columns % (mxv_currentState-> subdiagonals + 1);
	}

	init = (1 + (CSMS * (mxv_currentState-> subdiagonals + 1))) * mxv_currentState-> complete_rows;
	vec_offset = 1 + (CSMS * (mxv_currentState-> subdiagonals + 1));
	offset = (1 + CSMS) * (mxv_currentState-> subdiagonals + 1);
	rewrite = 0;
	append = 1;
	accData = 0;
	backupFile = 0;
	refillData = 0;

	ID00001007_mxv_setRows(address, port, mxv_currentState, filas);
	ID00001007_mxv_setColumns(address, port, mxv_currentState, columnas);
	ID00001007_mxv_setConfig(address, port, mxv_currentState);

	ID00001007_mxv_writeVectorsMems(address, port, mxv_currentState, sdramC_currentState, vec_offset);
	ID00001007_mxv_writeMatrixMems(address, port, mxv_currentState, sdramC_currentState, init, offset);
	ID00001007_startIP(address, port);
	/// ADD_CODE : En esta seccion se espera el done lo que requiere de la interrupcion
	usleep(100);
	ID00001007_mxv_readData(address, port, mxv_currentState, sdramC_currentState, refillData, rewrite, append, accData, backupFile, offset);
	// ADD_CODE : En esta seccion se espera limpiar la interrupcion

	// Hacer submatriz esquina inicial
	if (mxv_currentState-> subdiagonals > 0)
	{
		if (mxv_currentState-> corner_subdiagonals_on == 1)
		{
			filas = mxv_currentState-> subdiagonals;
			columnas = mxv_currentState-> subdiagonals;

			ID00001007_mxv_setRows(address, port, mxv_currentState, filas);
			ID00001007_mxv_setColumns(address, port, mxv_currentState, columnas);
			ID00001007_mxv_setConfig(address, port, mxv_currentState);

			init = mxv_currentState-> complete_rows * (mxv_currentState-> complete_columns - mxv_currentState-> subdiagonals);
			vec_offset = mxv_currentState-> complete_columns - mxv_currentState-> subdiagonals;
			offset = 0;
			rewrite = 1;
			append = 0;
			accData = 1;
			backupFile = 1;
			refillData = 1;

			ID00001007_mxv_writeVectorsMems(address, port, mxv_currentState, sdramC_currentState, vec_offset);
			ID00001007_mxv_writeMatrixMems(address, port, mxv_currentState, sdramC_currentState, init, offset);
			ID00001007_startIP(address, port);
			/// ADD_CODE : En esta seccion se espera el done lo que requiere de la interrupcion
			usleep(100);
        	ID00001007_mxv_readData(address, port, mxv_currentState, sdramC_currentState, refillData, rewrite, append, accData, backupFile, offset);
			// ADD_CODE : En esta seccion se espera limpiar la interrupcion
		}

		//Hacer submatriz esquina final
		if (mxv_currentState-> corner_subdiagonals_on == 1)
		{
			filas = mxv_currentState-> subdiagonals;
			columnas = mxv_currentState-> subdiagonals;

			ID00001007_mxv_setRows(address, port, mxv_currentState, filas);
			ID00001007_mxv_setColumns(address, port, mxv_currentState, columnas);
			ID00001007_mxv_setConfig(address, port, mxv_currentState);

			init = 0;
			vec_offset = 0;
			offset = mxv_currentState-> complete_rows - mxv_currentState-> subdiagonals;
			rewrite = 1;
			append = 0;
			accData = 1;
			backupFile = 1;
			refillData = 2;

			ID00001007_mxv_writeVectorsMems(address, port, mxv_currentState, sdramC_currentState, vec_offset);
			ID00001007_mxv_writeMatrixMems(address, port, mxv_currentState, sdramC_currentState, init, offset);
			ID00001007_startIP(address, port);
			/// ADD_CODE : En esta seccion se espera el done lo que requiere de la interrupcion

	        ID00001007_mxv_readData(address, port, mxv_currentState, sdramC_currentState, refillData, rewrite, append, accData, backupFile, offset);
	        // ADD_CODE : En esta seccion se espera limpiar la interrupcion
		}
	}

#endif




	return ret;
}

int32_t ID00001007_mxv_writeVectorsMems(uint8_t address, uint8_t port, mxv_state *mxv_currentState, sdramC_state *sdramC_currentState, uint32_t vec_offset){
	uint8_t numMemsIn = 0;
	uint32_t dataOutSDRAM[256];
	uint16_t limit;
#ifdef MEM_SEPARATED
		numMemsIn = 2;
		limit = 1;

		// TODO: Hay que corregir bug si el vector supera tamaño de 256
		if(mxv_currentState-> columns > 128){
			limit = 2;
		}else{
			limit = 1;
		}
#endif
#ifdef MEM_INTERCALATED
		numMemsIn = 1;
		// TODO: Hay que corregir bug si el vector supera tamaño de 256
		if(mxv_currentState-> columns > 64){
			limit = 2;
		}else{
			limit = 1;
		}
#endif

#ifdef MEM_SHARED
		numMemsIn = 1;
		// TODO: Hay que corregir bug si el vector supera tamaño de 256
		if(mxv_currentState-> columns > 256){ // WARNING: se modifico de 128 a 256
			limit = 2;
		}else{
			limit = 1;
		}
#endif


	for (uint8_t i = 0; i < numMemsIn; i++)
	{

//TODO: REVISAR MEM_SEPARATED VEC_OFFSET
		for(uint8_t j = 0; j < limit; j++){

			ID00001008_sdramC_readFrame(address, 0, sdramC_currentState , vec_offset + (j+i)*mxv_currentState-> columns, dataOutSDRAM, mxv_currentState-> columns);// TO FIX
			if (mxv_currentState->columns < mxv_currentState->numPEs)
			{
				printf("------------------VECTOR IN _shift-----------\n");

				for (uint8_t j = 0; j < mxv_currentState->columns; j++)
				{
					dataOutSDRAM[mxv_currentState->numPEs-1-j] = dataOutSDRAM[mxv_currentState->numPEs - mxv_currentState->columns-j];
				}

				for (uint8_t j = 0; j < mxv_currentState->numPEs - mxv_currentState->columns; j++)
				{
					dataOutSDRAM[j] = 0;
				}
                ID00001007_writeData(address, port, 0 + i  , dataOutSDRAM, mxv_currentState->numPEs, j*mxv_currentState-> columns);
			}
			else
			{






			    //id00003000_transferDataIP2IP(ADDR1, PORT0, "MMEMOUT", 0, ADDR1, PORT2, "MDATAIN",0,8);
			    id00003000_transferDataIP2IP(ADDR1, PORT0, MMEMOUT, 0, ADDR1, PORT1, 0, j*mxv_currentState-> columns ,mxv_currentState-> columns);
				//ID00001007_writeData(address, port, 0 + i , dataOutSDRAM, mxv_currentState-> columns, j*mxv_currentState-> columns);








			}
#define PRINTF_PARTIAL_VECTOR1
#ifdef PRINTF_PARTIAL_VECTOR
			printf("------------------VECTOR IN-----------\n");
			for (uint32_t j = 0; j < mxv_currentState-> columns; j++){
			printf("%08x[%03d] ", dataOutSDRAM[j],j);
			}
			printf("\n\n");
#endif
		}
	}


	return 0;

} 




int32_t ID00001007_mxv_writeDataMem(uint8_t address, uint8_t port, mxv_state *mxv_currentState,sdramC_state *sdramC_currentState, uint32_t punter, uint32_t add_mem_ip,uint32_t *dataOutSDRAM, uint32_t frame_size){
		
			if (mxv_currentState->rows < mxv_currentState->numPEs)
			{
				for (uint8_t j = 0; j < mxv_currentState->rows; j++)
				{
					dataOutSDRAM[mxv_currentState->numPEs-1-j] = dataOutSDRAM[mxv_currentState->numPEs - mxv_currentState->rows-j];			
				}

				for (uint8_t j = 0; j < mxv_currentState->numPEs - mxv_currentState->rows; j++)
				{
					dataOutSDRAM[j] = 0;			
				}
                ID00001007_writeData(address, port, punter, dataOutSDRAM, mxv_currentState->numPEs, add_mem_ip);
			}
			else
			{
            ID00001007_writeData(address, port, punter, dataOutSDRAM, frame_size, add_mem_ip);
			}
			
	return 0;
}



int32_t ID00001007_mxv_writeMatrixMems(uint8_t address, uint8_t port, mxv_state *mxv_currentState, sdramC_state *sdramC_currentState, uint32_t initial, uint32_t real_offset){

	uint8_t numMemsIn = 0; 
	uint32_t init = 0;
	uint32_t dataOutSDRAM[256];
	uint16_t stop;
	uint32_t back_adj;
	uint8_t  residue_on;
	uint32_t offset;
	uint32_t step;
	uint32_t punter;
	uint32_t idx;
	uint32_t frame_size;   /// se cambio a 32_ t observando un cambio en codigo
	uint32_t limit;
    uint32_t add_mem_ip;
    uint32_t addr_matrix;

	if(mxv_currentState-> residue == 0){
		limit = mxv_currentState->quotient;
	}else{
	    limit = mxv_currentState->quotient + 1;
	}
	frame_size = mxv_currentState->rows; // se agrego a driver

	#ifdef MEM_INTERCALATED
		uint32_t addr_matrix = 2*mxv_currentState->complete_columns; 
		stop = 2; // 
		back_adj = 2 * mxv_currentState->complete_rows;
		init = initial * 2;
	#endif
	#ifdef MEM_SEPARATED 
		uint32_t addr_matrix = mxv_currentState->complete_columns; 
		uint32_t addr_matrix2= mxv_currentState-> complete_columns * (2 + mxv_currentState-> complete_rows);
		stop = 1;
		back_adj = mxv_currentState->complete_rows;
		init = initial; 
	#endif 
	#ifdef MEM_SHARED 
		addr_matrix = mxv_currentState->complete_columns;
		stop = 1;		
		back_adj = mxv_currentState->complete_rows;
		init = initial; 
	#endif 
	
	for(uint32_t p = 0; p < mxv_currentState-> numPEs; p++){

#ifdef PRINTF_DATA_TO_WRITE_TO_THE_MATRIX
		printf("#################################################################################### WRITE MEMORY OF MATRIX:%x#############################################################################\n",p);
#endif
		if ((mxv_currentState-> numPEs - p) <= mxv_currentState->residue){
			residue_on = 1;
		}else{
			residue_on = 0;
		}
#ifdef MEM_INTERCALATED
		offset = 2 * real_offset + 2 * (mxv_currentState-> complete_rows) * p;
		step = 2 * mxv_currentState-> numPEs * mxv_currentState-> complete_rows ; 
#else 
		offset = real_offset + (mxv_currentState-> complete_rows) * p;
		step = mxv_currentState-> numPEs * mxv_currentState-> complete_rows;
#endif 

#ifdef MEM_SEPARATED
		punter = 4*p + 4;
#else 
		punter = p+1 ;
#endif

		for (uint32_t i = 0; i < limit; i++){
#ifdef PRINTF_DATA_TO_WRITE_TO_THE_MATRIX
			printf("------------------WRITE MEM MATRIX-----------\n");
#endif
			for(uint32_t o = 0; o < stop; o++){// se modifico stop = stop/128

#ifdef MEM_SHARED 
			if (mxv_currentState->rows < mxv_currentState->numPEs)
			{
				add_mem_ip = i * mxv_currentState->numPEs;
			}
			else
			{
				add_mem_ip = i * mxv_currentState->rows;
			}
			
				idx = init  + offset + i * step ; // original--idx = init + o + offset + i * step;

#else
			if (mxv_currentState->rows < mxv_currentState->numPEs)
			{
				add_mem_ip = 2 * i * mxv_currentState->numPEs + o * mxv_currentState->numPEs ;
			}
			else
			{
				add_mem_ip = 2 * i * mxv_currentState->rows + o * mxv_currentState->rows ;
			}
				add_mem_ip = 2 * i * mxv_currentState->rows + o * mxv_currentState->rows ;
				idx = init + o * mxv_currentState->rows + offset + i * step ; // original--idx = init + o + offset + i * step;

#endif
				if(i == mxv_currentState-> quotient){
					idx = idx - back_adj * (mxv_currentState-> numPEs - mxv_currentState-> residue);
				}

				if(mxv_currentState->numPEs-p > mxv_currentState-> columns){

					for (uint32_t j = 0; j <frame_size; j++){
						dataOutSDRAM[j]=0;
					}
					ID00001007_mxv_writeDataMem(address, port, mxv_currentState, sdramC_currentState, punter, add_mem_ip, dataOutSDRAM, frame_size);

#ifdef PRINTF_DATA_TO_WRITE_TO_THE_MATRIX
					for (uint32_t j = 0; j <frame_size; j++){
					printf("0[%d] ", add_mem_ip+j);
					}
#endif

#ifdef MEM_SEPARATED
					ID00001007_mxv_writeDataMem(address, port, mxv_currentState, sdramC_currentState, punter + 2, add_mem_ip, 0, frame_size);
#endif
				}else if(i < mxv_currentState-> quotient){
					ID00001008_sdramC_readFrame(address, 0, sdramC_currentState, idx + addr_matrix, dataOutSDRAM, frame_size);
					//ID00001007_mxv_writeDataMem(address, port, mxv_currentState, sdramC_currentState, punter, add_mem_ip, dataOutSDRAM, frame_size);

				    id00003000_transferDataIP2IP(ADDR1, PORT0, MMEMOUT, 0, ADDR1, PORT1, punter*2, add_mem_ip ,frame_size);







#ifdef PRINTF_DATA_TO_WRITE_TO_THE_MATRIX
					//printf("----------SDRAM READ DATA MEMIN---------------------");
					for (uint32_t j = 0; j <frame_size; j++){
					printf("%x[%d] ", dataOutSDRAM[j], add_mem_ip+j);
					}
#endif

#ifdef MEM_SEPARATED
				    ID00001008_sdramC_readFrame(address, port, sdramC_currentState, idx + addr_matrix2, dataOutSDRAM);
					ID00001007_mxv_writeDataMem(address, port, mxv_currentState, sdramC_currentState, punter + 2, add_mem_ip, dataOutSDRAM, frame_size);					
#endif

				}else if(residue_on == 0){

					for (uint32_t j = 0; j <frame_size; j++){
						dataOutSDRAM[j]=0;
					}
					ID00001007_mxv_writeDataMem(address, port, mxv_currentState, sdramC_currentState, punter, add_mem_ip, dataOutSDRAM, frame_size);

#ifdef PRINTF_DATA_TO_WRITE_TO_THE_MATRIX	
					for (uint32_t j = 0; j <frame_size; j++){
					printf("0[%d] ", add_mem_ip+j);
					}
#endif

#ifdef MEM_SEPARATED
					ID00001007_mxv_writeDataMem(address, port, mxv_currentState, sdramC_currentState, punter + 2, add_mem_ip, 0, frame_size);
					//aip_writem_intf(mxv_currentState->BASE_A, punter+2, add_mem_ip, 0, frame_size);

#endif
				
				}else {
				    ID00001008_sdramC_readFrame(address, 0, sdramC_currentState, idx + addr_matrix, dataOutSDRAM, frame_size); //Ojo idx +vector size
					//ID00001007_mxv_writeDataMem(address, port, mxv_currentState, sdramC_currentState, punter, add_mem_ip, dataOutSDRAM, frame_size);
				    id00003000_transferDataIP2IP(ADDR1, PORT0, MMEMOUT, 0, ADDR1, PORT1, punter*2, add_mem_ip ,frame_size);

#ifdef PRINTF_DATA_TO_WRITE_TO_THE_MATRIX					
					for (uint32_t j = 0; j <frame_size; j++){
					printf("%x[%d] ", dataOutSDRAM[j], add_mem_ip+j);
					}
#endif

#ifdef MEM_SEPARATED
				    ID00001008_sdramC_readFrame(address, port, sdramC_currentState, idx + addr_matrix2, dataOutSDRAM);
					ID00001007_mxv_writeDataMem(address, port, mxv_currentState, sdramC_currentState, punter + 2, add_mem_ip, dataOutSDRAM, frame_size);					
#endif
				}
			}
			
#ifdef PRINTF_DATA_TO_WRITE_TO_THE_MATRIX					
			printf("\n");
#endif
	
		}
	}
	return 0;
} 

int32_t ID00001007_mxv_readData(uint8_t address, uint8_t port, mxv_state *mxv_currentState, sdramC_state *sdramC_currentState, uint8_t refill, uint8_t rewrite, uint32_t append, uint32_t accData, uint32_t backupFile, uint32_t offset){
	dev_dbg("IPMXV","rewrite:%d, append:%d, accData:%d, backupFile:%d, offset:%d \n" , rewrite, append, accData, backupFile, offset );
	dev_dbg("IPMXV","Filas: %i, Columnas: %i\n", mxv_currentState->rows, mxv_currentState->columns);

	uint32_t off;
	uint32_t vector1_addrSDRAM = 2 * mxv_currentState->complete_columns * (mxv_currentState->complete_rows + 1) + offset;
    uint32_t vector2_addrSDRAM = 2 * mxv_currentState->complete_columns * (mxv_currentState->complete_rows + 1) + mxv_currentState->complete_columns + offset;
	uint16_t acc_r;
	uint16_t acc_i;	
	if (mxv_currentState->rows < mxv_currentState->numPEs)
	{
		off = mxv_currentState->numPEs - mxv_currentState->rows;
	}
	else
	{
		off = 0;
	}

#ifdef MEM_SHARED
    ID00001007_readData(address, port, 0, mxv_currentState-> mem_out , mxv_currentState->rows, off);
	//aip_readm_intf(mxv_currentState->BASE_A, MMEMOUT0, off, mxv_currentState->rows,mxv_currentState-> mem_out );	

    /*
			DEBUG_CODE_L1({
				dev_dbg("IPMXV","-----------------------------------------------------------RESULTANT VECTOR-----------------------------------------------------------------\n");

				for (uint32_t j = off; j <256; j++){
					dev_dbg("IPMXV","%X[%d] ", mxv_currentState-> mem_out[j],j);
				}
			});
	*/
	if (accData == 1)
	{
		ID00001008_sdramC_readFrame(address, 0, sdramC_currentState, vector1_addrSDRAM, mxv_currentState-> mem_out_temp, mxv_currentState-> rows );

			if (refill<2)
			{
			for (uint16_t i = 0; i < mxv_currentState->rows; i++)
			{
				acc_r = (mxv_currentState-> mem_out[i] >> 16) + (mxv_currentState-> mem_out_temp[i] >> 16);
				acc_i = (mxv_currentState-> mem_out[i] & 0xffff) + (mxv_currentState-> mem_out_temp[i] & 0xffff);
				mxv_currentState-> mem_out[i] = (acc_r << 16 ) + acc_i;
			}
			ID00001008_sdramC_writeFrame(address, 0, sdramC_currentState, vector1_addrSDRAM, mxv_currentState-> mem_out, mxv_currentState-> rows);
			}
	}
	else
	{

			ID00001008_sdramC_writeFrame(address, 0, sdramC_currentState, vector1_addrSDRAM, mxv_currentState-> mem_out, mxv_currentState-> rows);

	}

	if (refill > 0)
	{
		if (refill == 1)
		{
			for (uint16_t k = mxv_currentState->rows; k < mxv_currentState->complete_rows ; k++)
			{
				acc_r = (mxv_currentState-> mem_out[k] >> 16) + (mxv_currentState-> mem_out_temp[k] >> 16);
				acc_i = (mxv_currentState-> mem_out[k] & 0xffff) + (mxv_currentState-> mem_out_temp[k] & 0xffff);
				mxv_currentState-> mem_out[k] = (acc_r << 16 ) + acc_i;
			}
			ID00001008_sdramC_writeFrame(address, 0, sdramC_currentState, vector1_addrSDRAM, mxv_currentState-> mem_out, mxv_currentState-> rows);
		}
		else if (refill == 2)
		{
			for (uint16_t k = 0; k < mxv_currentState->complete_rows - mxv_currentState->rows ; k++)
			{
				acc_r = (mxv_currentState-> mem_out[k] >> 16) + (mxv_currentState-> mem_out_temp[k] >> 16);
				acc_i = (mxv_currentState-> mem_out[k] & 0xffff) + (mxv_currentState-> mem_out_temp[k] & 0xffff);
				
				mxv_currentState-> mem_out[k] = (acc_r << 16 ) + acc_i;
			}
			ID00001008_sdramC_writeFrame(address, 0, sdramC_currentState, vector1_addrSDRAM, mxv_currentState-> mem_out, mxv_currentState->rows );
		}
	}
	



#else

    D00001007_readData(address, port, 0, mxv_currentState-> mem_out , mxv_currentState->rows, off);
	//aip_readm_intf(mxv_currentState->BASE_A, MMEMOUT0, off, mxv_currentState->rows,mxv_currentState-> mem_out );

	



    D00001007_readData(address, port, 1, mxv_currentState-> mem_out , mxv_currentState->rows, off);

	//aip_readm_intf(mxv_currentState->BASE_A, MMEMOUT1, off, mxv_currentState->rows,mxv_currentState-> mem_out2 );
	printf("DELAY");
(sdramC_currentState, vector1_addrSDRAM, mxv_currentState-> mem_out_temp);
	printf("Direccion de SDRAM:%d,offset:%d\n",vector2_addrSDRAM,offset);
(sdramC_currentState, vector2_addrSDRAM, mxv_currentState-> mem_out_temp2);
#ifdef PRINT_PARTIAL_RESULTS
	printf("------------------VALUE OUT RESULT IN ITER MXV-----------\n");
	for (uint32_t j = 0; j <mxv_currentState->rows; j++){
	printf("%x[%d] ", mxv_currentState->mem_out2[j],offset+j);
	}
	printf("\n");
#endif
	if (accData == 1)
	{

			for (uint8_t i = 0; i < mxv_currentState->rows; i++)
			{
				mxv_currentState-> mem_out[i] = mxv_currentState-> mem_out[i] + mxv_currentState-> mem_out_temp[i];
				mxv_currentState-> mem_out2[i] = mxv_currentState-> mem_out2[i] + mxv_currentState-> mem_out_temp2[i];
			}

			sdramController_write_data(sdramC_currentState, vector1_addrSDRAM, mxv_currentState-> mem_out);

			printf("DELAY");
			if (offset==0)
			{
				printf("------------------VALUE TO MEM_OUT2-----------\n");
				for (uint32_t j = 0; j <mxv_currentState->rows; j++){
					printf("%x[%d] ", mxv_currentState->mem_out2[j],offset+j);
				}
				printf("\n");
				printf("-----------------------------------------------------------MATRIX_VECTOR_RESULT_1-----------------------------------------------------------------\n");
				for (uint32_t i = 0; i < 1; i++) {
					uint32_t addr = i*128 + 804402 + 2000;
				ID00001008_sdramC_readFrame(address, port, sdramC_currentState, addr, mxv_currentState-> mem_out2);
					for (uint32_t j = 0; j <48; j++){
						printf("%X[%d] ", mxv_currentState-> mem_out2[j],i*128+j);
					}
				}
				printf("\n\n");
			}

			sdramController_write_data(sdramC_currentState, vector2_addrSDRAM, mxv_currentState-> mem_out2);
			


				printf("-----------------------------------------------------------MATRIX_VECTOR_RESULT_IN_EACH ITER-----------------------------------------------------------------\n");
				for (uint32_t i = 0; i < 1; i++) {
					uint32_t addr = i*128 + 804402 + 2000;
				ID00001008_sdramC_readFrame(address, port, sdramC_currentState, addr, mxv_currentState-> mem_out2);
					for (uint32_t j = 0; j <48; j++){
						printf("%X[%d] ", mxv_currentState-> mem_out2[j],i*128+j);
					}
				}
				printf("\n\n");
			
	}
	else
	{
			sdramController_write_data(sdramC_currentState, vector1_addrSDRAM, mxv_currentState-> mem_out);
			sdramController_write_data(sdramC_currentState, vector2_addrSDRAM, mxv_currentState-> mem_out2);
	}


	if (refill > 0)
	{
		if (refill == 1)
		{
			for (uint16_t k = mxv_currentState->rows; k < mxv_currentState->complete_rows ; k++)
			{
				mxv_currentState-> mem_out[k] = mxv_currentState-> mem_out[k] + mxv_currentState-> mem_out_temp[k];
				mxv_currentState-> mem_out2[k] = mxv_currentState-> mem_out2[k] + mxv_currentState-> mem_out_temp2[k];
			}

			sdramController_write_data(sdramC_currentState, vector1_addrSDRAM, mxv_currentState-> mem_out);
			sdramController_write_data(sdramC_currentState, vector2_addrSDRAM, mxv_currentState-> mem_out2);
		}

		else if (refill == 2)
		{

			for (uint16_t k = 0; k < mxv_currentState->complete_rows - mxv_currentState->rows ; k++)
			{	
				mxv_currentState-> mem_out[k] = mxv_currentState-> mem_out[k] + mxv_currentState-> mem_out_temp[k];
				mxv_currentState-> mem_out2[k] = mxv_currentState-> mem_out2[k] + mxv_currentState-> mem_out_temp2[k];
			}

			sdramController_write_data(sdramC_currentState, vector1_addrSDRAM, mxv_currentState-> mem_out);
			sdramController_write_data(sdramC_currentState, vector2_addrSDRAM, mxv_currentState-> mem_out2);
		}
		
	}
				printf("-----------------------------------------------------------MATRIX_VECTOR_ENDREAD_DATA-----------------------------------------------------------------\n");
				for (uint32_t i = 0; i < 1; i++) {
					uint32_t addr = i*128 + 804402 + 2000;
				ID00001008_sdramC_readFrame(address, port, sdramC_currentState, addr, mxv_currentState-> mem_out2);
					for (uint32_t j = 0; j <48; j++){
						printf("%X[%d] ", mxv_currentState-> mem_out2[j],i*128+j);
					}
				}
				printf("\n\n");


#endif			
	return 0;
}
