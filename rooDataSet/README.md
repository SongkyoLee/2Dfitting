# HIN-14-009
- Usage example
 - make : to compile [Makefile]
  	- change the makefile depending on KUNPL(root6.04.02) or LXPLUS(CMSSW_8_0_0)
 - make Tree2DataSetsXXX : to compile [tree2Datasets_xxx.cpp]
    - e.g.) Tree2DataSetsPP from tree2Datasets_pp.cpp
    - e.g.) Tree2DataSetsPPMC from tree2Datasets_pp_mc.cpp
  - You can check the name of executable in Makefile
 - ./runTree_XXX.sh : to run executable and get output roodatatest in outRoo_xxx
    - runTree_pp_newcut_off.sh creates outRoo_Data_pp_newcut, outRoo_PRMC_pp_newcut_off, outRoo_NPMC_pp_newcut_off

- Notices
  - [xxx_flip.cpp] is to flip the rapidity sign of 2nd run to match 1st run y_lab (only for pa 2nd run)
  - [xxx_mc_zvtx.cpp] is only for crosschecks of z vertex weight (not used for the actual fitting)
  - [draw_xxx.cpp] is to draw variables in RooDataSets
    - [draw_zVtx_1file.cpp] requires RooDataSets with Zvtx from [xxx_zvtx.cpp]
    - [draw_singlemu.cpp] requires RooDataSets with SglMu [cpp removed now]

- Total output files needed
 
 - outRoo_Data_Pbp_xxx_v1
 - outRoo_Data_Pbp_xxx_v2
 - outRoo_Data_pPbFlip_xxx
 - outRoo_PRMC_Pbp_xxx
 - outRoo_NPMC_Pbp_xxx
 - outRoo_PRMC_pPbFlip_xxx
 - outRoo_NPMC_pPbFlip_xxx 
 - outRoo_Data_pp_xxx 
 - outRoo_PRMC_pp_xxx
 - outRoo_NPMC_pp_xxx

