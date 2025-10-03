# ===== Run3 PF-Scouting Standalone Makefile (CMSSW_15_0_6) =====

# Compiler & flags
COMP  ?= g++
FLAGS ?=
FLAGS += -std=c++20 -Wno-attributes -O2 -g -fPIC -fdiagnostics-color=always
FLAGS += -DSAVE_ALL_HISTOGRAMS
FLAGS += -DBOOST_BIND_GLOBAL_PLACEHOLDERS
#FLAGS += -DUSE_EXAMPLE
#FLAGS += -DCREATE_OPT_CUT_FILE

# Query CMSSW externals with scram (works after `cmsenv`)
scram = $(shell scram tool tag $(1) $(2))

# ROOT
ROOTCFLAGS := $(shell root-config --cflags)
ROOTLIBS   := $(shell root-config --glibs) -lMinuit

# CMSSW lib/include paths
CMSSW_LIBDIR := ${CMSSW_RELEASE_BASE}/lib/${SCRAM_ARCH}
CMSSW_INCDIR := ${CMSSW_RELEASE_BASE}/src

# --- fmt from CMSSW externals
FMT_BASE   := $(call scram,fmt,FMT_BASE)
FMT_LIBDIR := $(call scram,fmt,LIBDIR)

# (Optional) Boost headers if you need them in your sources
# BOOST_BASE := $(call scram,boost,BOOST_BASE)

# Include paths (project + CMSSW + externals)
INC  = -I. -I.. -I./include -I$(CMSSW_INCDIR)
# If you also keep private code in your dev area:
INC += -I${CMSSW_BASE}/src
# External fmt
INC += -I$(FMT_BASE)/include
# Optional extras:
# INC += -I$(BOOST_BASE)/include

# Guard CLHEP so it doesn't become "-I/include" when unset
ifdef CLHEP
  INC  += -I${CLHEP}/include
endif

# ROOT headers
INC += $(ROOTCFLAGS)

# >>> CMSSW JEC headers (so short names resolve, e.g. "JetCorrectorParameters.h")
INC += -I$(CMSSW_INCDIR)/CondFormats/JetMETObjects/interface
INC += -I$(CMSSW_INCDIR)/JetMETCorrections/Objects/interface

# Libraries
LIBS  = -L$(CMSSW_LIBDIR) $(ROOTLIBS)
LIBS += -lCondFormatsJetMETObjects -lJetMETCorrectionsObjects -lFWCoreMessageLogger
LIBS += -L$(FMT_LIBDIR) -lfmt
ifdef CLHEP
  LIBS += -L${CLHEP}/lib
endif

# RPATHs so executable finds the shared libs at runtime
RPATH = -Wl,-rpath,$(CMSSW_LIBDIR) -Wl,-rpath,$(FMT_LIBDIR)

# Sources
SRC = ./src
EXE = main

# Core objects (keep analysisClass.o and JECTool.o)
OBJS = \
  $(SRC)/rootNtupleClass.o \
  $(SRC)/baseClass.o \
  $(SRC)/analysisClass.o \
  $(SRC)/jsonParser.o \
  $(SRC)/pileupReweighter.o \
  $(SRC)/eventListHelper.o \
  $(SRC)/JECTool.o

# ===== Targets =====
.PHONY: all clean
all: $(EXE)

$(EXE): $(SRC)/$(EXE).o $(OBJS)
	$(COMP) $(FLAGS) $(RPATH) -o $@ $(OBJS) $(SRC)/$(EXE).o $(LIBS)

clean:
	@rm -f $(SRC)/*.o *.lo core core.* *~ *.exe $(EXE)

# ===== Compilation rules (NO dependency generation; no .d files) =====
$(SRC)/%.o: $(SRC)/%.cc
	$(COMP) -c $(INC) $(FLAGS) -o $@ $<
$(SRC)/%.o: $(SRC)/%.cpp
	$(COMP) -c $(INC) $(FLAGS) -o $@ $<
$(SRC)/%.o: $(SRC)/%.cxx
	$(COMP) -c $(INC) $(FLAGS) -o $@ $<
$(SRC)/%.o: $(SRC)/%.C
	$(COMP) -c $(INC) $(FLAGS) -o $@ $<

