CC := g++


all: clean templateinput create_label

templateinput: template_input.cpp
	g++ -I/usr/local/Cellar/opencv3/3.2.0/include -L/usr/local/Cellar/opencv3/3.2.0/lib -lopencv_highgui -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_calib3d template_input.cpp -o templateinput

create_label: create_label_img.cpp
	g++ -I/usr/local/Cellar/opencv3/3.2.0/include -L/usr/local/Cellar/opencv3/3.2.0/lib -lopencv_highgui -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_calib3d create_label_img.cpp -o create_label


topology: topology_feature.cpp
	g++ -I/usr/local/Cellar/opencv3/3.2.0/include -L/usr/local/Cellar/opencv3/3.2.0/lib -lopencv_highgui -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_calib3d topology_feature.cpp -o topology



template: template_topology_feature.cpp
	g++ -I/usr/local/Cellar/opencv3/3.2.0/include -L/usr/local/Cellar/opencv3/3.2.0/lib -lopencv_highgui -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_calib3d template_topology_feature.cpp -o template


input: input_topology_feature.cpp
	g++ -I/usr/local/Cellar/opencv3/3.2.0/include -L/usr/local/Cellar/opencv3/3.2.0/lib -lopencv_highgui -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_calib3d input_topology_feature.cpp -o input

debug_template: template_topology_feature.cpp
	g++ -I/usr/local/Cellar/opencv3/3.2.0/include -L/usr/local/Cellar/opencv3/3.2.0/lib -lopencv_highgui -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_calib3d template_topology_feature.cpp -o template -g -O0


debug_input: input_topology_feature.cpp
	g++ -I/usr/local/Cellar/opencv3/3.2.0/include -L/usr/local/Cellar/opencv3/3.2.0/lib -lopencv_highgui -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_calib3d input_topology_feature.cpp -o input -g -O0


clean:
	rm -f *.o create_label templateinput


