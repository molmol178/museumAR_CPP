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

  // 時間計算のための周波数
  double f = 1000.0 / cv::getTickFrequency();
  int64 time_all = cv::getTickCount(); // 時間計測 Start

  int64 time_s; //スタート時間
  double time_detect; // 検出エンド時間
  double time_match; // マッチングエンド時間
  double time_alls;

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
  time_s = cv::getTickCount(); // 時間計測 Start
  template_featurepoint = featureDetection(patch_size, template_img);
  time_detect = (cv::getTickCount() - time_s)*f; // 時間計測 Stop

  input_featurepoint = featureDetection(patch_size, input_img);


  cout << "write featurepoint" << endl;
  string writeFeatureImage_fp = "template_img_out/template_detect_feature_point.tiff";
  writeFeaturePoint(template_img, template_featurepoint, writeFeatureImage_fp);
  string writtenFeatureImage_fp = "input_img_out/input_detect_feature_point.tiff";
  writeFeaturePoint(input_img, input_featurepoint, writtenFeatureImage_fp);

  cout << "calib input img" << endl;
  calib_input_featurepoint(input_centroids, template_centroids, input_featurepoint, template_featurepoint, input_img ,template_img);

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
  time_s = cv::getTickCount(); // 時間計測 Start
  featureMatching(template_img, input_img, template_keypoint_binary, input_keypoint_binary, template_featurepoint, input_featurepoint);
  time_match = (cv::getTickCount() - time_s)*f; // 時間計測 Stop



  time_alls = (cv::getTickCount() - time_all)*f; // 時間計測 Stop


  cout << "detect time =" << time_detect << " ms" <<endl;
  cout << "matching time = " << time_match << " ms" << endl;
  cout << "all processing time = " << time_alls << " ms" << endl;

  return 0;
}
