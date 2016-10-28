all: solver

solver: main.o sudoku.o
	g++ main.o sudoku.o -o solver

main.o: main.cpp
	g++ -c main.cpp

sudoku.o: sudoku.cpp sudoku.h
	g++ -c sudoku.cpp

clean:
	$(RM) *.o
