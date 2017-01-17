CC := g++


all: comparison MSER MSER3 inlier

comparison: comparison.cpp
	g++ -I/usr/local/Cellar/opencv/2.4.12_2/include -L/usr/local/Cellar/opencv/2.4.12_2/lib `pkg-config --cflags opencv` `pkg-config --libs opencv` comparison.cpp -o comparison
MSER: comparison_MSER.cpp
	g++ -I/usr/local/Cellar/opencv/2.4.12_2/include -L/usr/local/Cellar/opencv/2.4.12_2/lib `pkg-config --cflags opencv` `pkg-config --libs opencv` comparison_MSER.cpp -o comparison_MSER

MSER3: comparison_mser_3.cpp
	g++ -I/usr/local/Cellar/opencv3/3.2.0/include -L/usr/local/Cellar/opencv3/3.2.0/lib -lopencv_highgui -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_calib3d -lopencv_xfeatures2d -lopencv_features2d comparison_mser_3.cpp -o comparison_mser_3

inlier: calc_inlier.cpp
	g++ -I/usr/local/Cellar/opencv3/3.2.0/include -L/usr/local/Cellar/opencv3/3.2.0/lib -lopencv_highgui -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_calib3d -lopencv_xfeatures2d -lopencv_features2d calc_inlier.cpp -o calc_inlier



clean:
	rm -f *.o comparison
