CC      = g++
CFLAGS  = -O3 -I. -I.. `pkg-config --cflags opencv`
LIBS    = `pkg-config --libs opencv`

all: cvpcadiffs.exe

cvpcadiffs.exe: cvpcadiffs.cpp ../cvpcadiffs.h
	echo $(CFLAGS) $(LIBS) | xargs $(CC) cvpcadiffs.cpp -o $@

clean:
	rm -f *~ *.o *.exe core
