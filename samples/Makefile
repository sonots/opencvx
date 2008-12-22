CC      = g++ -Wall
OPENCVX = ..
CFLAGS  = -I. -I$(OPENCVX) `pkg-config --cflags opencv`
LIBS    = `pkg-config --libs opencv`
CXXTESTGEN = cxxtestgen.pl --error-printer
# GNU make
SRCS = $(wildcard *.cpp)
EXES = $(SRCS:.cpp=.exe)

all: $(EXES)

%.exe: %.cpp
	echo $(CFLAGS) $(LIBS) | xargs $(CC) $^ -o $@

clean:
	rm -f *~ *.o *.exe core
