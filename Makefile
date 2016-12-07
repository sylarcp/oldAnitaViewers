# Makefile
#
# ANITA Viewer

#WXWIN=/usr/local/wxgtk2
WXVERSION:=$(shell echo `wx-config --version`)
CXX=g++
#RCFILEOBJ=AnitaViewer_rc.o
#RCFILESRC=AnitaViewer.rc
TARGETS=AnitaViewer TrigMon SlowMo
TARBALL_FILES=*_splash.png Aview-howto.html latestAcqdPeds.dat
SOURCE_FILES=*.cpp *.h *.dtd README.txt README_STATIC TODO Makefile $(TARBALL_FILES)

# For development
#DEBUG=yes
# Debug
ifeq ($(DEBUG),yes)
DEBUGFLAGS=-DDEBUG -D_WXDEBUG_ -g
OPTFLAGS=-O0
PATHBASE=Debug
else
DEBUGFLAGS=
OPTFLAGS=-O3
#OPTFLAGS=-00
PATHBASE=
endif

LINKERFLAGS=-L${ANITA_FLIGHT_SOFT_DIR}/lib
ifeq ($(WIN),yes)
OUTPUT=MSW
OBJECT=MSW
EXE=.exe
STATIC=yes
STATIC_LIBS=
OS_LIBS=-lwsock32 -lwinmm -ladvapi32
CPPFLAGS= -DWIN32 -D__WXMSW__ -D__GNUWIN32__ -D__WIN95__
CPPINC=-I/usr/local/include -I/usr/local/pgsql/include
LINKERFLAGS+=-L/usr/local/pgsql/lib
FLIGHTSOFT_LIB=-lCompress -lPedestal -lz

else
EXE=
OUTPUT=GTK2
OBJECT=GTK2
TARBALL_DIR=ANITA_GSE_LINUX
#TARGETS+=TrigMon
ifeq ($(NONFC),yes)
STATIC_LIBS+=-lcairo -lpangocairo-1.0 -lXau
endif

ifeq ($(SL5),yes)
# this is for SL5 and later
STATIC_LIBS+=-lrt -lXft -lcairo -lpangocairo-1.0 -lpangoft2-1.0 -lfontconfig -lfreetype -lXi -lXrandr -lXcursor -lXrender -lXext -lexpat -lssl -lcrypto -lkrb5 -lkrb5support -lcom_err -lcrypt -lresolv -lnsl -lk5crypto -lkeyutils -lselinux -lXfixes -lX11 -lXau -lXdmcp -lsepol
else
# this is for FC3 / SL4 (original)
STATIC_LIBS+=-lXft -lpangoft2-1.0 -lfontconfig -lfreetype -lXi -lXrandr -lXcursor -lXrender -lXext -lX11 -lexpat -lssl -lcrypto -lkrb5 -lcom_err -lcrypt -lresolv -lnsl -lgssapi_krb5 -lk5crypto
endif
FLIGHTSOFT_LIB=-lCompress -lPedestal -lUtil -lz
CPPFLAGS=-DLINUX
CPPINC=
LINKERFLAGS+=
endif

# Static
ifeq ($(STATIC),yes)
LIBS=`wx-config --version=2.6 --libs --static` -lpq -lfftw3 $(STATIC_LIBS) $(FLIGHTSOFT_LIB) -lm
#LIBS=`wx-config --version=2.6 --libs --static` $(STATIC_LIBS) -lpq -lm
LINKERFLAGS+=-static
CPPINC+=`wx-config --version=2.6 --cxxflags --static` -I$(ANITA_FLIGHT_SOFT_DIR)/common -I/usr/include/postgresql/
else
LIBS=`wx-config --version=2.6 --libs` -lpq -lfftw3 $(FLIGHTSOFT_LIB) -lm
LINKERFLAGS+=
CPPINC+=`wx-config --version=2.6 --cxxflags` -I$(ANITA_FLIGHT_SOFT_DIR)/common -I/usr/include/postgresql/
endif

OUTPUTPATH=$(addprefix $(PATHBASE),$(OUTPUT))
OBJECTPATH=$(addprefix $(PATHBASE),$(OBJECT))

WARNINGFLAGS=-Wall -Wno-strict-aliasing -Wno-deprecated
#CPPINC+=`wx-config --version=2.6 --cxxflags` -I$(ANITA_FLIGHT_SOFT_DIR)/common -I/usr/include/postgresql/
LDFLAGS=$(LINKERFLAGS) $(LIBS) $(OS_LIBS)
CPPFLAGS+=$(CPPINC) $(GCCFLAGS) $(DEBUGFLAGS) $(OPTFLAGS) $(WARNINGFLAGS)

PROGRAMS=$(addprefix $(OUTPUTPATH)/,$(TARGETS))

SOURCES = rangedgauge.cpp rangedstattext.cpp range.cpp historydisplay.cpp \
 historystattext.cpp multigraphdialog.cpp utcdatectrl.cpp sort.cpp BarItem.cpp\
 BaseDBHandler.cpp BaseXMLHandler.cpp BarInfo.cpp TextInfo.cpp graph.cpp \
 codedstattext.cpp DBHandler.cpp BaseControlItem.cpp FlightControlItem.cpp \
 DBControlItem.cpp ControlPanel.cpp DBTerminal.cpp FileHandler.cpp \
 TimeTranslator.cpp rfpower_cal.cpp

