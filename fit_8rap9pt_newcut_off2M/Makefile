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
NGLIBS      += -L/data/root-v6.04.02h/lib -lMathMore -lMinuit -lRooFit -lRooFitCore -lFoam
GLIBS				= $(filter-out -lNew, $(NGLIBS))

INCLUDEDIR	=	./
CPP					+= -I$(INCLUDEDIR)
#CPP         += -I/afs/cern.ch/user/k/kyolee/public/root_v5.28.00h/include
#CPP         += -I/data/root-v5.28.00h/include
CPP         += -I/data/root-v6.04.02h/include
OUTLIB			= ./

.SUFFIXES:	.cc,.C,.hh,.h
.PREFIXES:	./

RooHistPdfConv.o: $(INCLUDEDIR)/RooHistPdfConv.cpp
	$(CPP) $(CPPFLAGS) -c -o $(OUTLIB)/libRooHistPdfConv.o $(NGLIBS) $<

RooExp2.o: $(INCLUDEDIR)/RooExp2.cpp
	$(CPP) $(CPPFLAGS) -c -o $(OUTLIB)/libRooExp2.o $(NGLIBS) $<

Fit2DDataAllOptions:	$(INCLUDEDIR)fit2DData_allOptions.cpp
	$(CPP) $(CPPFLAGS) -o Fit2DDataAll $(OUTLIB)/*.o $(GLIBS) $ $<

Fit2DDataPA:	$(INCLUDEDIR)fit2DData_pA.cpp
	$(CPP) $(CPPFLAGS) -o Fit2DDataPA $(OUTLIB)/*.o $(GLIBS) $ $<

Fit2DDatapp:	$(INCLUDEDIR)fit2DData_pp.cpp
	$(CPP) $(CPPFLAGS) -o Fit2DDatapp $(OUTLIB)/*.o $(GLIBS) $ $<

clean:
	rm -f $(OUTLIB)*.o $(OUTLIB)*.so
