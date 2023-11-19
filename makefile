OBJECTS = lab3.o main.o

validate_sudoku: $(OBJECTS)
	gcc -o $@ $(OBJECTS) -pthread

lab3.o: lab3.c 
	gcc -c lab3.c -pthread

main.o: main.c 
	gcc -c main.c -pthread

clean:
	-rm *.o core validate_sudoku
