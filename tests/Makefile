CC      = g++ -Wall
OPENCVX = ../..
CFLAGS  = -I. -I$(OPENCVX) `pkg-config --cflags opencv`
LIBS    = `pkg-config --libs opencv`

all: cvpcadiffs.exe

cvpcadiffs.exe: cvpcadiffs.cpp $(OPENCVX)/opencvx/cvpcadiffs.h
	echo $(CFLAGS) $(LIBS) | xargs $(CC) cvpcadiffs.cpp -o $@

clean:
	rm -f *~ *.o *.exe core
