#!/bin/bash

#sysbins=(nominal)
sysbins=(nominal sys01_01 sys01_02 sys01_03 sys02_01 sys03_01 sys03_02 sys04_01)
binNameArr=("pp_8rap9pt_newcut" "pA_8rap9pt_newcut" "pA_6rap2pt_newcut")
### 66, 66, 36

for binName in ${binNameArr[@]};
do
  for isys in ${sysbins[@]};
  do
    echo "dir :: "$binName"_"$isys
    ls fittingArea/Results/$binName"_"$isys | wc
    ls fittingArea/Results/$binName"_"$isys | wc
  done
done

