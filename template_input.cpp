#define _USE_MATH_DEFINES
#include <iostream>
#include <math.h>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "topology_feature.cpp"
#include <time.h>

using namespace std;
using namespace cv;
using namespace TopologyFeature;

int main(int argc, char** argv){
  clock_t start = clock(); //処理時間計測開始

  Mat input_img;
  Mat template_img;

  int patch_size;
  int tmp_range;
 
  input_img = imread(argv[1], IMREAD_GRAYSCALE);
  template_img = imread(argv[2], IMREAD_GRAYSCALE);
  patch_size = 7;
  tmp_range = 20;


  cout << "calc centroids" << endl;
  vector<Centroids> input_centroids;
  vector<Centroids> template_centroids;
  string input_centroids_path = "input_img_out/input_centroids.tiff";
  string template_centroids_path = "template_img_out/template_centroids.tiff";
  input_centroids = calcCentroids(input_centroids_path, input_img);
  template_centroids = calcCentroids(template_centroids_path, template_img);


  cout << "feature detection" << endl;
  vector<Featurepoints> template_featurepoint;
  vector<Featurepoints> input_featurepoint;
  template_featurepoint = featureDetection(patch_size, template_img);
  input_featurepoint = featureDetection(patch_size, input_img);

  string writeFeatureImage_fp = "template_img_out/template_detect_feature_point.tiff";
  writeFeaturePoint(template_img, template_featurepoint, writeFeatureImage_fp);

  string writtenFeatureImage_fp = "input_img_out/input_detect_feature_point.tiff";
  writeFeaturePoint(input_img, input_featurepoint, writtenFeatureImage_fp);

  calib_input_featurepoint(input_centroids, input_featurepoint, input_img);
  calib_input_featurepoint(input_centroids, template_featurepoint, input_img);

  cout << "feature description" << endl;
  vector<vector<int> > template_keypoint_binary;
  vector<vector<int> > input_keypoint_binary;

  input_keypoint_binary = featureDescription(input_featurepoint, input_img);
  template_keypoint_binary = featureDescription(template_featurepoint, template_img);
  //string key_bin_path = "template_img_out/template_keypoint_binary.csv";
  //twod_intCsvWriter(template_keypoint_binary, key_bin_path);

  //string key_bin_path = "input_img_out/input_keypoint_binary.csv";
  //twod_intCsvWriter(input_keypoint_binary, key_bin_path);

  cout << "feature matching " << endl;
  featureMatching(template_img, input_img, template_keypoint_binary, input_keypoint_binary, template_featurepoint, input_featurepoint);

  clock_t end = clock(); //処理時間計測終了
  cout << "duration = " << (double)(end - start) / CLOCKS_PER_SEC << "sec.\n";


  return 0;
}
