#!/bin/bash

##### Pbp
./runBatch_Pbp_8rap9pt_newcut.sh Fit2DData /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_Pbp_newcut_v1/outRoo_Data_Pbp_newcut_v1.root /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_Pbp_newcut_v2/outRoo_Data_Pbp_newcut_v2.root Pbp_8rap9pt_newcut >& log_Pbp_8rap9pt_newcut & 

##### pPb
./runBatch_pPb_8rap9pt_newcut.sh Fit2DData /afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_Data_pPb_newcut/outRoo_Data_pPb_newcut.root pPb_8rap9pt_newcut >& log_pPb_8rap9pt_newcut & 



