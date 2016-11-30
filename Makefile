CC := g++


all: template input

topology: topology_feature.cpp
	g++ -I/usr/local/Cellar/opencv/2.4.12_2/include -L/usr/local/Cellar/opencv/2.4.12_2/lib `pkg-config --cflags opencv` `pkg-config --libs opencv` topology_feature.cpp -o topology

template: template_topology_feature.cpp
	g++ -I/usr/local/Cellar/opencv/2.4.12_2/include -L/usr/local/Cellar/opencv/2.4.12_2/lib `pkg-config --cflags opencv` `pkg-config --libs opencv` template_topology_feature.cpp -o template


input: input_topology_feature.cpp
	g++ -I/usr/local/Cellar/opencv/2.4.12_2/include -L/usr/local/Cellar/opencv/2.4.12_2/lib `pkg-config --cflags opencv` `pkg-config --libs opencv` input_topology_feature.cpp -o input

debug_template: template_topology_feature.cpp
	g++ -I/usr/local/Cellar/opencv/2.4.12_2/include -L/usr/local/Cellar/opencv/2.4.12_2/lib `pkg-config --cflags opencv` `pkg-config --libs opencv` template_topology_feature.cpp -o template -g -O0


debug_input: input_topology_feature.cpp
	g++ -I/usr/local/Cellar/opencv/2.4.12_2/include -L/usr/local/Cellar/opencv/2.4.12_2/lib `pkg-config --cflags opencv` `pkg-config --libs opencv` input_topology_feature.cpp -o input -g -O0



clean:
	rm -f *.o template input