ANITAVIEWER_HEADERS = aviewdefs.h options.h options_default.h surfMapping.h

ANITAVIEWER_SOURCES = AviewFrame.cpp AnitaViewer.cpp rftab.cpp rfwindow.cpp \
 HeaderInfo.cpp TriggerInfo.cpp OptionsDialog.cpp payload.cpp vector.cpp \
 rotationMatrix.cpp GPSDataInfo.cpp GPSSatInfo.cpp AuxInfo.cpp \
 AviewXMLHandler.cpp TrigPatternInfo.cpp HistInfo.cpp ConfigViewer.cpp \
 DB_HKViewer.cpp DB_HeaderViewer.cpp  ADU5PatViewer.cpp ADU5SatViewer.cpp \
 ADU5VtgViewer.cpp MonitorViewer.cpp HeaderViewer.cpp HKViewer.cpp \
 HKCalViewer.cpp CommandViewer.cpp DB_MonitorViewer.cpp DB_CmdViewer.cpp \
 G12SatViewer.cpp GpsPosViewer.cpp

ANITAVIEWER_OBJECTS= $(addprefix $(OBJECTPATH)/,$(ANITAVIEWER_SOURCES:.cpp=.o)) $(addprefix $(OBJECTPATH)/,$(SOURCES:.cpp=.o))

TRIGMON_HEADERS = trigmondefs.h trigmon_options.h trigmon_defaults.h

TRIGMON_SOURCES = TrigMon.cpp TrigMonFrame.cpp TURFInfo.cpp SURFInfo.cpp \
 TrigMonXMLHandler.cpp TrigMonOptions.cpp  TurfViewer.cpp SurfViewer.cpp

TRIGMON_OBJECTS= $(addprefix $(OBJECTPATH)/,$(TRIGMON_SOURCES:.cpp=.o)) \
 $(addprefix $(OBJECTPATH)/,$(SOURCES:.cpp=.o))

SLOWMO_HEADERS = slowmo_options.h slowmo_defaults.h

SLOWMO_SOURCES = SlowMo.cpp SlowMoFrame.cpp SlowInfo.cpp SlowMoXMLHandler.cpp \
 SlowMoOptions.cpp SlowMoViewer.cpp

SLOWMO_OBJECTS=$(addprefix $(OBJECTPATH)/,$(SLOWMO_SOURCES:.cpp=.o)) \
 $(addprefix $(OBJECTPATH)/,$(SOURCES:.cpp=.o))

AVIEW_SVNVERSION=`svnversion .`
FLIGHTSOFT_SVNVERSION=`svnversion $(ANITA_FLIGHT_SOFT_DIR)/common`

all:	$(OBJECTPATH)

$(OBJECTPATH):
	mkdir -p $(OBJECTPATH)

clean: cleanexe cleandeps
	rm -f $(OBJECTPATH)/*.o

cleandeps:
	rm -rf .deps

cleanexe:
	rm -f $(addsuffix $(EXE),$(PROGRAMS))

dist:
	mkdir -p aview
	cp $(SOURCE_FILES) aview
	tar czf aview.tgz aview
	rm -rf aview

tarexe:
	make cleanexe
	make STATIC=yes
	strip $(PROGRAMS)
	mkdir -p $(TARBALL_DIR)
	cp $(PROGRAMS) $(TARBALL_DIR)
	cp $(TARBALL_FILES) $(TARBALL_DIR)
	tar czf  $(TARBALL_DIR).tgz $(TARBALL_DIR)
	rm -rf $(TARBALL_DIR)

help:
	@echo "Usage: make -f Makefile WIN=[yes|no] DEBUG=[yes|no] STATIC=[yes|no] [all|clean|cleandeps|cleanexe|dist|tarexe|help|aview|trigmon]"


### Targets ####
addsvnversion:
	sed -i "/ddm_placeholder_for_svnversion/s/ddm_placeholder_for_svnversion/$(FLIGHTSOFT_SVNVERSION) $(AVIEW_SVNVERSION)/" AviewFrame.cpp SlowMoFrame.cpp TrigMonFrame.cpp

all: .deps $(PROGRAMS)

aview: .deps $(OUTPUTPATH)/AnitaViewer

trigmon: .deps $(OUTPUTPATH)/TrigMon

slowmo: .deps $(OUTPUTPATH)/SlowMo

$(OUTPUTPATH)/AnitaViewer:  $(ANITAVIEWER_OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OUTPUTPATH)/TrigMon: $(TRIGMON_OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OUTPUTPATH)/SlowMo: $(SLOWMO_OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

.deps:
	@list='$(SOURCES) $(ANITAVIEWER_SOURCES) $(TRIGMON_SOURCES) $(SLOWMO_SOURCES)'; \
	for file in $$list; do \
	  object=`echo \$$\(OBJECTPATH\)/$$file | sed 's/.cpp$$/.o/'`; \
	  $(CXX) $(CPPINC) -MM -MT $$object -fpch-deps $$file >> $@; \
	done

-include .deps

$(OBJECTPATH)/%.o:
	$(CXX) -c -o $@ $(CPPFLAGS) $<

.PHONY:	all clean
