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
  //Mat numbered_dst_data;

 //コマンドライン引数
  int patch_size;
  int tmp_range;
  if(argc == 3){
    template_img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    patch_size = atoi(argv[2]);
    cout << "patch_size = "<<patch_size <<endl;
    cout << "please set argv[3] about tmp_range. defalt is 30" << endl;
    tmp_range = 30;
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
    tmp_range = 30;
  }
//featureDetection variables
  vector<vector<int> > sum_label_one_dimention_scanning;
  //vector<vector<int> > sum_one_dimention_scanning;
  vector<vector<int> > sum_xy;
  vector<vector<int> > sum_boundary;
  vector<vector<double> > sum_ave_keypoint;

  vector<vector<int> > keypoint_binary;
  vector<vector<int> > sum_min_label_word;

  //ガウシアンブラー
  GaussianBlur(template_img, Gaussian_template, Size(7,7), 3, 3);
  cvtColor(Gaussian_template, template_hsv, CV_BGR2HSV);
  //cvtColor(template_img, template_hsv, CV_BGR2HSV);

  label_template_img = tf.template_splitRegion(tmp_range, template_hsv);
  imwrite("template_img_out/label_template_img.tiff", label_template_img);

  dst_data = tf.re_label(label_template_img);
  //unsigned short画像の領域をunsigned charのランダムな値で埋める
  //changed_label_img = TopologyFeature::writeDstData(dst_data);

  clean_label_img = tf.cleanLabelImage(dst_data, patch_size);
  //ofstream of_test("template_img_out/clean_label_img.csv");
  //of_test << clean_label_img << endl;
  changed_label_img = tf.writeDstData(clean_label_img);
  imwrite("template_img_out/clean_label_img_input.tiff", clean_label_img);

  //numbered_dst_data = tf.re_label(changed_label_img);
  //ofstream of_two("template_img_out/numbered_dst_data.csv");
  //of_two << numbered_dst_data << endl;
  //numbered_dst_data = tf.writeDstData(numbered_dst_data);
  //imwrite("template_img_out/numbered_label_img.tiff", numbered_dst_data);



  //numbered_dst_data = tf.re_label(clean_label_img);

  cout << "end label_img cleaning" << endl;


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


  //unsigned short画像の領域をunsigned charのランダムな値で埋める
  //ラベル画像確認用
  changed_label_img = tf.writeDstData(clean_label_img);
  imwrite("template_img_out/changed_label_img.tiff", changed_label_img);

  cout << "feature detection" << endl;
  sum_min_label_word = tf.featureDetection(patch_size, label_template_img,clean_label_img, &sum_label_one_dimention_scanning, &sum_xy, &sum_boundary, &sum_ave_keypoint);
  //sum_min_label_word = tf.featureDetection(patch_size, label_template_img,clean_label_img, &sum_one_dimention_scanning, &sum_xy, &sum_boundary, &sum_ave_keypoint);

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

  ofstream ofs_sum_one_d_scan("template_img_out/template_sum_label_one_dimention_scanning.csv");
  for(int i = 0; i < sum_label_one_dimention_scanning.size(); i++){
    for(int j = 0; j < sum_label_one_dimention_scanning[0].size(); j ++){
      if(j == sum_label_one_dimention_scanning[0].size()-1){
        ofs_sum_one_d_scan << sum_label_one_dimention_scanning[i][j];
      }else{
        ofs_sum_one_d_scan << sum_label_one_dimention_scanning[i][j] << ",";
      }
    }
    ofs_sum_one_d_scan << endl;
  }
  ofs_sum_one_d_scan << endl;
  /*
  ofstream ofs_sum_one_d_scan("template_img_out/template_sum_one_dimention_scanning.csv");
  for(int i = 0; i < sum_one_dimention_scanning.size(); i++){
    for(int j = 0; j < sum_one_dimention_scanning[0].size(); j ++){
      if(j == sum_one_dimention_scanning[0].size()-1){
        ofs_sum_one_d_scan << sum_one_dimention_scanning[i][j];
      }else{
        ofs_sum_one_d_scan << sum_one_dimention_scanning[i][j] << ",";
      }
    }
    ofs_sum_one_d_scan << endl;
  }
  ofs_sum_one_d_scan << endl;
  */


  ofstream ofs_sum_xy("template_img_out/template_yx.csv");
  for(int i = 0; i < sum_xy.size(); i++){
    for(int j = 0; j < sum_xy[0].size(); j ++){
      if(j == sum_xy[0].size() - 1){
        ofs_sum_xy << sum_xy[i][j];
      }else{
        ofs_sum_xy << sum_xy[i][j] << ",";
      }
    }
    ofs_sum_xy << endl;
  }
  ofs_sum_xy << endl;

  ofstream ofs_sum_boundary("template_img_out/template_sum_boundary.csv");
  for(int i = 0; i < sum_boundary.size(); i++){
    for(int j = 0; j < sum_boundary[0].size(); j ++){
      if(j == sum_boundary[0].size() -1){
        ofs_sum_boundary << sum_boundary[i][j] ;
      }else{
        ofs_sum_boundary << sum_boundary[i][j] << ",";
      }
    }
    ofs_sum_boundary << endl;
  }
  ofs_sum_boundary << endl;

  ofstream ofs_sum_ave("template_img_out/template_sum_ave_keypoint.csv");
  for(int i = 0; i < sum_ave_keypoint.size(); i++){
    for(int j = 0; j < sum_ave_keypoint[0].size(); j ++){
      if(j == sum_ave_keypoint[0].size()-1){
        ofs_sum_ave << sum_ave_keypoint[i][j] ;
      }else{
        ofs_sum_ave << sum_ave_keypoint[i][j] << ",";
      }
    }
    ofs_sum_ave << endl;
  }
  ofs_sum_ave << endl;

  ofstream ofs_sum_min("template_img_out/template_sum_min_label_word.csv");
  for(int i = 0; i < sum_min_label_word.size(); i++){
    for(int j = 0; j < sum_min_label_word[0].size(); j ++){
      if(j == sum_min_label_word[0].size()-1){
        ofs_sum_min << sum_min_label_word[i][j] ;
      }else{
        ofs_sum_min << sum_min_label_word[i][j] << ",";
      }
    }
    ofs_sum_min << endl;
  }
  ofs_sum_min << endl;


  template_img = tf.writeFeaturePoint(template_img, &sum_xy, &sum_boundary);
  imwrite("template_img_out/template_detect_feature_point.tiff", template_img);

  keypoint_binary = tf.featureDescription(&sum_label_one_dimention_scanning, label_template_img);
  //keypoint_binary = tf.featureDescription(&sum_one_dimention_scanning, clean_label_img);

  ofstream ofs_key_bin("template_img_out/template_keypoint_binary.csv");
  for(int i = 0; i < keypoint_binary.size(); i++){
    for(int j = 0; j < keypoint_binary[0].size(); j ++){
      if(j == keypoint_binary[0].size() - 1){
        ofs_key_bin << keypoint_binary[i][j];
      }else{
        ofs_key_bin << keypoint_binary[i][j] << ",";
      }
    }
    ofs_key_bin << endl;
  }
  ofs_key_bin << endl;

  return 0;
}
