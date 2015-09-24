CC=gcc-5
HEAT_FLAGS=-Wall -fopenmp -I/usr/X11R6/include -L/usr/X11R6/lib -lX11

heat:
	$(CC) $(HEAT_FLAGS) heatdistribution.c

clean:
	rm *.out *.o
