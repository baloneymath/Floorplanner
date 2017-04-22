# CC and CFLAGS are varilables
CC = g++
CFLAGS = -c -std=c++11
BOOSTFLAGS = -lutil -lboost_iostreams -lboost_system -lboost_filesystem
AR = ar
ARFLAGS = rcv
# -c option ask g++ to compile the source files, but do not link.
# -g option is for debugging version
# -O3 option is for optimized version
OPTFLAGS = -O3

all	: Floorplan.exe
	@echo -n ""

# optimized version
Floorplan.exe 	: B_Tree.o floorplanner.o fastSA.o main.o
				$(CC) $(OPTFLAGS) B_Tree.o floorplanner.o fastSA.o main.o -o Floorplan.exe $(BOOSTFLAGS)
main.o 		: src/main.cpp
			$(CC) $(CFLAGS) $< -o $@ $(BOOSTFLAGS)
B_Tree.o	: src/B_Tree.cpp src/B_Tree.h
			$(CC) $(CFLAGS) $(OPTFLAGS) $< -o $@ $(BOOSTFLAGS)
floorplanner.o	: src/floorplanner.cpp src/floorplanner.h
				$(CC) $(CFLAGS) $(OPTFLAGS) $< -o $@ $(BOOSTFLAGS)
fastSA.o	: src/fastSA.cpp src/fastSA.h
			$(CC) $(CFLAGS) $(OPTFLAGS) $< -o $@ $(BOOSTFLAGS)


# clean all the .o and executable files
clean:
		rm -rf *.o *.gprof gmon.out lib/*.a lib/*.o bin/* Floorplan.exe

