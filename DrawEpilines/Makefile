
CC=g++
CFLAGS=`pkg-config --cflags --libs opencv`

DrawEpilines:DrawEpilines.cpp
	$(CC)  -o $@ $< $(CFLAGS)
	
clean:
	rm -f *.o core *~ DrawEpilines
	
