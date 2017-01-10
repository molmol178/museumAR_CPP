CC := g++


all: comparison

comparison: comparison.cpp
	g++ -I/usr/local/Cellar/opencv/2.4.12_2/include -L/usr/local/Cellar/opencv/2.4.12_2/lib `pkg-config --cflags opencv` `pkg-config --libs opencv` comparison.cpp -o comparison

clean:
	rm -f *.o comparison
