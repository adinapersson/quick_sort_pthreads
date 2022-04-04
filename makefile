quicksort: quicksort.c	
	gcc -Wall -O2 -o quicksort quicksort.c -lm -lpthread

clean:
	rm -f *.o
