CC=g++
CFLAGS= -std=c++11 -g
INCLUDE=-I ./includes
HDF5INCLUDE=-I /usr/include/hdf5/serial
HDF5LIB=/usr/lib
LIBOBJ=./Linux/libobj/
BINOBJ=./Linux/binobj/
LIBDIR=./Linux/lib/
OBJLIST=Factor.o FactorUtilities.o PGMUtilities.o CliqueTree.o FactorIO.o

default:
	@echo "PGM Make File v0.1"
	@echo "use make all"
	@echo $(OBJLIST)

libPGM.a: $(OBJLIST) 
	ar rcs $(LIBDIR)libPGM.a $(foreach n,$(OBJLIST),$(LIBOBJ)$(n))

Factor.o: ./Factors/Factor.cpp 
	$(CC) $(CFLAGS) $(INCLUDE) -c ./Factors/Factor.cpp -o $(LIBOBJ)Factor.o

FactorUtilities.o: ./Factors/FactorUtilities.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c ./Factors/FactorUtilities.cpp -o $(LIBOBJ)FactorUtilities.o

PGMUtilities.o: ./PGMUtilities/PGMUtilities.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c ./PGMUtilities/PGMUtilities.cpp -o $(LIBOBJ)PGMUtilities.o

CliqueTree.o: ./CliqueTree/CliqueTree.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c ./CliqueTree/CliqueTree.cpp -o $(LIBOBJ)CliqueTree.o

FactorIO.o: ./FactorIO/FactorIO.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(HDF5INCLUDE) -c ./FactorIO/FactorIO.cpp -o $(LIBOBJ)FactorIO.o

clean:
	-rm $(LIBOBJ)*.o $(BINOBJ)*.o
