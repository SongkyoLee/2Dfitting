#!/bin/bash

#################################################
############ for Pbp (1st run)
#################################################
### for v1 and v2 merging
#./runBatch_Pbp.sh Fit2DDataPA /afs/cern.ch/work/k/kyolee/private/pAJpsi_rooDataSet_20160214/outRoo_Data_Pbp_newcut_v1/outRoo_Data_Pbp_newcut_v1.root /afs/cern.ch/work/k/kyolee/private/pAJpsi_rooDataSet_20160214/outRoo_Data_Pbp_newcut_v2/outRoo_Data_Pbp_newcut_v2.root Pbp_8rap3pt_newcut_official >& log_Pbp_8rap3pt_newcut_official & 
#./runBatch_Pbp.sh Fit2DDataPA /afs/cern.ch/work/k/kyolee/private/pAJpsi_rooDataSet_20160214/outRoo_Data_Pbp_newcut_v1/outRoo_Data_Pbp_newcut_v1.root /afs/cern.ch/work/k/kyolee/private/pAJpsi_rooDataSet_20160214/outRoo_Data_Pbp_newcut_v2/outRoo_Data_Pbp_newcut_v2.root Pbp_8rap3pt_newcut_private_test >& log_Pbp_8rap3pt_newcut_private_test & 
#./runBatch_Pbp.sh Fit2DDataPA /afs/cern.ch/work/k/kyolee/private/pAJpsi_rooDataSet_20160214/outRoo_Data_Pbp_oldcut_v1/outRoo_Data_Pbp_oldcut_v1.root /afs/cern.ch/work/k/kyolee/private/pAJpsi_rooDataSet_20160214/outRoo_Data_Pbp_oldcut_v2/outRoo_Data_Pbp_oldcut_v2.root Pbp_8rap3pt_oldcut_official_test >& log_Pbp_8rap3pt_oldcut_official_test & 
#./runBatch_Pbp.sh Fit2DDataPA /afs/cern.ch/work/k/kyolee/private/pAJpsi_rooDataSet_20160214/outRoo_Data_Pbp_oldcut_v1/outRoo_Data_Pbp_oldcut_v1.root /afs/cern.ch/work/k/kyolee/private/pAJpsi_rooDataSet_20160214/outRoo_Data_Pbp_oldcut_v2/outRoo_Data_Pbp_oldcut_v2.root Pbp_8rap3pt_oldcut_private >& log_Pbp_8rap3pt_oldcut_private & 

#################################################
############ for pPb (2nd run) 
#################################################
./runBatch_pPb.sh Fit2DDataPA /home/songkyo/kyo/2Dfitting/rooDataSet_v60402/outRoo_Data_pPb_newcut/outRoo_Data_pPb_newcut.root pPb_8rap3pt_newcut_off2M >& log_pPb_8rap3pt_newcut_off2M &  
#./runBatch_pPb_sys0101.sh Fit2DDataPA /home/songkyo/kyo/2Dfitting/rooDataSet_v60402/outRoo_Data_pPb_newcut/outRoo_Data_pPb_newcut.root pPb_8rap3pt_newcut_off2M_sys0101 >& log_pPb_8rap3pt_newcut_off2M_sys0101 &  
#./runBatch_pPb_sys0102.sh Fit2DDataPA /home/songkyo/kyo/2Dfitting/rooDataSet_v60402/outRoo_Data_pPb_newcut/outRoo_Data_pPb_newcut.root pPb_8rap3pt_newcut_off2M_sys0102 >& log_pPb_8rap3pt_newcut_off2M_sys0102 &  
#./runBatch_pPb_sys0103.sh Fit2DDataPA /home/songkyo/kyo/2Dfitting/rooDataSet_v60402/outRoo_Data_pPb_newcut/outRoo_Data_pPb_newcut.root pPb_8rap3pt_newcut_off2M_sys0103 >& log_pPb_8rap3pt_newcut_off2M_sys0103 &  
#./runBatch_pPb_sys0104.sh Fit2DDataPA /home/songkyo/kyo/2Dfitting/rooDataSet_v60402/outRoo_Data_pPb_newcut/outRoo_Data_pPb_newcut.root pPb_8rap3pt_newcut_off2M_sys0104 >& log_pPb_8rap3pt_newcut_off2M_sys0104 &  
#./runBatch_pPb_sys0105.sh Fit2DDataPA /home/songkyo/kyo/2Dfitting/rooDataSet_v60402/outRoo_Data_pPb_newcut/outRoo_Data_pPb_newcut.root pPb_8rap3pt_newcut_off2M_sys0105 >& log_pPb_8rap3pt_newcut_off2M_sys0105 &  
#./runBatch_pPb_sys0201.sh Fit2DDataPA /home/songkyo/kyo/2Dfitting/rooDataSet_v60402/outRoo_Data_pPb_newcut/outRoo_Data_pPb_newcut.root pPb_8rap3pt_newcut_off2M_sys0201 >& log_pPb_8rap3pt_newcut_off2M_sys0201 &  
#./runBatch_pPb_sys0301.sh Fit2DDataPA /home/songkyo/kyo/2Dfitting/rooDataSet_v60402/outRoo_Data_pPb_newcut/outRoo_Data_pPb_newcut.root pPb_8rap3pt_newcut_off2M_sys0301 >& log_pPb_8rap3pt_newcut_off2M_sys0301 &  
#./runBatch_pPb_sys0302.sh Fit2DDataPA /home/songkyo/kyo/2Dfitting/rooDataSet_v60402/outRoo_Data_pPb_newcut/outRoo_Data_pPb_newcut.root pPb_8rap3pt_newcut_off2M_sys0302 >& log_pPb_8rap3pt_newcut_off2M_sys0302 &  
./runBatch_pPb_sys0401.sh Fit2DDataPA /home/songkyo/kyo/2Dfitting/rooDataSet_v60402/outRoo_Data_pPb_newcut/outRoo_Data_pPb_newcut.root pPb_8rap3pt_newcut_off2M_sys0401 >& log_pPb_8rap3pt_newcut_off2M_sys0401 &  

#./runBatch_pPb.sh Fit2DDataPA /afs/cern.ch/work/k/kyolee/private/pAJpsi_rooDataSet_20160214/outRoo_Data_pPb_newcut/outRoo_Data_pPb_newcut.root pPb_8rap3pt_newcut_official >& log_pPb_8rap3pt_newcut_official & 
#./runBatch_pPb.sh Fit2DDataPA /afs/cern.ch/work/k/kyolee/private/pAJpsi_rooDataSet_20160214/outRoo_Data_pPb_newcut/outRoo_Data_pPb_newcut.root pPb_8rap3pt_newcut_private_test >& log_pPb_8rap3pt_newcut_private_test & 
#./runBatch_pPb.sh Fit2DDataPA /afs/cern.ch/work/k/kyolee/official/pAJpsi_rooDataSet_20160214/outRoo_Data_pPb_oldcut/outRoo_Data_pPb_oldcut.root pPb_8rap3pt_oldcut_official_test >& log_pPb_8rap3pt_oldcut_official_test & 
#./runBatch_pPb.sh Fit2DDataPA /afs/cern.ch/work/k/kyolee/private/pAJpsi_rooDataSet_20160214/outRoo_Data_pPb_oldcut/outRoo_Data_pPb_oldcut.root pPb_8rap3pt_oldcut_private >& log_pPb_8rap3pt_oldcut_private & 

