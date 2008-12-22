CC      = g++ -Wall -I"C:\cxxtest"
OPENCVX = ..
CFLAGS  = -I. -I$(OPENCVX) `pkg-config --cflags opencv`
LIBS    = `pkg-config --libs opencv`
CXXTESTGEN = cxxtestgen.pl --error-printer

all: cvpcadiffs.exe cvhaarlike.exe cvanglemean.exe

cvanglemean.exe: cvanglemean.cxx
	echo $(CFLAGS) $(LIBS) | xargs $(CC) cvanglemean.cxx -o $@

cvanglemean.cxx: cvanglemean.hxx
	$(CXXTESTGEN) cvanglemean.hxx -o $@

cvhaarlike.exe: cvhaarlike.cpp
	echo $(CFLAGS) $(LIBS) -I$(OPENCVX)/HaarTraining | xargs g++ cvhaarlike.cpp -o $@

cvpcadiffs.exe: cvpcadiffs.cpp
	echo $(CFLAGS) $(LIBS) | xargs $(CC) cvpcadiffs.cpp -o $@

clean:
	rm -f *~ *.o *.exe core
