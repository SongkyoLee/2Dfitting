#!/bin/bash
#eval `scramv1 runtime -sh`

###########################################################################################
####################################### input data ######################################## 
###########################################################################################
inputf=root://eoscms//eos/cms
##### v1 : 1st 7 run excluded, v2 : 1st 7 run (reprocessed reco)
inputfpbpV1=/home/songkyo/kyo/pPbDataSample/Data/RD2013_pa_1st_run_210676-211256_GR_P_V43D_nocut.root
inputfpbpV2=/home/songkyo/kyo/pPbDataSample/Data/RD2013_pa_1st_run_210498-210658_GR_P_V43F_nocut.root
inputfppb=/home/songkyo/kyo/pPbDataSample/Data/RD2013_pa_2nd_run_211313-211631_GR_P_V43D_nocut.root

#-----input MC
## 1) official no acc cut
inputfPbpPRMC=/home/songkyo/kyo/pPbDataSample/EfficiencySample/tot_JPsiWithFSR_pa_1st_run_STARTHI53_V27-v1_noAccCut_sglTrig_genMatch-v2.root
inputfPbpNPMC=/home/songkyo/kyo/pPbDataSample/EfficiencySample/tot_inclBtoJPsiMuMu_pa_1st_run_STARTHI53_V27-v1_noAccCut_sglTrig_genMatch-v2.root
inputfpPbPRMC=/home/songkyo/kyo/pPbDataSample/EfficiencySample/tot_JPsiWithFSR_pa_2nd_run_STARTHI53_V27-v1_noAccCut_sglTrig_genMatch-v1.root
inputfpPbNPMC=/home/songkyo/kyo/pPbDataSample/EfficiencySample/tot_inclBtoJPsiMuMu_pa_2nd_run_STARTHI53_V27-v1_noAccCut_sglTrig_genMatch-v1.root

##-----input for Z vertex weighting
##### 1) new acc cut && official
zPbpPRMC=/home/songkyo/kyo/2Dfitting/rooDataSet/zVtx201603/zVtx_G1_Pbp_isPrompt1_isOldAccCut0_isOfficial1.root
zPbpNPMC=/home/songkyo/kyo/2Dfitting/rooDataSet/zVtx201603/zVtx_G1_Pbp_isPrompt0_isOldAccCut0_isOfficial1.root
zpPbPRMC=/home/songkyo/kyo/2Dfitting/rooDataSet/zVtx201603/zVtx_G1_pPb_isPrompt1_isOldAccCut0_isOfficial1.root
zpPbNPMC=/home/songkyo/kyo/2Dfitting/rooDataSet/zVtx201603/zVtx_G1_pPb_isPrompt0_isOldAccCut0_isOfficial1.root

###########################################################################################
###########################################################################################
###########################################################################################

###### -----other options
trigType=1 #L1DoubleMuOpen_v1 (bit 1,2,4,8,...) 
accType=2 # 1=oldAccCut, 2=newAccCut, 3=preLooseCut, 4=2015PbPb
datazweighting=0
mczweighting=1
initev=0
nevt=-1
#nevt=100000
#mergeData=0

function program {
  if [ ! -d $1 ]; then
    mkdir $1
  else
    echo " "
    echo "===== Target directory exists! Check is it okay to delete or not.";
  fi
  ./Tree2DatasetsPA =t $2 =a $3 =w $4 =m $5 =n $6 $7 =f $8 $1 >& $1/log &
}

function programMC {
  if [ ! -d $1 ]; then
    mkdir $1
  else
    echo " "
    echo "===== Target directory exists! Check is it okay to delete or not.";
  fi
  ./Tree2DatasetsPAMC =t $2 =a $3 =w $4 =m $5 =n $6 $7 =z $8 =f $9 $1 >& $1/log &
}

### 1. Data
## -- run for Pbp
program outRoo_Data_Pbp_newcut_v1 $trigType $accType $datazweighting 1 $initev $nevt $inputfpbpV1
program outRoo_Data_Pbp_newcut_v2 $trigType $accType $datazweighting 2 $initev $nevt $inputfpbpV2
## -- run for pPb
program outRoo_Data_pPb_newcut  $trigType $accType $datazweighting 0 $initev $nevt $inputfppb

### 2. MC
## -- run for Pbp -MC
#programMC outRoo_PRMC_Pbp_newcut_off2M $trigType $accType $mczweighting 0 $initev $nevt $zPbpPRMC $inputfPbpPRMC
#programMC outRoo_NPMC_Pbp_newcut_off2M $trigType $accType $mczweighting 0 $initev $nevt $zPbpNPMC $inputfPbpNPMC
## -- run for pPb -MC
#programMC outRoo_PRMC_pPb_newcut_off2M $trigType $accType $mczweighting 0 $initev $nevt $zpPbPRMC $inputfpPbPRMC
#programMC outRoo_NPMC_pPb_newcut_off2M $trigType $accType $mczweighting 0 $initev $nevt $zpPbNPMC $inputfpPbNPMC

