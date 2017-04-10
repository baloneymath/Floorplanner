# CC and CFLAGS are varilables
CC = g++
CFLAGS = -c -std=c++11
AR = ar
ARFLAGS = rcv
# -c option ask g++ to compile the source files, but do not link.
# -g option is for debugging version
# -O3 option is for optimized version
OPTFLAGS = -O3

all	: Floorplan.exe
	@echo -n ""

# optimized version
Floorplan.exe 	: chip.o B_Tree.o floorplanner.o main.o
				$(CC) $(OPTFLAGS) chip.o B_Tree.o floorplanner.o main.o -o Floorplan.exe
main.o 		: src/main.cpp
			$(CC) $(CFLAGS) $< -o $@
chip.o		: src/chip.cpp src/chip.h
			$(CC) $(CFLAGS) $(OPTFLAGS) $< -o $@
B_Tree.o	: src/B_Tree.cpp src/B_Tree.h
			$(CC) $(CFLAGS) $(OPTFLAGS) $< -o $@
floorplanner.o	: src/floorplanner.cpp src/floorplanner.h
				$(CC) $(CFLAGS) $(OPTFLAGS) $< -o $@


# clean all the .o and executable files
clean:
		rm -rf *.o *.gprof gmon.out lib/*.a lib/*.o bin/* Floorplan.exe

