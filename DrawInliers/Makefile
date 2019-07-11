
CC=g++
CFLAGS=`pkg-config --cflags --libs opencv`

DrawInliers:DrawInliers.cpp
	$(CC)  -o $@ $< $(CFLAGS)
	
clean:
	rm -f *.o core *~ DessinerInliers
	
