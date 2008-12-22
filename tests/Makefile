CC      = g++ -Wall
OPENCVX = ..
CFLAGS  = -I. -I$(OPENCVX) `pkg-config --cflags opencv`
LIBS    = `pkg-config --libs opencv`

all: cvpcadiffs.exe cvhaarlike.exe

cvhaarlike.exe: cvhaarlike.cpp
	echo $(CFLAGS) $(LIBS) -I$(OPENCVX)/HaarTraining | xargs g++ cvhaarlike.cpp -o $@

cvpcadiffs.exe: cvpcadiffs.cpp
	echo $(CFLAGS) $(LIBS) | xargs $(CC) cvpcadiffs.cpp -o $@

clean:
	rm -f *~ *.o *.exe core
