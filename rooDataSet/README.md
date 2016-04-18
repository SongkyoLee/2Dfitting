# HIN-14-009
- Usage example
 - make : to compile [MakeFile
 - make Tree2DataSets : to compile [tree2Datasets_xxx.cpp]
 - runTree_pa_newcut_off2M.sh : to run executable and produce outRoo_xxx

- [xxx_flip.cpp] is to flip the rapidity sign of 2nd run to match 1st run y_lab (only for pa)
- [xxx_mc_zvtx.cpp] is only for crosschecks of z vertex weight (not used for the actual fitting)
- [draw_xxx.cpp] is to draw variables in RooDataSets
 - [draw_zVtx_1file.cpp] requires RooDataSets with Zvtx from [xxx_zvtx.cpp]
 - [draw_singlemu.cpp] requires RooDataSets with SglMu [cpp removed now]

- output files
 - for pA Data 
   - outRoo_Data_Pbp_xxx_v1
   - outRoo_Data_Pbp_xxx_v2
   - outRoo_Data_pPb_xxx 
   - outRoo_Data_pPbFlip_xxx
 - for pA MC   
   - outRoo_PRMC_Pbp_xxx
   - outRoo_NPMC_Pbp_xxx
   - outRoo_PRMC_pPb_xxx
   - outRoo_NPMC_pPb_xxx 
   - outRoo_PRMC_pPbFlip_xxx
   - outRoo_NPMC_pPbFlip_xxx 
 - for pp
   - outRoo_Data_pp_xxx 
   - outRoo_PRMC_pp_xxx
   - outRoo_NPMC_pp_xxx
