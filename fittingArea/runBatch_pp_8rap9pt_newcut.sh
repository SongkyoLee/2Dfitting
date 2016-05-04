#!/bin/bash
if [ $# -ne 3 ]; then
  echo "Usage: $0 [Executable] [Input Data] [Dir name] "
  exit;
fi

executable=$(pwd)/$1
datasets=$2
dirname=$3

################################################################ 
########## Script parameter setting
################################################################ 

### Storage for batch jobs (should be under /afs)
storage=$(pwd)/Results/$dirname
scripts=$(pwd)/Scripts
if [ ! -d "$(pwd)/Results" ]; then
  mkdir $(pwd)/Results
fi
if [ ! -d "$storage" ]; then
  mkdir $storage
fi
if [ ! -d "$scripts" ]; then
  mkdir $scripts
fi

### Prompt MC
prmc=/afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_PRMC_pp_newcut_off/outRoo_PRMC_pp_newcut_off.root
### Non-prompt MC
npmc=/afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/rooDataSet/outRoo_NPMC_pp_newcut_off/outRoo_NPMC_pp_newcut_off.root

########### options
sysString="nominal" ## sys01_01,...,sys01_05, sys02_01, sys03_01, sys03_02, sys04_01
mcweight=0  #0: Do NOT mcweight(dataJpsi), 1: Do mcweight(dataJpsiWeight)
dataMerge=1 # number of input data files to be merged
mcMerge=1 # number of input mc files to be merged
isPA=0 # 0:pp, 1:Pbp, 2:pPb, 3:pAMerged
eventActivity=0 #0:nothing 1:Ntrack 2:ET^{HF}
absoluteY=0 #use absolute y binning or not (e.g. 1.6 < |y| < 2.4)
### read the ctauErrFile or not
#readct=0 #0:calculate in the code, 1:read from file, 2:constant
readct=1 #0:calculate in the code, 1:read from file, 2:constant
cterrfile=$(pwd)/outCtErr/fit_ctauErrorRange_pp_8rap9pt_newcut.txt

#mSigF="G1CB1Sig" # Mass signal function name (options: sigCB2WNG1 (default), signalCB3WN)
#mBkgF="expBkg" # Mass background function name (options: expFunct (default), polFunct)
ctaurange=1.5-3.0

rapbins=(-2.40--1.93 -1.93--1.50 -1.50--0.90 -0.90-0.00 0.00-0.90 0.90-1.50 1.50-1.93 1.93-2.40)
ptbins=(3.0-4.0 4.0-5.0 5.0-6.5 6.5-7.5 7.5-8.5 8.5-10.0 10.0-14.0 14.0-30.0)
#rapbins=(-1.93--1.50 -1.50--0.90 -0.90-0.00 0.00-0.90 0.90-1.50 1.50-1.93)
#ptbins=(5.0-6.5 6.5-10.0 10.0-30.0)
ethfbins=(0.0-120.0)
ntrkbins=(0.0-350.0)

################################################################ 
########## Information print out
################################################################ 
txtrst=$(tput sgr0)
txtred=$(tput setaf 2)  #1 Red, 2 Green, 3 Yellow, 4 Blue, 5 Purple, 6 Cyan, 7 White
txtbld=$(tput bold)     #dim (Half-bright mode), bold (Bold characters)
echo "Run fits with ${txtbld}${txtred}$executable${txtrst} on ${txtbld}${txtred}$datasets${txtrst}."
if [ "$storage" != "" ]; then
  echo "Results will be placed on ${txtbld}${txtred}$storage${txtrst}."
fi

################################################################ 
########## Function for progream running
################################################################ 
function program {
  ### Arguments
  rap=$1
  pt=$2
	ntrk=$3
	ethf=$4

	work=$dirname"_rap"$rap"_pT"$pt"_ntrk"$ntrk"_ET"$ethf; # Output file name has this prefix

  echo "Processing: "$work
  printf "#!/bin/bash\n" > $scripts/$work.sh
	### bottom 2 lines for_LXPLUS
	printf "eval `scramv1 runtime -sh` \n" >> $scripts/$work.sh
	printf "cp %s/%s.sh . \n" $scripts $work >> $scripts/$work.sh
	script="$executable -d $dirname -f $dataMerge $datasets -m $mcMerge $prmc $npmc -w $mcweight -c $isPA $eventActivity -s $sysString -x $readct $cterrfile -p $pt -y $absoluteY $rap -l $ctaurange -n $ntrk -h $ethf >& $work.log;"
  echo $script >> $scripts/$work.sh
  
  printf "tar zcvf %s.tgz %s*\n" $work $work >> $scripts/$work.sh
  printf "cp %s.tgz %s\n" $work $storage >> $scripts/$work.sh
  printf "rm -f %s*\n" $work >> $scripts/$work.sh #for_LXPLUS (batch)
	bsub -R "pool>10000" -u songkyo.lee@cer.c -q 1nd -J $work < $scripts/$work.sh #for_LXPLUS (batch)
	#$(pwd)/condor_executable_simple.sh	$work #for_KUNPL (condor)
}

################################################################ 
########## Running script with pre-defined binnings
################################################################ 

for rap in ${rapbins[@]}; do
	for pt in ${ptbins[@]}; do
		program $rap $pt $ntrkbins $ethfbins
	done
done
program -2.40--1.93 2.0-3.0 $ntrkbins $ethfbins 
program 1.93-2.40 2.0-3.0 $ntrkbins $ethfbins 

### TEST
#program -2.40--1.93 2.0-3.0 $ntrkbins $ethfbins 
#program -2.40--1.93 5.0-6.5 $ntrkbins $ethfbins 
#program -2.40--1.93 8.5-10.0 $ntrkbins $ethfbins 
#program 0.90-1.50 8.5-10.0 $ntrkbins $ethfbins 
#program 1.93-2.40 2.0-3.0 $ntrkbins $ethfbins 
#program 1.93-2.40 14.0-30.0 $ntrkbins $ethfbins 

