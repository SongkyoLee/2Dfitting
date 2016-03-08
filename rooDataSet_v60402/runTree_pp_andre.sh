#!/bin/bash
#eval `scramv1 runtime -sh`

###########################################################################################
####################################### input data ######################################## 
###########################################################################################
inputf=root://eoscms//eos/cms
### Trk
#inputfpp=$inputf/store/group/phys_heavyions/dileptons/Data2015/pp502TeV/TTrees/PromptAOD/OniaTree_DoubleMu_Run2015E-PromptReco-v1_Run_262157_262328_noCUT_TRKMU.root
#inputfpp=/home/songkyo/kyo/ppDataSample/OniaTree_DoubleMu_Run2015E-PromptReco-v1_Run_262157_262328_noCUT_TRKMU.root
### GlbTrk NOCUT
#inputfpp=$inputf/store/group/phys_heavyions/dileptons/Data2015/pp502TeV/TTrees/PromptAOD/OniaTree_DoubleMu_Run2015E-PromptReco-v1_Run_262157_262328_noCUT.root
#inputfpp=/home/samba/Onia5TeV/ppData/OniaTree_DoubleMu_Run2015E-PromptReco-v1_Run_262157_262328_noCUT.root
### GlbTrk 
#inputfpp=$inputf/store/group/phys_heavyions/dileptons/Data2015/pp502TeV/TTrees/PromptAOD/OniaTree_DoubleMu_Run2015E-PromptReco-v1_Run_262157_262328.root
inputfpp=/home/samba/Onia5TeV/ppData/OniaTree_DoubleMu_Run2015E-PromptReco-v1_Run_262157_262328.root

#-----input MC
## 1) official no acc cut // tmp
#inputfppPRMC=$inputf/store/group/phys_heavyions/dileptons/MC2013/pPb502TeV/PromptJpsi/tot_JPsiWithFSR_pa_1st_run_STARTHI53_V27-v1_noAccCut_sglTrig_genMatch-v2.root
#inputfppNPMC=$inputf/store/group/phys_heavyions/dileptons/MC2013/pPb502TeV/NonPromptJpsi/tot_inclBtoJPsiMuMu_pa_1st_run_STARTHI53_V27-v1_noAccCut_sglTrig_genMatch-v2.root
inputfppPRMC=/home/songkyo/kyo/pPbDataSample/EfficiencySample/tot_JPsiWithFSR_pa_1st_run_STARTHI53_V27-v1_noAccCut_sglTrig_genMatch-v2.root
inputfppNPMC=/home/songkyo/kyo/pPbDataSample/EfficiencySample/tot_inclBtoJPsiMuMu_pa_1st_run_STARTHI53_V27-v1_noAccCut_sglTrig_genMatch-v2.root

##-----input for Z vertex weight
## 1) new acc cut && official // tmp
#zppPRMC=/afs/cern.ch/work/k/kyolee/private/pAJpsi_rooDataSet_20160214/zVtx201602/zVtx_G1_v52800h_is1st1_isPrompt1_isOldAccCut0_isOfficial1.root
#zppNPMC=/afs/cern.ch/work/k/kyolee/private/pAJpsi_rooDataSet_20160214/zVtx201602/zVtx_G1_v52800h_is1st1_isPrompt0_isOldAccCut0_isOfficial1.root

##-----other options
trigType=1 #L1DoubleMuOpen_v1 (bit 1,2,4,8,...) 
accType=4 # 1=oldAccCut, 2=newAccCut, 3=preLooseCut, 4=2015PbPb
zweighting=1 #currently, zVtxWeight!!
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
  #./Tree2DatasetsPP =t $2 =a $3 =w $4 =m $5 =n $6 $7 =f $8 $1 >& $1/log &
  ./Tree2DatasetsPPAndre =t $2 =a $3 =w $4 =m $5 =n $6 $7 =f $8 $1 >& $1/log &
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
program outRoo_Data_pp_andre $trigType $accType $zweighting 0 $initev $nevt $inputfpp
### 2. MC
#programMC outRoo_PRMC_pp_oldcut_off2M $trigType $accType $zweighting 0 $initev $nevt $zppPRMC $inputfppPRMC
#programMC outRoo_NPMC_pp_oldcut_off2M $trigType $accType $zweighting 0 $initev $nevt $zppNPMC $inputfppNPMC

