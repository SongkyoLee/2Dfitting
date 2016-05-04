#!/bin/bash -f

########## Directory where the job submittion performed
submitdir="$(pwd)/Scripts/"
########## Castor directory that contains results
resdir_="$(pwd)/Results/"
########## Directory where python & root scripts are located
workdir="$(pwd)"
########## Prefix of jobs
prefixarr=$(ls $resdir_)
echo " *** prefix: "$prefixarr

############################################################
eval `scramv1 runtime -sh`

############################################################
########## Copy files from castor and extract it. Run python & root scripts over all files
for prefix in ${prefixarr[@]}; do
  mkdir /tmp/kyolee/$prefix
  cd /tmp/kyolee/$prefix

  resdir=$resdir_/$prefix
  if [ ! -d $resdir ]; then
    echo $resdir "is not a directory."
    continue
  fi

  echo " *** result to be handled in resdir: "$resdir
  tarfilelist=$(ls $resdir | grep tgz)

  for tarfile in $tarfilelist; do   # Get files from castor using prefix
    if echo $tarfile | grep -q $prefix; then 
      echo $tarfile
      cp $resdir/$tarfile .
      tar zfx ./$tarfile
      rm -f ./$tarfile
    fi
  done

  ############################################################
  ### Run python & root script for 1 prefix
  python $workdir/extract.py $prefix ../$prefix

  ############################################################
  ### Summarize results
  mkdir /tmp/kyolee/$prefix/summary
  mv fit_parameters fit_table fit_errorbins fit_ctauErrorRange /tmp/kyolee/$prefix/summary

  ls $submitdir | grep $prefix | awk 'BEGIN{FS=".csh"}; {print $1}' > $submitdir/$prefix\_submit
  ls /tmp/kyolee/$prefix | grep txt | awk 'BEGIN{FS=".txt"}; {print $1}' > $submitdir/$prefix\_txt
  diff $submitdir/$prefix\_submit $submitdir/$prefix\_txt > $submitdir/diff_$prefix

  ### tar them and put into results
  #tar zfc $resdir_/$prefix.tgz /tmp/kyolee/$prefix
  cp -r /tmp/kyolee/$prefix $resdir_/tar_$prefix
  cd $resdir_
  tar zfc tar_$prefix.tgz tar_$prefix
done
