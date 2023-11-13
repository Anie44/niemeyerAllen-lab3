OBJECTS = lab3.o main.o

validate_sudoku: $(OBJECTS)
	gcc -o $@ $(OBJECTS) 

lab3.o:lab3.c
	gcc -c lab2.c

main.o: 
	gcc -c main.c

clean:
	-rm *.o core validate_sudoku