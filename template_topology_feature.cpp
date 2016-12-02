#define _USE_MATH_DEFINES
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include <vector>
#include <fstream>
#include "topology_feature.cpp"
using namespace std;
using namespace cv;

int main(int argc, char** argv){
  TopologyFeature tf;
  Mat Gaussian_template;
  Mat template_hsv;
  Mat label_template_img;
  Mat dst_data;
  Mat clean_label_img;
  Mat changed_label_img;
  Mat template_img;
  Mat last_label_img;
  Mat last_changed_label_img;

 //コマンドライン引数
  int patch_size;
  int tmp_range;
  if(argc == 3){
    template_img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    patch_size = atoi(argv[2]);
    cout << "patch_size = "<<patch_size <<endl;
    cout << "please set argv[3] about tmp_range. defalt is 30" << endl;
    tmp_range = 40;
  }else if(argc == 4){
    template_img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    patch_size = atoi(argv[2]);
    cout << "patch_size = "<<patch_size <<endl;
    tmp_range = atoi(argv[3]);
    cout << "tmp_range = " << tmp_range << endl;
  }else if(argc == 1){
    cout << "please set your template img." << endl;
    return 0;
  }else{
    template_img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    cout << "please set argv[2] about patch size. defalt is 7" <<endl;
    patch_size = 7;
    cout << "please set argv[3] about tmp_range. defalt is 30" << endl;
    tmp_range = 40;
  }

//featureDetection variables
  //vector<vector<int> > sum_label_one_dimention_scanning;
  vector<vector<int> > sum_one_dimention_scanning;
  vector<vector<float> > sum_xy;
  vector<vector<int> > sum_boundary;
  vector<vector<double> > sum_ave_keypoint;

  vector<vector<int> > keypoint_binary;
  vector<vector<int> > sum_min_label_word;
  vector<vector<int> > sum_mean_vector;

  GaussianBlur(template_img, Gaussian_template, Size(7,7), 1.5, 1.5);
  //cvtColor(Gaussian_template, template_hsv, CV_BGR2HSV);
  cvtColor(Gaussian_template, template_hsv, CV_BGR2RGB);
  imwrite("forslides/gaussian.tiff", Gaussian_template);



  label_template_img = tf.template_splitRegion(tmp_range, Gaussian_template);


  imwrite("template_img_out/label_template_img.tiff", label_template_img);
  imwrite("forslides/label_template_img.tiff", label_template_img);



  dst_data = tf.re_label(label_template_img);
  changed_label_img = tf.writeDstData(dst_data);
  imwrite("forslides/dst_data.tiff", changed_label_img);



  clean_label_img = tf.cleanLabelImage(dst_data, patch_size);
  changed_label_img = tf.writeDstData(clean_label_img);

  last_label_img = tf.remapLabel(clean_label_img);
  last_changed_label_img = tf.writeDstData(last_label_img);

  ofstream test("template_test.csv");
  test <<last_label_img;
  test << endl;

  imwrite("template_img_out/template_clean_label_img.tiff", clean_label_img);
  imwrite("template_img_out/template_changed_label_img.tiff", changed_label_img);
  imwrite("template_img_out/template_last_changed_label_img.tiff", last_changed_label_img);



  //ofstream ofs("template_img_out/test_img.csv");
  //ofs << test_dst_data;


  cout << "end label_img cleaning" << endl;

 /*
  int input_y = clean_label_img.rows;
  int input_x = clean_label_img.cols;

  double maxVal;
  minMaxLoc(clean_label_img, NULL, &maxVal);
  vector<unsigned short> clean_label_hist(maxVal, 0);

  cout << "clean_label max = " << maxVal << endl;;
  for(int y = 0; y < input_y; y++ ){
    for (int x = 0; x < input_x; x++){
      int v = clean_label_img.at<unsigned short>(y,x);
      clean_label_hist[v] += 1;
    }
  }
  int i  =0;
  for(int v = 0; v < clean_label_hist.size(); v++){
    cout << clean_label_hist[v] << ", ";
    if(clean_label_hist[v] == 0){
      i++;
    }
  }
  cout << endl;
  cout << "0 hist is = " << i << endl;
  */


  cout << "feature detection" << endl;
  sum_min_label_word = tf.featureDetection(patch_size,last_label_img, &sum_one_dimention_scanning, &sum_xy, &sum_boundary, &sum_ave_keypoint, &sum_mean_vector);

  /*
  cout << "===============================================================" << endl;
  for (int i = 0; i < sum_label_one_dimention_scanning.size(); i++){
    cout << "sum_one_dimention_sacnning = ";
    for (int j = 0; j < sum_label_one_dimention_scanning[0].size(); j++){
      cout << sum_label_one_dimention_scanning[i][j] << ", ";
    }
    cout << endl;
    cout << "sum_xy = ";
    for (int a = 0; a < sum_xy[0].size(); a++){
      cout << sum_xy[i][a] << ", ";
    }
    cout << endl;
    for (int a = 0; a < sum_boundary[0].size(); a++){
      cout << "sum_boundary = " << sum_boundary[i][a]  << ", ";
    }
    cout << endl;
    for (int a = 0; a < sum_ave_keypoint[0].size(); a++){
      cout << "sum_ave_keypoint = " << sum_ave_keypoint[i][a] << ", ";
    }
    cout << endl;
    cout << "============================================" << endl;
  }
  cout << "finish feature detection" << endl;
  */

  string template_sum_label_d_path = "template_img_out/template_sum_one_dimention_scanning.csv";
  tf.twod_intCsvWriter(sum_one_dimention_scanning, template_sum_label_d_path);

  string template_sum_xy_path = "template_img_out/template_yx.csv";
  tf.twod_floatCsvWriter(sum_xy, template_sum_xy_path);

  string sum_boundary_path = "template_img_out/template_sum_boundary.csv";
  tf.twod_intCsvWriter(sum_boundary, sum_boundary_path);

  string sum_ave_path = "template_img_out/template_sum_ave_keypoint.csv";
  tf.twod_doubleCsvWriter(sum_ave_keypoint, sum_ave_path);

  string sum_min_path = "template_img_out/template_sum_min_label_word.csv";
  tf.twod_intCsvWriter(sum_min_label_word, sum_min_path);

  string sum_mean_vector_path = "template_img_out/template_sum_mean_vector.csv";
  tf.twod_intCsvWriter(sum_mean_vector, sum_mean_vector_path);

  template_img = tf.writeFeaturePoint(template_img, &sum_xy, &sum_boundary);
  imwrite("template_img_out/template_detect_feature_point.tiff", template_img);

  keypoint_binary = tf.featureDescription(&sum_one_dimention_scanning, last_label_img);

  string key_bin_path = "template_img_out/template_keypoint_binary.csv";
  tf.twod_intCsvWriter(keypoint_binary, key_bin_path);

  return 0;
}
