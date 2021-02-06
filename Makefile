all:
	gcc -g -fprofile-arcs -ftest-coverage src/hw.c -o a.out

clean:
	rm -rf *.gcda *.gcno core*