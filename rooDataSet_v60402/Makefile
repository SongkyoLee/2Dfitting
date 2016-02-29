ROOTCFLAGS	=	$(shell root-config --cflags)
ROOTGLIBS		=	$(shell root-config --glibs)

CPP					=	g++
CPPFLAGS		=	-g -fPIC -Wno-deprecated -O2 -ansi
LD					=	g++
LDFLAGS			=	-g
SOFLAGS			=	-shared

CPPFLAGS		+= $(ROOTCFLAGS)
NGLIBS			=	$(ROOTGLIBS)
#NGLIBS      += -L/afs/cern.ch/user/k/kyolee/public/root_v5.28.00h/lib -lMathMore -lMinuit -lRooFit -lRooFitCore -lFoam
#NGLIBS      += -L/data/root-v5.28.00h/lib -lMathMore -lMinuit -lRooFit -lRooFitCore -lFoam
#NGLIBS      += -L/data/root-v5.34.21/lib -lMathMore -lMinuit -lRooFit -lRooFitCore -lFoam
NGLIBS      += -L/data/root-v6.04.02/lib -lMathMore -lMinuit -lRooFit -lRooFitCore -lFoam
GLIBS				= $(filter-out -lNew, $(NGLIBS))

INCLUDEDIR	=	./
CPP					+= -I$(INCLUDEDIR)
#CPP         += -I/afs/cern.ch/user/k/kyolee/public/root_v5.28.00h/include
#CPP         += -I/data/root-v5.28.00h/include
#CPP         += -I/data/root-v5.34.21/include
CPP         += -I/data/root-v6.04.02/include
OUTLIB			= ./

.SUFFIXES:	.cc,.C,.hh,.h
.PREFIXES:	./

RooHistPdfConv.o: $(INCLUDEDIR)/RooHistPdfConv.cpp
	$(CPP) $(CPPFLAGS) -c -o $(OUTLIB)/libRooHistPdfConv.o $(NGLIBS) $<

RooExp2.o: $(INCLUDEDIR)/RooExp2.cpp
	$(CPP) $(CPPFLAGS) -c -o $(OUTLIB)/libRooExp2.o $(NGLIBS) $<


Tree2DatasetsPA:	$(INCLUDEDIR)tree2Datasets_pa.cpp
	$(CPP) $(CPPFLAGS) -o Tree2DatasetsPA $(GLIBS) $ $<

Tree2DatasetsPAMC:	$(INCLUDEDIR)tree2Datasets_pa_mc.cpp
	$(CPP) $(CPPFLAGS) -o Tree2DatasetsPAMC $(GLIBS) $ $<

Tree2DatasetsPAMCZvtx:	$(INCLUDEDIR)tree2Datasets_pa_mc_zvtx.cpp
	$(CPP) $(CPPFLAGS) -o Tree2DatasetsPAMCZvtx $(GLIBS) $ $<

Tree2DatasetsPP: $(INCLUDEDIR)tree2Datasets_pp.cpp
	$(CPP) $(CPPFLAGS) -o Tree2DatasetsPP $(GLIBS) $ $<

Tree2DatasetsPPAndre: $(INCLUDEDIR)tree2Datasets_pp_Andre.cpp
	$(CPP) $(CPPFLAGS) -o Tree2DatasetsPPAndre $(GLIBS) $ $<

Tree2DatasetsPPMC:	$(INCLUDEDIR)tree2Datasets_pp_mc.cpp
	$(CPP) $(CPPFLAGS) -o Tree2DatasetsPPMC $(GLIBS) $ $<


DrawVariables:	$(INCLUDEDIR)draw_variables.cpp
	$(CPP) $(CPPFLAGS) -o DrawVariables $(GLIBS) $ $<

DrawVariablesSglMuTest:	$(INCLUDEDIR)draw_variables_sglmutest.cpp
	$(CPP) $(CPPFLAGS) -o DrawVariablesSglMuTest $(GLIBS) $ $<

DrawZvtx1File:	$(INCLUDEDIR)draw_zVtx_1file.cpp
	$(CPP) $(CPPFLAGS) -o DrawZvtx1File $(GLIBS) $ $<

DrawM1File:	$(INCLUDEDIR)draw_m_1file.cpp
	$(CPP) $(CPPFLAGS) -o DrawM1File $(GLIBS) $ $<

DrawWeightComp:	$(INCLUDEDIR)draw_weightComp.cpp
	$(CPP) $(CPPFLAGS) -o DrawWeightComp $(GLIBS) $ $<

DrawWeightCompRatio:	$(INCLUDEDIR)draw_weightComp_ratio.cpp 
	$(CPP) $(CPPFLAGS) -o DrawWeightCompRatio $(GLIBS) $ $<


clean:
	rm -f $(OUTLIB)*.o $(OUTLIB)*.so
