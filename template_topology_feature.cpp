#define _USE_MATH_DEFINES
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include <vector>
#include <fstream>
#include "topology_feature.cpp"
#include <time.h>

using namespace std;
using namespace cv;

int main(int argc, char** argv){
  clock_t start = clock(); //処理時間計測開始

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
    //template_img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    template_img = imread(argv[1], 0);
    patch_size = atoi(argv[2]);
    cout << "patch_size = "<<patch_size <<endl;
    cout << "please set argv[3] about tmp_range. defalt is 40" << endl;
    tmp_range = 20;
  }else if(argc == 4){
    //template_img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    template_img = imread(argv[1], 0);
    patch_size = atoi(argv[2]);
    cout << "patch_size = "<<patch_size <<endl;
    tmp_range = atoi(argv[3]);
    cout << "tmp_range = " << tmp_range << endl;
  }else if(argc == 1){
    cout << "please set your template img." << endl;
    return 0;
  }else{
    //template_img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    template_img = imread(argv[1], 0);
    cout << "please set argv[2] about patch size. defalt is 7" <<endl;
    patch_size = 7;
    cout << "please set argv[3] about tmp_range. defalt is 40" << endl;
    tmp_range = 20;
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
/*
  GaussianBlur(template_img, Gaussian_template, Size(7,7), 1.5, 1.5);
  //cvtColor(Gaussian_template, template_hsv, CV_BGR2HSV);
  cvtColor(Gaussian_template, template_hsv, CV_BGR2RGB);
  imwrite("forslides/gaussian.tiff", Gaussian_template);

//splitRegion用-----------------------------------------------------------------------
  int template_x = template_hsv.cols;
  int template_y = template_hsv.rows;
  vector<Mat> planes;
  Mat v_value = Mat(template_y, template_x, CV_8UC1);
  vector<int> v_count_list(255, 0);

  split(template_hsv, planes);
  v_value = planes[2];

  //equalizeHist(v_value, v_value);
  for(int y = 0; y < template_y; y++ ){
    for(int x = 0; x < template_x; x++){
      int v = v_value.at<unsigned char>(y,x);
      v_count_list[v] += 1;
    }
  }

  string v_count_path = "template_img_out/template_v_count_list.csv";
  tf.oned_intCsvWriter(v_count_list, v_count_path);

  //for gnuplot
  string v_path = "graphPlot/template_v_count_list.csv";
  tf.oned_vertical_intCsvWriter(v_count_list, v_path);
//ここまで--------------------------------------------------------------------------

  int flag = 0;
  label_template_img = tf.template_splitRegion(tmp_range, template_hsv, v_count_list, flag);

  imwrite("template_img_out/label_template_img.tiff", label_template_img);
  //imwrite("forslides/label_template_img.tiff", label_template_img);

  dst_data = tf.re_label(label_template_img);
  changed_label_img = tf.writeDstData(dst_data);
  //imwrite("forslides/dst_data.tiff", changed_label_img);

  clean_label_img = tf.cleanLabelImage(dst_data, patch_size);
  changed_label_img = tf.writeDstData(clean_label_img);

  last_label_img = tf.remapLabel(clean_label_img);
  last_changed_label_img = tf.writeDstData(last_label_img);

  imwrite("template_img_out/template_clean_label_img.tiff", clean_label_img);
  imwrite("template_img_out/template_changed_label_img.tiff", changed_label_img);
  imwrite("template_img_out/template_last_changed_label_img.tiff", last_changed_label_img);

  Mat UC_last_label_img = Mat(template_y, template_x, CV_8UC1);
  int label_value;
  for(int y = 0; y < template_y; y++){
    for(int x = 0; x < template_x; x++){
      label_value = last_label_img.at<unsigned short>(y, x);
      UC_last_label_img.at<unsigned char>(y,x) = label_value * 10;
    }
  }
  imwrite("template_img_out/template_last_label_img.tiff", UC_last_label_img);


  cout << "end label_img cleaning" << endl;
*/
  cout << "feature detection" << endl;
  //sum_min_label_word = tf.featureDetection(patch_size,last_label_img, &sum_one_dimention_scanning, &sum_xy, &sum_boundary, &sum_ave_keypoint, &sum_mean_vector);
  sum_min_label_word = tf.featureDetection(patch_size,template_img, &sum_one_dimention_scanning, &sum_xy, &sum_boundary, &sum_ave_keypoint, &sum_mean_vector);



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

  cout << "write feature points..." << endl;
  string writeFeatureImage_fp = "template_img_out/template_detect_feature_point.tiff";
  tf.writeFeaturePoint(template_img, &sum_xy, &sum_boundary, writeFeatureImage_fp);

  cout << "description..." << endl;
  //keypoint_binary = tf.featureDescription(&sum_one_dimention_scanning, last_label_img);
  keypoint_binary = tf.featureDescription(&sum_one_dimention_scanning, template_img);



  clock_t end = clock(); //処理時間計測終了
  cout << "duration = " << (double)(end - start) / CLOCKS_PER_SEC << "sec.\n";

  cout << "CSV output..." << endl;

  string key_bin_path = "template_img_out/template_keypoint_binary.csv";
  tf.twod_intCsvWriter(keypoint_binary, key_bin_path);

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

  return 0;
}
