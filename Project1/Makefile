shell: myshell.o builtin.o argparse.o
	gcc -o shell myshell.o builtin.o argparse.o

myshell.o: myshell.c builtin.h argparse.h
	gcc -c myshell.c

builtin.o: builtin.c builtin.h
	gcc -c builtin.c

argparse.o: argparse.c argparse.h
	gcc -c argparse.c

clean:
	rm -f *.o *.bak *~*
