#define _USE_MATH_DEFINES
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "topology_feature.cpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv){
  TopologyFeature tf;

  Mat Gaussian_input;
  Mat input_hsv;
  Mat label_input_img;
  Mat dst_data;
  Mat clean_label_img;
  Mat changed_label_img;
  Mat input_img;
  Mat template_img;
  Mat last_label_img;



 //コマンドライン引数
  int patch_size;
  int tmp_range;
  if(argc == 3){
    input_img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    template_img = imread(argv[2], CV_LOAD_IMAGE_COLOR);
    cout << "please set argv[3] about patch size. defalt is 7" <<endl;
    patch_size = 7;
    cout << "patch_size = "<<patch_size <<endl;
    cout << "please set argv[4] about tmp_range. defalt is 30" << endl;
    tmp_range = 40;
  }else if(argc == 4){
    input_img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    template_img = imread(argv[2], CV_LOAD_IMAGE_COLOR);
    patch_size = atoi(argv[3]);
    cout << "patch_size = "<< patch_size <<endl;
    cout << "please set argv[4] about tmp_range. defalt is 30" << endl;
    tmp_range = 40;
  }else if(argc == 5){
    input_img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    template_img = imread(argv[2], CV_LOAD_IMAGE_COLOR);
    patch_size = atoi(argv[3]);
    cout << "patch_size = "<<patch_size <<endl;
    tmp_range = atoi(argv[4]);
    cout << "tmp_range = " << tmp_range << endl;
  }else if(argc == 1 || argc == 2){
    cout << "please set your input img at argv[1] and template imgat argv[2]" << endl;
    return 0;
  }


//featureDetection variables
  //vector<vector<int> > sum_one_dimention_scanning;
  vector<vector<int> > sum_label_one_dimention_scanning;
  vector<vector<int> > sum_xy;
  vector<vector<int> > sum_boundary;
  vector<vector<double> > sum_ave_keypoint;

  vector<vector<int> > keypoint_binary;
  vector<vector<int> > sum_min_label_word;
  vector<vector<int> > sum_mean_vector;


  GaussianBlur(input_img, Gaussian_input, Size(7, 7), 1.5, 1.5);
  //cvtColor(Gaussian_input, input_hsv, CV_BGR2HSV);
  cvtColor(Gaussian_input, input_hsv, CV_BGR2RGB);



  label_input_img = tf.inputCreateLabelImg(input_hsv);
  //label_input_img = tf.template_splitRegion(tmp_range, input_hsv);
  imwrite("input_img_out/label_input_img.tiff", label_input_img);

  dst_data = tf.re_label(label_input_img);
  //unsigned short画像の領域をunsigned charのランダムな値で埋める
  //changed_label_img = tf.writeDstData(dst_data);


  clean_label_img = tf.cleanLabelImage(dst_data, patch_size);
  imwrite("input_img_out/clean_label_img_input.tiff", clean_label_img);

  cout << "test!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;

  //unsigned short画像の領域をunsigned charのランダムな値で埋める
  //ラベル画像確認用
  changed_label_img = tf.writeDstData(clean_label_img);
  imwrite("input_img_out/changed_label_img_input.tiff", changed_label_img);

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
  sum_min_label_word = tf.featureDetection(patch_size, label_input_img,clean_label_img, &sum_label_one_dimention_scanning, &sum_xy, &sum_boundary, &sum_ave_keypoint, &sum_mean_vector);
  //sum_min_label_word = tf.featureDetection(patch_size, label_input_img,clean_label_img, &sum_one_dimention_scanning, &sum_xy, &sum_boundary, &sum_ave_keypoint);

  /*
  string sum_one_d_scan_path = "input_img_out/input_sum_one_dimention_scanning.csv";
  tf.twod_intCsvWriter(sum_one_dimention_scanning, sum_one_d_scan_path);
  */

  string sum_one_d_scan_path = "input_img_out/input_sum_label_one_dimention_scanning.csv";
  tf.twod_intCsvWriter(sum_label_one_dimention_scanning, sum_one_d_scan_path);

  string sum_xy_path = "input_img_out/input_yx.csv";
  tf.twod_intCsvWriter(sum_xy, sum_xy_path);

  string sum_boundary_path = "input_img_out/input_sum_boundary.csv";
  tf.twod_intCsvWriter(sum_boundary, sum_boundary_path);

  string sum_ave_path = "input_img_out/input_sum_ave_keypoint.csv";
  tf.twod_doubleCsvWriter(sum_ave_keypoint, sum_ave_path);

  string sum_min_path = "input_img_out/input_sum_min_label_word.csv";
  tf.twod_intCsvWriter(sum_min_label_word, sum_min_path);

  string sum_mean_vector_path = "input_img_out/input_sum_mean_vector.csv";
  tf.twod_intCsvWriter(sum_mean_vector, sum_mean_vector_path);

  input_img = tf.writeFeaturePoint(input_img, &sum_xy, &sum_boundary);
  imwrite("input_img_out/input_detect_feature_point.tiff", input_img);

  keypoint_binary = tf.featureDescription(&sum_label_one_dimention_scanning, label_input_img);
  //keypoint_binary = tf.featureDescription(&sum_one_dimention_scanning, clean_label_img);

  string key_bin_path = "input_img_out/input_keypoint_binary.csv";
  tf.twod_intCsvWriter(keypoint_binary, key_bin_path);

  tf.featureMatching(input_img, template_img, keypoint_binary, sum_label_one_dimention_scanning, sum_xy, sum_boundary, sum_ave_keypoint, sum_min_label_word, sum_mean_vector);
  //tf.featureMatching(input_img, template_img, keypoint_binary, sum_one_dimention_scanning, sum_xy, sum_boundary, sum_ave_keypoint, sum_min_label_word);

  tf.graphPlot();
  return 0;
}
