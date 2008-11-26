CC      = g++
CFLAGS  = -O3 -I. -I.. `pkg-config --cflags opencv`
LIBS    = `pkg-config --libs opencv`

all: pca.exe

pca.exe: pca.cpp ../cvxpca.h
	echo $(CFLAGS) $(LIBS) | xargs $(CC) pca.cpp -o $@

clean:
	rm -f *~ *.o *.exe core
