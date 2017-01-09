#!/bin/bash
#################################################
############ for pp 
#################################################
### integrated
./runBatch_pp_integrated.sh Fit2DData /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_pp_newcut/outRoo_Data_pp_newcut.root pp_integrated >& log_pp_integrated &  

#################################################
############ for pA (Pbp + pPbFlip)
#################################################
### integrated
./runBatch_pA_integrated.sh Fit2DData /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_Pbp_newcut_v1/outRoo_Data_Pbp_newcut_v1.root /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_Pbp_newcut_v2/outRoo_Data_Pbp_newcut_v2.root /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_pPbFlip_newcut/outRoo_Data_pPbFlip_newcut.root pA_integrated >& log_pA_integrated & 

