#!/bin/python
import sys,string,re,math,subprocess
from os import listdir
from operator import itemgetter

### Prefix used in Fit2DDataPbPb fitting step
### (in rfcp.sh : python $workdir/extractpy $prefix ../$prefix)
if (len(sys.argv) < 3):
  print "Usage: python %s [Prefix] [Directory of result files 1] [Directory of result files 2] ..." %(sys.argv[0])
  sys.exit()
else:
  prefix=str(sys.argv[1]);

### All fit parameters will be written here
fparam = open("fit_parameters","w")
### Number of signal and background yields will be written here
ftable = open("fit_table","w")
### Print bins with large NSigErr or sigWidthErr (inclusive fits)
ferrorbin = open("fit_errorbins","w")
### ctau error range list
fctauerr = open("fit_ctauErrorRange","w")

### Parameters without error values in the output files
nonErrParam=("NLL","nFullBinsResid","RSS","All","Signal", "UnNormChi2_mass", "nFitParam_mass", "nFullBinsPull_mass", "Dof_mass", "Chi2_mass", "theChi2Prob_mass", " UnNormChi2_time", "nFitParam_time", "nFullBinsPull_time", "Dof_time", "Chi2_time", "theChi2Prob_time", "UnNormChi2_side", "nFitParam_side", "nFullBinsPull_side", "Dof_side", "Chi2_side", "theChi2Prob_side")
### Parameters with error values but errors will be dropped in the excel file
dropErrParam=("ctauErrMin","ctauErrMax","NoCutEntry","CutEntry")

