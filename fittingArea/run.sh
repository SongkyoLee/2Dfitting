#!/bin/bash
#################################################
############ for pp 
#################################################
### 8rap9pt newcut
./runBatch_pp_8rap9pt_newcut.sh Fit2DData /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_pp_newcut/outRoo_Data_pp_newcut.root pp_8rap9pt_newcut >& log_pp_8rap9pt_newcut &  

#################################################
############ for pA (Pbp + pPbFlip)
#################################################
### 8rap9pt newcut
#./runBatch_pA_8rap9pt_newcut.sh Fit2DData /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_Pbp_newcut_v1/outRoo_Data_Pbp_newcut_v1.root /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_Pbp_newcut_v2/outRoo_Data_Pbp_newcut_v2.root /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_pPbFlip_newcut/outRoo_Data_pPbFlip_newcut.root pA_8rap9pt_newcut >& log_pA_8rap9pt_newcut & 
### 6rap2pt newcut
#./runBatch_pA_6rap2pt_newcut.sh Fit2DData /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_Pbp_newcut_v1/outRoo_Data_Pbp_newcut_v1.root /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_Pbp_newcut_v2/outRoo_Data_Pbp_newcut_v2.root /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_pPbFlip_newcut/outRoo_Data_pPbFlip_newcut.root pA_6rap2pt_newcut >& log_pA_6rap2pt_newcut & 

