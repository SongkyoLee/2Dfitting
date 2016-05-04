#!/bin/bash
#eval `scramv1 runtime -sh`

###########################################################################################
####################################### input data ######################################## 
###########################################################################################
inputf=root://eoscms//eos/cms
#inputfpp=$inputf/store/group/phys_heavyions/dileptons/Data2015/pp502TeV/TTrees/PromptAOD/OniaTree_DoubleMu_Run2015E-PromptReco-v1_Run_262157_262328_noCUT_TRKMU.root
inputfpp=/storage/OniaTree/Onia5TeV/ppData/OniaTree_DoubleMu_Run2015E-PromptReco-v1_Run_262157_262328_noCUT_TRKMU.root

#-----input MC
#inputfppPRMC=$inputf/store/group/phys_heavyions/dileptons/MC2015/pp502TeV/TTrees/OniaTree_JpsiMM_5p02TeV_TuneCUETP8M1_Trk_HINppWinter16DR-75X_mcRun2_asymptotic_ppAt5TeV_v3-v1.root
#inputfppNPMC=$inputf/store/group/phys_heavyions/dileptons/MC2015/pp502TeV/TTrees/OniaTree_BJpsiMM_5p02TeV_TuneCUETP8M1_Trk_HINppWinter16DR-75X_mcRun2_asymptotic_ppAt5TeV_v3-v1.root
inputfppPRMC=/storage/OniaTree/Onia5TeV/ppOfficialMC/OniaTree_JpsiMM_5p02TeV_TuneCUETP8M1_Trk_HINppWinter16DR-75X_mcRun2_asymptotic_ppAt5TeV_v3-v1.root
inputfppNPMC=/storage/OniaTree/Onia5TeV/ppOfficialMC/OniaTree_BJpsiMM_5p02TeV_TuneCUETP8M1_Trk_HINppWinter16DR-75X_mcRun2_asymptotic_ppAt5TeV_v3-v1.root

##-----input for Z vertex weight
# new cut
zppPRMC=/home/songkyo/kyo/2Dfitting/rooDataSet/zVtx201604/zVtx_G1_pp_isPrompt1_isNewAccCut1_isOfficial1.root
zppNPMC=/home/songkyo/kyo/2Dfitting/rooDataSet/zVtx201604/zVtx_G1_pp_isPrompt1_isNewAccCut1_isOfficial1.root
# old cut
#zppPRMC=/home/songkyo/kyo/2Dfitting/rooDataSet/zVtx201604/zVtx_G1_pp_isPrompt1_isNewAccCut0_isOfficial1.root
#zppNPMC=/home/songkyo/kyo/2Dfitting/rooDataSet/zVtx201604/zVtx_G1_pp_isPrompt1_isNewAccCut0_isOfficial1.root

##-----other options
trigType=1 #L1DoubleMuOpen_v1 (bit 1,2,4,8,...) 
accType=2 # 1=oldAccCut, 2=newAccCut, 3=preLooseCut, 4=2015PbPb
datazweighting=0
#mczweighting=1 #0 for pp, 1 for pPb
mczweighting=0 #0 for pp, 1 for pPb
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
  ./Tree2DatasetsPP =t $2 =a $3 =w $4 =m $5 =n $6 $7 =f $8 $1 >& $1/log &
}

function programMC {
  if [ ! -d $1 ]; then
    mkdir $1
  else
    echo " "
    echo "===== Target directory exists! Check is it okay to delete or not.";
  fi
  ./Tree2DatasetsPPMC =t $2 =a $3 =w $4 =m $5 =n $6 $7 =z $8 =f $9 $1 >& $1/log &
#  ./Tree2DatasetsPPMCZvtx =t $2 =a $3 =w $4 =m $5 =n $6 $7 =z $8 =f $9 $1 >& $1/log &
}

### 1. Data
program outRoo_Data_pp_newcut $trigType $accType $datazweighting 0 $initev $nevt $inputfpp
### 2. MC
programMC outRoo_PRMC_pp_newcut_off $trigType $accType $mczweighting 0 $initev $nevt $zppPRMC $inputfppPRMC
programMC outRoo_NPMC_pp_newcut_off $trigType $accType $mczweighting 0 $initev $nevt $zppNPMC $inputfppNPMC

