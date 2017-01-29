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

  Mat test_input;
  Mat test_template;

  Mat input_img;
  Mat template_img;

  int patch_size;
  int tmp_range;

  test_template= imread(argv[1], IMREAD_GRAYSCALE);
  test_input= imread(argv[2], IMREAD_GRAYSCALE);
  patch_size = 7;
  tmp_range = 20;

  //手動でラベリングした画像に対してフォトショの色のおかしい問題を吸収
  input_img = correct_image(test_input);
  template_img = correct_image(test_template);


  clock_t all_s = clock();

  ////cout << "calc centroids" << endl;
  vector<Centroids> input_centroids;
  vector<Centroids> template_centroids;
  string input_centroids_path = "input_img_out/input_centroids.tiff";
  string template_centroids_path = "template_img_out/template_centroids.tiff";
  input_centroids = calcCentroids(input_centroids_path, input_img);
  template_centroids = calcCentroids(template_centroids_path, template_img);

  //cout << "feature detection" << endl;
  vector<Featurepoints> tmp_template_featurepoint;
  vector<Featurepoints> tmp_input_featurepoint;
  //cout << "template_feature detection" << endl;

  clock_t detect_start = clock();
  tmp_template_featurepoint = featureDetection(patch_size, template_img);
  clock_t detect_end = clock();

  //cout << "input_feature detection" << endl;
  tmp_input_featurepoint = featureDetection(patch_size, input_img);

  vector<Featurepoints> template_featurepoint;
  vector<Featurepoints> input_featurepoint;

  int marge_pt = 6;
  template_featurepoint =  marge_featurepoint(tmp_template_featurepoint, marge_pt);
  input_featurepoint =  marge_featurepoint(tmp_input_featurepoint, marge_pt);

  //cout << "write featurepoint" << endl;
  /*
  string writeFeatureImage_fp = "template_img_out/template_detect_feature_point.tiff";
  writeFeaturePoint(template_img, template_featurepoint, writeFeatureImage_fp);
  string writtenFeatureImage_fp = "input_img_out/input_detect_feature_point.tiff";
  writeFeaturePoint(input_img, input_featurepoint, writtenFeatureImage_fp);
  */
  //cout << "calib featurepoint" << endl;

  int min_value;

  min_value = calc_same_min(template_img, input_img);

  vector<Featurepoints> input_relative_featurepoint;
  vector<Featurepoints> template_relative_featurepoint;
  vector<Centroids> input_relative_centroids;
  vector<Centroids> template_relative_centroids;

  calib_featurepoint(input_centroids, input_featurepoint, input_img, &input_relative_centroids, &input_relative_featurepoint, min_value);
  calib_featurepoint(template_centroids, template_featurepoint,template_img, &template_relative_centroids, &template_relative_featurepoint, min_value);




  //cout << "feature description" << endl;
  vector<keypoint> template_keypoint_binary;
  vector<keypoint> input_keypoint_binary;

  //input_keypoint_binary = featureDescription(input_featurepoint, input_img);
  //template_keypoint_binary = featureDescription(template_featurepoint, template_img);
  //string key_bin_path = "template_img_out/template_keypoint_binary.csv";
  //twod_intCsvWriter(template_keypoint_binary, key_bin_path);

  //string key_bin_path = "input_img_out/input_keypoint_binary.csv";
  //twod_intCsvWriter(input_keypoint_binary, key_bin_path);

  vector<cent2feature> calced_template_vector;
  vector<cent2feature> calced_input_vector;
  clock_t desc_s = clock();
  calced_template_vector = calc_relative_centroids2featurepointVector(template_relative_centroids,template_relative_featurepoint);
  clock_t desc_e = clock();

  calced_input_vector = calc_relative_centroids2featurepointVector(input_relative_centroids,input_relative_featurepoint);



  cout << "feature matching " << endl;

  clock_t match_s = clock();
  featureMatching(template_img, input_img, template_keypoint_binary, input_keypoint_binary, calced_template_vector, calced_input_vector);
  clock_t match_e = clock();

  clock_t all_e =clock();
  cout << "keypoint count" << template_featurepoint.size() <<endl;

  cout << "," << (double)(detect_end - detect_start) / CLOCKS_PER_SEC * 1000 << "," << (double)(desc_e - desc_s) << "," << (double)(match_e - match_s) << "," << (double)(all_e - all_s) << endl;;

  //cout << "detect time =" << time_detect << " ms" <<endl;
  //cout << "description time =" << time_desc << " ms" <<endl;
  //cout << "matching time = " << time_match << " ms" << endl;
  //cout << "all processing time = " << time_alls << " ms" << endl;
  //cout << ","<< time_detect << "," << time_desc << "," << time_match << "," << time_alls << endl; 


  return 0;
}
