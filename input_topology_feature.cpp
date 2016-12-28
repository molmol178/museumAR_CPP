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
#include <time.h>

using namespace std;
using namespace cv;

int main(int argc, char** argv){
  clock_t start = clock(); //処理時間計測開始
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
  Mat last_changed_label_img;

 //コマンドライン引数
  int patch_size;
  int tmp_range;
  /*
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
  */
  //input_img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  //template_img = imread(argv[2], CV_LOAD_IMAGE_COLOR);
  input_img = imread(argv[1], 0);
  template_img = imread(argv[2], 0);
  patch_size = 7;
  tmp_range = 20;

//featureDetection variables
  vector<vector<int> > sum_one_dimention_scanning;
  //vector<vector<int> > sum_label_one_dimention_scanning;
  vector<vector<float> > sum_xy;
  vector<vector<int> > sum_boundary;
  vector<vector<double> > sum_ave_keypoint;

  vector<vector<int> > keypoint_binary;
  vector<vector<int> > sum_min_label_word;
  vector<vector<int> > sum_mean_vector;

/*
  GaussianBlur(input_img, Gaussian_input, Size(7, 7), 1.5, 1.5);
  //cvtColor(Gaussian_input, input_hsv, CV_BGR2HSV);
  cvtColor(Gaussian_input, input_hsv, CV_BGR2RGB);

  label_input_img = tf.inputCreateLabelImg(input_hsv);
  imwrite("input_img_out/input_label_input_img.tiff", label_input_img);

  dst_data = tf.re_label(label_input_img);
  changed_label_img = tf.writeDstData(dst_data);

  clean_label_img = tf.cleanLabelImage(dst_data, patch_size);
  changed_label_img = tf.writeDstData(clean_label_img);

  last_label_img = tf.remapLabel(clean_label_img);
  last_changed_label_img = tf.writeDstData(last_label_img);

  imwrite("input_img_out/input_clean_label_img.tiff", clean_label_img);
  imwrite("input_img_out/input_changed_label_img.tiff", changed_label_img);
  imwrite("input_img_out/input_last_changed_label_img.tiff", last_changed_label_img);

  cout << "end label_img cleaning" << endl;

  Mat UC_last_label_img = Mat(input_img.rows, input_img.cols, CV_8UC1);
  int label_value;
  for(int y = 0; y < input_img.rows; y++){
    for(int x = 0; x < input_img.cols; x++){
      label_value = last_label_img.at<unsigned short>(y, x);
      UC_last_label_img.at<unsigned char>(y,x) = label_value * 10;
    }
  }
  imwrite("input_img_out/input_last_label_img.tiff", UC_last_label_img);

*/
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
  sum_min_label_word = tf.featureDetection(patch_size, input_img, &sum_one_dimention_scanning, &sum_xy, &sum_boundary, &sum_ave_keypoint, &sum_mean_vector);
  cout << "feature detection end " << endl;

/*
  string sum_one_d_scan_path = "input_img_out/input_sum_one_dimention_scanning.csv";
  tf.twod_intCsvWriter(sum_one_dimention_scanning, sum_one_d_scan_path);

  string sum_xy_path = "input_img_out/input_yx.csv";
  tf.twod_floatCsvWriter(sum_xy, sum_xy_path);

  string sum_boundary_path = "input_img_out/input_sum_boundary.csv";
  tf.twod_intCsvWriter(sum_boundary, sum_boundary_path);

  string sum_ave_path = "input_img_out/input_sum_ave_keypoint.csv";
  tf.twod_doubleCsvWriter(sum_ave_keypoint, sum_ave_path);

  string sum_min_path = "input_img_out/input_sum_min_label_word.csv";
  tf.twod_intCsvWriter(sum_min_label_word, sum_min_path);

  string sum_mean_vector_path = "input_img_out/input_sum_mean_vector.csv";
  tf.twod_intCsvWriter(sum_mean_vector, sum_mean_vector_path);
*/
  string writtenFeatureImage_fp = "input_img_out/input_detect_feature_point.tiff";
  tf.writeFeaturePoint(input_img, &sum_xy, &sum_boundary, writtenFeatureImage_fp);


  cout << "feature description" << endl;
  //keypoint_binary = tf.featureDescription(&sum_one_dimention_scanning,last_label_img);
  keypoint_binary = tf.featureDescription(&sum_one_dimention_scanning, input_img);

  string key_bin_path = "input_img_out/input_keypoint_binary.csv";
  tf.twod_intCsvWriter(keypoint_binary, key_bin_path);

  cout << "feature matching " << endl;
  tf.featureMatching(input_img, template_img, keypoint_binary, sum_one_dimention_scanning, sum_xy, sum_boundary, sum_ave_keypoint, sum_min_label_word, sum_mean_vector);


/* ---------------------------------------opencv description and matching---------------------------------------------
  KeyPoint input_key;
  vector<KeyPoint> input_keypoints;
  for(int y = 0; y < sum_xy.size(); y++){
      float xpt = sum_xy[y][1];
      float ypt = sum_xy[y][0];
      input_key.pt.x = xpt;
      input_key.pt.y = ypt;
      input_key.size = 7;
      input_key.angle = 10;
      //float xangle = sum_mean_vector[y][0];
      //float yangle = sum_mean_vector[y][1];
      //float angle=acos(xangle/sqrt(xangle*xangle+yangle*yangle));
      //angle=angle*180.0/M_PI;
      //if(yangle<0)angle=360.0-angle;
      //input_key.angle = angle;
      input_keypoints.push_back(input_key);
  }


  string  template_yx_path = "template_img_out/template_yx.csv";
  vector<vector<float> > template_xy;
  template_xy = tf.twod_floatCsvReader(template_yx_path);
  //string template_mean_vector_path = "template_img_out/template_sum_mean_vector.csv";
  //vector<vector<int> > template_mean_vector;
  //template_mean_vector = tf.twod_intCsvReader(template_mean_vector_path);

  KeyPoint template_key;
  vector<KeyPoint> template_keypoints;
  for(int y = 0; y < template_xy.size()-1; y++){
      float xpt = template_xy[y][1];
      float ypt = template_xy[y][0];
      template_key.pt.x = xpt;
      template_key.pt.y = ypt;
      template_key.size = 7;
      template_key.angle = 10;
      //float xangle = template_mean_vector[y][0];
      //float yangle = template_mean_vector[y][1];
      //float angle=acos(xangle/sqrt(xangle*xangle+yangle*yangle));
      //angle=angle*180.0/M_PI;
      //if(yangle<0)angle=360.0-angle;
      //template_key.angle = angle;
      template_keypoints.push_back(template_key);
  }
  */
/*
  Ptr<FeatureDetector> detector = FeatureDetector::create("BRISK");
  vector<KeyPoint> input_keypoints, template_keypoints;
  detector->detect(input_img,input_keypoints);
  detector->detect(template_img, template_keypoints);

  for(int i = 0; i < input_keypoints.size(); i++){
//    input_keypoints[i].response = 0;
//    input_keypoints[i].angle = 0;
//    input_keypoints[i].octave = 0;
//    input_keypoints[i].size= 0;

    cout << "--------------------input_keypoints-------------------------" << endl;
    cout << "pt = " << input_keypoints[i].pt << endl;
    cout << "size = " << input_keypoints[i].size << endl;
    cout << "angle = " << input_keypoints[i].angle << endl;
    cout << "response = " << input_keypoints[i].response << endl;
    cout << "octave = " << input_keypoints[i].octave << endl;
    cout << "class_id = " << input_keypoints[i].class_id << endl;

  }
*/
/*
  Mat gray_input, gray_template;
  cvtColor(input_img, gray_input, CV_BGR2GRAY);
  cvtColor(template_img, gray_template, CV_BGR2GRAY);
  // DescriptorExtractorオブジェクトの生成
  Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create("BRISK");
  // 画像の特徴情報を格納するための変数
  Mat input_descriptor;
  // 特徴記述の計算を実行
  extractor->compute(input_img, input_keypoints, input_descriptor);
  // 画像の特徴情報を格納するための変数
  Mat template_descriptor;

  // 特徴記述の計算を実行
  extractor->compute(template_img, template_keypoints, template_descriptor);



  // DescriptorMatcherオブジェクトの生成
  Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");
  // 特徴点のマッチング情報を格納する変数
  vector<DMatch> dmatch;
  // クロスチェックする場合
  vector<DMatch> match12, match21;
  matcher->match(input_descriptor, template_descriptor, match12);
  matcher->match(template_descriptor, input_descriptor, match21);
  for (size_t i = 0; i < match12.size(); i++){
    DMatch forward = match12[i];
    DMatch backward = match21[forward.trainIdx];
    if (backward.trainIdx == forward.queryIdx)
     dmatch.push_back(forward);
  }

  // マッチング結果画像の作成
  Mat result;
  drawMatches(input_img, input_keypoints, template_img, template_keypoints, dmatch, result);
  imwrite("input_img_out/matching.png", result);

-------------------------------------opencv description and matching-------------------------------------------------------
*/
  clock_t end = clock(); //処理時間計測終了
  cout << "duration = " << (double)(end - start) / CLOCKS_PER_SEC << "sec.\n";
  //tf.graphPlot();
  return 0;
}
