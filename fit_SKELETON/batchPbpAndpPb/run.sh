#!/bin/bash
#################################################
############ for Pbp (1st run)
#################################################
./runBatch_Pbp.sh Fit2DData /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_Pbp_newcut_v1/outRoo_Data_Pbp_newcut_v1.root /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_Pbp_newcut_v2/outRoo_Data_Pbp_newcut_v2.root Pbp_8rap9pt_newcut >& log_Pbp_8rap9pt_newcut & 

#################################################
############ for pPb (2nd run) 
#################################################
./runBatch_pPb.sh Fit2DData /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_pPb_newcut/outRoo_Data_pPb_newcut.root pPb_8rap9pt_newcut >& log_pPb_8rap9pt_newcut &  

#################################################
############ for pp 
#################################################
./runBatch_pp.sh Fit2DData /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_pp_newcut/outRoo_Data_pp_newcut.root pp_8rap9pt_newcut >& log_pp_8rap9pt_newcut &  

#################################################
############ for pA (Pbp + pPbFlip)
#################################################
./runBatch_pAMerged.sh Fit2DData /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_Pbp_newcut_v1/outRoo_Data_Pbp_newcut_v1.root /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_Pbp_newcut_v2/outRoo_Data_Pbp_newcut_v2.root /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_pPbFlip_newcut/outRoo_Data_pPbFlip_newcut.root pAMerged_8rap9pt_newcut >& log_pAMerged_8rap9pt_newcut & 



