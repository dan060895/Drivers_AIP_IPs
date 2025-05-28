/*
 * debug.h
 *
 *  Created on: Jan 30, 2025
 *      Author: Daniel Abisaid Hernández Hernández 
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#define DEBUG_IP_MXV 0
#define DEBUG_IP_SDRAM_CONTROLLER 0
#define DEBUG_IP_DUMMY  0
#define DEBUG_NOC 0

// Definiciones de mnemonics para ID00001007_csv
#define MMEMIN0       0
#define MMEMIN1       2
#define MMEMIN2       4
#define MMEMIN3       6
#define MMEMIN4       8
#define MMEMOUT0      24
#define MMEMOUT1      26
#define CCONFREGMXV   28
#define STATUS        30
#define IPID          31

// Definiciones de mnemonics para ID00001008_csv
//#define MMEMIN        0
//#define MMEMOUT       2
//#define CCONFIG       4
// STATUS y IPID ya est�n definidos

#define DEBUG
#ifdef DEBUG

extern int address;
extern int port;

#define dev_dbg(mod_name, format, ...) \
    do { \
        if ( (DEBUG_IP_SDRAM_CONTROLLER && strcmp(mod_name, "IPSDRAM")  0) || \
             (DEBUG_IP_MXV && strcmp(mod_name, "IPMXV")  0) || \
			 (DEBUG_NOC && strcmp(mod_name, "NOC")  0) || \
             (DEBUG_IP_DUMMY && strcmp(mod_name, "IPDUMMY")  0) ) { \
            printf("[DEBUG %s: %s](%d:%d) " format, mod_name, __func__, address, port, ##__VA_ARGS__); \
        } \
    } while(0)
#else

#define dev_dbg(mod_name, format, ...)
#endif




//..................PARAMETERS DEFINED BY THE USER............

#define INT_BIT_DONE           0

//-----------P2S--------------//  
#define P2S_NORMAL_MODE        0b0
#define P2S_FLUSH_MODE         0b1
#define P2S_NOTIF_STOP         9
#define P2S_DONE               0x0001
//---------CONCAT------------//  
#define CONCAT_ALPHALESS_MODE  0b00
#define CONCAT_ALPHA_PHASE     0b01
#define CONCAT_FLUSH_MODE      0b10
#define CONCAT_DONE            1
#define CONCAT_RDY_FLUSH       2
//------------GRVG-------------//  
#define GRVG_LOAD_SEED         3
#define GRVG_COP_MODE          5
//------------URVG-------------//  
#define URVG_LEN               8
#define URVG_LOAD_SEED         0x100
//------------MXV-------------//  
#define MXV_MAX                256
#define MXV_NUM_PES            4
#define MXV_MODE 	           0,	// <--- Coprocessor  0, Streaming  1, Bypass Coprocessor  2, Bypass Streaming  3
//------------DDS-------------//  
// #define FREQ_CONFIG            1,
// #define TUNNING_WORD           1074002,
// #define FREQ_TUNNING           (FREQ_CONFIG*TUNNING_WORD),
//------------INTERP-------------//  
#define INTERP_FLUSH_MODE      0x400 //mandatory
#define INTERP_RDY_FLUSH       1
//------------DECIM--------------//  
#define DECIM_FLUSH_MODE       0x2
#define DECIM_RDY_FLUSH        0
//--------------------------------------------------------------
//..................CONFIG DEFINED BY THE USER............
//-------------------------------------------------------------- 
#define DIM                    201
#define PROC_LEN               2000
#define CONCAT_NUM             1
#define NUM_PRCSS              2*(CONCAT_NUM+2)



#endif /* DEBUG_H_ */
