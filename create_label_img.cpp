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


  Mat input_img;
  Mat template_img;

  int patch_size;
  int tmp_range;

  patch_size = 7;
  tmp_range = 30;

  input_img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  template_img = imread(argv[2], CV_LOAD_IMAGE_COLOR);

//start creating label img
  Mat Gaussian_input;
  Mat input_hsv;
  Mat label_input_img;
  Mat input_dst_data;
  Mat input_clean_label_img;
  Mat input_changed_label_img;
  Mat input_last_label_img;
  Mat input_last_changed_label_img;
  Mat input_changed_clean_label_img;

  Mat Gaussian_template;
  Mat template_hsv;
  Mat label_template_img;
  Mat template_dst_data;
  Mat template_clean_label_img;
  Mat template_changed_label_img;
  Mat template_last_label_img;
  Mat template_last_changed_label_img;
  Mat template_changed_clean_label_img;


  //start for template_img
  cout << "template labeling----------------------------------------------------------" << endl;
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
  oned_intCsvWriter(v_count_list, v_count_path);

  //for gnuplot
  string v_path = "graphPlot/template_v_count_list.csv";
  oned_vertical_intCsvWriter(v_count_list, v_path);
  //ここまで--------------------------------------------------------------------------

  int flag = 0;
  label_template_img = template_splitRegion(tmp_range, template_hsv, v_count_list, flag);

  imwrite("template_img_out/template_1st_label.tiff", label_template_img);

  cout << "1st labeling finish" << endl;
  template_dst_data = re_label(label_template_img);
  template_changed_label_img = writeDstData(template_dst_data);
  cout << "2nd labeling finish" << endl;

  template_clean_label_img = cleanLabelImage(template_dst_data, patch_size);
  template_changed_clean_label_img = writeDstData(template_clean_label_img);
  cout << "3rd labeling finish" << endl;

  template_last_label_img = remapLabel(template_clean_label_img);
  template_last_changed_label_img = writeDstData(template_last_label_img);
  cout << "4th labeling finish" << endl;

  imwrite("template_img_out/template_2nd_label.tiff", template_changed_label_img);
  imwrite("template_img_out/template_3rd_label.tiff", template_changed_clean_label_img);
  imwrite("template_img_out/template_4th_label.tiff", template_last_changed_label_img);


  cout << "input labeling-----------------------------------------------------" << endl;
  GaussianBlur(input_img, Gaussian_input, Size(7, 7), 1.5, 1.5);
  //cvtColor(Gaussian_input, input_hsv, CV_BGR2HSV);
  cvtColor(Gaussian_input, input_hsv, CV_BGR2RGB);

  label_input_img = inputCreateLabelImg(input_hsv);
  imwrite("input_img_out/input_1st_label.tiff", label_input_img);
  cout << "1st labeling finish" << endl;

  input_dst_data = re_label(label_input_img);
  input_changed_label_img = writeDstData(input_dst_data);
  cout << "2nd labeling finish" << endl;

  input_clean_label_img = cleanLabelImage(input_dst_data, patch_size);
  cout << "testtest" << endl;
  input_changed_clean_label_img = writeDstData(input_clean_label_img);
  cout << "3rd labeling finish" << endl;

  input_last_label_img = remapLabel(input_clean_label_img);
  input_last_changed_label_img = writeDstData(input_last_label_img);
  cout << "4th labeling finish" << endl;

  imwrite("input_img_out/input_2nd_label.tiff", input_clean_label_img);
  imwrite("input_img_out/input_3rd_label.tiff", input_changed_clean_label_img);
  imwrite("input_img_out/input_4th_label.tiff", input_last_changed_label_img);
  graphPlot();
}
