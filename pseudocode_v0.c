int main(){
    //Target IP: UNIFORM_RAND_GEN
    seeds = randn(2,1);                         //generate 2 seeds
    ID00002004_writeMem(URVG_MEMIN0, seeds, 2);
    ID00002004_writeConfReg(loads_seeds_enable, 1);
    ID00002004_startIP();
    ID00002004_waitDone();

    //Target IP: GAUSSIAN_RAND_GEN
    seeds = randn(8,1);                         //generate 8 seeds
    ID00002005_writeMem(GRVG_MEMIN0, seeds, 2);
    ID00002005_writeConfReg(loads_seeds_enable, 1); 
    ID00002005_startIP();
    ID00002005_waitDone();

    //Target IP: DDS
    pkgDDS[0] = 1074002*freq;
    pkgTemp[0] = pkgDDS[0];
    pkgTemp[1] = pkgDDS[0];
    pkgTemp[2] = 0; //phase
    pkgTemp[3] = 0x00004000; //Amplitude 1
    pkgTemp[4] = 0x00004000; //Amplitude 2
    pkgTemp[5] = 0; // Period2
    pkgTemp[6] = 0; // Period1
    pkgTemp[7] = 0x00000010;  // bit [4] Enabling generation
    dds_freq = pkgTemp;
    ID00002001_writeMem(DDS_MEMIN0, dds_freq); 
    ID00002001_startIP();

    //Target IP: UNIFORM_RAND_GEN
    num_flows = 1
    ID00002004_writeConfReg(num_flows, 1); // Get alpha
    ID00002004_startIP();
    ID00002004_waitDone();
    ID00002004_readMem(URVG_MEMOUT, alpha_reg, 1); 
    alpha_val = alpha_reg[0];

    //Target IP: CONCATER_GEN
    ID00001010_writeConfReg(alpha_val, 1);
    ID00001010_startIP();

    //Target IP: MXV0
    ID00001006_writeConfReg(config_MXV0, 1);
    ID00001006_startIP();

    //Target IP: MXV_CP1
    ID00001007_writeConfReg(config_MXV1, 1);
    ID00001007startIP();

    //Target IP: INTERP_D4
    ID00001012_writeConfReg(interp_factor, 1);
    ID00001012_startIP()
    ID00001012_writeConfReg(decim_factor, 1);
    ID00001012_startIP()

    while(True){

        // Obtain package of weighted random variables.
    
        //Target IP: GAUSSIAN_RAND_GEN
        ID00002005_writeMem(GRVG_MEMIN0, varz, DIM);
        temp[0] = CP_MODE;
        temp[2] = DIM;
        ID00002005_writeConfReg(temp, 3); 
        ID00002005_startIP();
        ID00002005_waitDone();
        pkgA = ID00002005_readMem(GRVG_MEMOUT0, DIM);

        //Target IP: MXV0

        //Write vector MXV0
        ID00001006_writeMem(MXV0_MEMIN0, pkgA, DIM);
        ID00001006_startIP();
        ID00001006_waitDone();
        //Write MATRIX MXV0
        j = 0
        for(i = 1; i < TO MXV_NUM_PES; i++){
            //SDRAM
            pkgUa = sdram_ctrl();
            ID00001006_writeMem(MXV0_MEMIN1+j, pkgUa, MXV0_SIZE);
            j = j + 2;
        }
        ID00001006_startIP();
        ID00001006_waitDone();
        pkgAUa = ID00001006_readMem(MXV0_MEMOUT0, DIM);
        
        //Target IP: MXV1

        //Write vector MXV1
        ID00001007_writeMem(MXV1_MEMIN0, pkgAUa, DIM);
        ID00001007_startIP();
        //Write MATRIX MXV1
        j = 0
        for(i = 1; i < TO MXV_NUM_PES; i++){
            //SDRAM
            pkgPsiUB = sdram_ctrl();
            ID00001007_writeMem(MXV1_MEMIN1+j, pkgPsiUB, MXV1_SIZE);
            j = j + 2
        }
        ID00001007_startIP();
        ID00001007_waitDone();
        pkgProcess = ID00001007_readMem(MXV1_MEMOUT0, PROC_LEN);

        //Target IP: P2S_CONVERTER
        do {
            ID00001009_getStatus(statusTemp);
        }while (statusTemp[done_bit]);  
        ID00001009_writeMem(P2S_MEMIN0, pkgProcess);
        ID00001009_startIP();
        ID00001009_waitDone();
    }

    return 0;
}