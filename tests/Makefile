CC      = g++ -Wall -I"C:\cxxtest"
OPENCVX = ..
CFLAGS  = -I. -I$(OPENCVX) -I$(OPENCVX)/HaarTraining `pkg-config --cflags opencv`
LIBS    = `pkg-config --libs opencv`
CXXTESTGEN = cxxtestgen.pl --error-printer
# GNU make
SRCS = $(wildcard *.hxx)
CXXS = $(SRCS:.hxx=.cxx)
EXES = $(CXXS:.cxx=.exe)

all: $(EXES)

%.exe: %.cxx
	echo $(CFLAGS) $(LIBS) | xargs $(CC) $^ -o $@

%.cxx: %.hxx
	$(CXXTESTGEN) $^ -o $@

clean:
	rm -f *~ *.o *.exe core