for f in range(2,len(sys.argv)):
  folder = sys.argv[f]
  print folder
  ## writing prefix on the 1st line of output files
  fparam.write(folder + "\n")
  ftable.write(folder + "\n")
  fctauerr.write(folder + "\n")

  ## Reading 1 fitting method, various bins
  filelist = listdir(folder)
  datapar = []
  datatab = []
  datactauerr = []
  duplicateCheck = []
  for files in filelist:  # 1 bin per a line
    par = []
    table = []
    ctauerr = []

    try:
      rap = re.search('rap-?\d{1,3}\.\d{1,3}--?\d{1,3}\.\d{1,3}',files).group()
      pt = re.search('pT\d+\.\d-\d+\.\d',files).group()
      ntrk = re.search('ntrk\d+\.?\d?-\d+\.?\d?',files).group()
      et = re.search('ET\d+\.?\d?-\d+\.?\d?',files).group()
    except:
      continue;

    ### form a txt file name to read out
    filename = prefix + "_" + rap + "_" + pt + "_" + ntrk + "_" + et ;
    try:
      finput = open("./"+folder+"/"+filename+".txt",'r')
    except:
      filename = prefix + "_" + rap + "_" + pt + "_" + et + "_" + ntrk ;
      try:
        finput = open("./"+folder+"/"+filename+".txt",'r')
      except:
        print "./"+folder+"/"+filename+".txt doesn't exist"
        continue;

    rapN = rap.split("rap")[1]
    ptN = pt.split("pT")[1]
    ntrkN = ntrk.split("ntrk")[1]
    etN = et.split("ET")[1]
    
    ### append values from txt to output file
    par.append(rapN)
    par.append(ptN)
    par.append(ntrkN)
    par.append(etN)
    table.append(rapN)
    table.append(ptN)
    table.append(ntrkN)
    table.append(etN)
    ctauerr.append(rapN)
    ctauerr.append(ptN)
    ctauerr.append(ntrkN)
    ctauerr.append(etN)

    header = "rap\tpT\tntrk\tEt\t"
    headerShort = "rap\tpT\tntrk\tEt\t"
    headerError = "rap\tpT\t\tntrk\tEt\t"

    for line in finput: # Read one .txt file (result file)
      tmp = line.split(" ")
      
      if [check for check in dropErrParam if tmp[0] in check]:  #Drop error on dropErrParam and write
        header = header + str(tmp[0]) + "\t"
        par.append(float(tmp[1]))
      
      for i in tmp: # tmp[x] holds 1 column: [0] variable name, [1] value, [2] error
        if i == "ctauErrMin" or i == "ctauErrMax" or i == "NoCutEntry" or i == "CutEntry" :
          try:
            ctauerr.append (float(tmp[1]))
          except:
            pass

        if i == "NSig" or i == "NBkg" or i == "PROMPT" or i == "NON-PROMPT" or i == "Bfraction" :
          try:
            table.append (float(tmp[-2]))
            table.append (float(tmp[-1]))
          except:
            pass
          
          ### If this parameter doesn't have error values in the output file
          if [check for check in nonErrParam if str(i) in check]:
            headerShort = headerShort + str(i) + "\t"
          else:
            headerShort = headerShort + str(i) + "\t" + str(i) + "Err\t"

        ### Skip dropErrParam elements, cause they were written before this loop was started
        if [check for check in dropErrParam if tmp[0] in check]:
          continue

        try:
          par.append(float(i))
        except:
          ### If this parameter doesn't have error values in the output file
          if [check for check in nonErrParam if str(i) in check]:
            header = header + str(i) + "\t"
          elif [check for check in dropErrParam if str(i) in check]:
            header = header + str(i) + "\t"
          else:
            header = header + str(i) + "\t" + str(i) + "Err\t"
      ### End of (for i in tmp:)
    ### End of analyzing 1 line (for line in finput:)

    ### Check if this bin is already analyzed or not
    thisEntryHas = rapN+" "+ptN+" "+ntrkN+" "+etN
    if [ check for check in duplicateCheck if str(thisEntryHas) in str(check) ]:
      continue
    else:
      ### This bin wasn't analyzed yet! Put 1 line for each bin (1 .txt file)
      duplicateCheck.append(rapN+" "+ptN+" "+ntrkN+" "+etN)
      datapar.append(par)
      datatab.append(table)
      datactauerr.append(ctauerr)

  ### Write fit_parameters with All results
  if f is 2:
    fparam.write(header+"\n")
  dataparfin = sorted(datapar, key=itemgetter(0,1,2,3)) #sorting from 0:rap, 1:pT, 2:ntrk. 3:et
  for i in dataparfin:
    line = ""
    for j in i:
      line = line+str(j)+"\t"
    fparam.write(line+"\n")

  ### Write fit_table with yield-related results
  if f is 2:
    ftable.write(headerShort+"\n")
  datatabfin = sorted(datatab, key=itemgetter(0,1,2,3))
  for i in datatabfin:
    line = ""
    for j in i:
      line = line+str(j)+"\t"
    ftable.write(line+"\n")

  ### Write fit_errobins with large errors on incusive fits
  
  ferrorbin.write("rap\tpT\tntrk\tET\tNSig\tNSigErr\tsigWidth\tsigWidthErr\n");

  for i in dataparfin:
    ### Check "NSigErr[9]" and "SigWidthErr[29]". if too large, write them onto a errorbin file
    ### KYO : modify arrary numbers by hand !!!
    if i[8]*0.15 < i[9] or ((i[28]*0.2 < i[29]) and (i[29] > 0.01)) :
      ferrorbin.write(str(i[0])+"\t"+str(i[1])+"\t"+str(i[2])+"\t"+str(i[3])+"\t")
      ferrorbin.write(str(i[8])+"\t"+str(i[9])+"\t")
      ferrorbin.write(str(i[28])+"\t"+str(i[29])+"\n")

  ### Write fit_ctauErrorRange
  if f is 2:
    fctauerr.write("rap\tpT\tntrk\tET\tctauErrMin\tctauErrMax\tNoCutEntry\tCutEntry\n")
  datactauerrfin = sorted(datactauerr, key=itemgetter(0,1,2,3,4,5))
  for i in datactauerrfin:
    line = ""
    for j in i:
      line = line+str(j)+"\t"
    fctauerr.write(line+"\n");
''' 
  # write a file to create a root file with nSig, nPrompt, nNon-prompt 
  
  sigAry = [];
  bkgAry = [];
  prAry = [];
  npAry = [];
  bAry = [];
  sigEAry = [];
  bkgEAry = [];
  prEAry = [];
  npEAry = [];
  bEAry = [];

  for i in datatabfin:
    headerbin = "rap" + i[0] + "_pT" + i[1] + "_cent" + i[2] + "_ntrk" + i[4] + "_ET" + i[5] + "\n";
#    thisbin = "rap" + i[0] + "_cent" + i[2] +  "_pT" + i[1] +"_dPhi" + i[3];
    thisbin = "rap" + i[0] + "_pT" + i[1] + "_cent" + i[2] + "_dPhi" + i[3] + "_ntrk" + i[4] + "_ET" + i[5] ;

    sigAry.append(i[6]);
    sigEAry.append(i[7]);
    bkgAry.append(i[8]);
    bkgEAry.append(i[9]);
    prAry.append(i[10]);
    prEAry.append(i[11]);
    npAry.append(i[12]);
    npEAry.append(i[13]);
    bAry.append(i[14]);
    bEAry.append(i[15]);
''' 
fparam.close();
ftable.close();
ferrorbin.close();
fctauerr.close();
