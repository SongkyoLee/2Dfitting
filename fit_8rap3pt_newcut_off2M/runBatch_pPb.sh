#!/bin/bash
if [ $# -ne 3 ]; then
  echo "Usage: $0 [Executable] [Input root File] [Prefix] "
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
prmc=/home/songkyo/kyo/2Dfitting/rooDataSet_v60402/outRoo_PRMC_pPb_newcut_off2M/outRoo_PRMC_pPb_newcut_off2M.root
### Non-prompt MC
npmc=/home/songkyo/kyo/2Dfitting/rooDataSet_v60402/outRoo_NPMC_pPb_newcut_off2M/outRoo_NPMC_pPb_newcut_off2M.root

########### options
sysString="nominal" ## sys01_01-05, sys02_01, sys03_01-02, sys04_01
weight=1  #0: Do NOT weight(dataJpsi), 1: Do weight(dataJpsiWeight)
isMerge=0 # 1 merging the dataset from two files (v1 and v2)
isPA=2 # 0:pp, 1:Pbp, 2:pPb
eventActivity=0 #0:nothing 1:Ntrack 2:ET^{HF}
### read the ctauErrFile or not
readct=0
cterrfile=$(pwd)/outCtErr/fit_ctauErrorRange_pPb.txt

#mSigF="G1CB1Sig" # Mass signal function name (options: sigCB2WNG1 (default), signalCB3WN)
#mBkgF="expBkg" # Mass background function name (options: expFunct (default), polFunct)
ctaurange=1.5-3.0

rapbins=(-2.4--1.93 -1.93--1.46 -1.46--1.03 -1.03--0.43 -0.43-0.47 0.47-1.37 1.37-1.97 1.97-2.4)
ptbins=(5.0-6.5 6.5-10.0 10.0-30.0)
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
  printf "#!/bin/csh\n" > $scripts/$work.csh
	### only for lxplus
	#printf "source /afs/cern.ch/sw/lcg/external/gcc/4.7/x86_64-slc6/setup.csh; source /afs/cern.ch/work/k/kyolee/public/root_v5.28.00d/bin/thisroot.csh\n" >> $scripts/$work.csh
	# printf "cp %s/%s.csh %s/fit2DData.h %s/fit2DData_all.cpp .\n" $scripts $work $(pwd) $(pwd) >> $scripts/$work.csh
  script="$executable -d $dirname -f $isMerge $datasets -m $prmc $npmc -w $weight -c $isPA $eventActivity -s $sysString -x $readct $cterrfile -p $pt -y $rap -l $ctaurange -n $ntrk -h $ethf >& $work.log;"
  echo $script >> $scripts/$work.csh
  
      printf "tar zcvf %s.tgz %s*\n" $work $work >> $scripts/$work.csh
      printf "cp %s.tgz %s\n" $work $storage >> $scripts/$work.csh
      #printf "rm -f %s*\n" $work >> $scripts/$work.csh
			#$(pwd)/condor_executable_simple.sh	$work
			#bsub -R "pool>10000" -u songkyo.lee@cer.c -q 1nd -J $work < $scripts/$work.csh
}

################################################################ 
########## Running script with pre-defined binnings
################################################################ 

### 8rap8pt
#for rap in ${rapbins[@]}; do
#	for pt in ${ptbins[@]}; do
#		program $rap $pt $ntrkbins $ethfbins 0.0-100.0
#	done
#done

#program -2.4--1.93 2.0-3.0 $ntrkbins $ethfbins 0.0-100.0
#program 1.97-2.4 2.0-3.0 $ntrkbins $ethfbins 0.0-100.0

program -0.43-0.47 10.0-30.0 $ntrkbins $ethfbins 0.0-100.0
#program -0.43-0.47 6.5-10.0 $ntrkbins $ethfbins 0.0-100.0
