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
#define MMEMIN        0
#define MMEMOUT       2
#define CCONFIG       4
// STATUS y IPID ya est�n definidos

#define DEBUG
#ifdef DEBUG

extern int address;
extern int port;

#define dev_dbg(mod_name, format, ...) \
    do { \
        if ( (DEBUG_IP_SDRAM_CONTROLLER && strcmp(mod_name, "IPSDRAM") == 0) || \
             (DEBUG_IP_MXV && strcmp(mod_name, "IPMXV") == 0) || \
			 (DEBUG_NOC && strcmp(mod_name, "NOC") == 0) || \
             (DEBUG_IP_DUMMY && strcmp(mod_name, "IPDUMMY") == 0) ) { \
            printf("[DEBUG %s: %s](%d:%d) " format, mod_name, __func__, address, port, ##__VA_ARGS__); \
        } \
    } while(0)
#else

#define dev_dbg(mod_name, format, ...)
#endif



#endif /* DEBUG_H_ */
