CC := g++


all: clean template input


template: template_topology_feature.cpp
	g++ -I/usr/local/Cellar/opencv3/3.0.0/include -L/usr/local/Cellar/opencv3/3.0.0/lib -lopencv_highgui -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_calib3d template_topology_feature.cpp -o template


input: input_topology_feature.cpp
	g++ -I/usr/local/Cellar/opencv3/3.0.0/include -L/usr/local/Cellar/opencv3/3.0.0/lib -lopencv_highgui -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_calib3d input_topology_feature.cpp -o input

debug_template: template_topology_feature.cpp
	g++ -I/usr/local/Cellar/opencv3/3.0.0/include -L/usr/local/Cellar/opencv3/3.0.0/lib -lopencv_highgui -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_calib3d template_topology_feature.cpp -o template -g -O0


debug_input: input_topology_feature.cpp
	g++ -I/usr/local/Cellar/opencv3/3.0.0/include -L/usr/local/Cellar/opencv3/3.0.0/lib -lopencv_highgui -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_calib3d input_topology_feature.cpp -o input -g -O0


clean:
	rm -f *.o template input


