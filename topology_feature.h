#define _USE_MATH_DEFINES
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include <vector>
#include <fstream>

using namespace std;
using namespace cv;
class TopologyFeature{
  public:
    Mat template_splitRegion(int tmp_range, Mat template_hsv);
    Mat re_label(Mat label_template_img);
    Mat cleanLabelImage(Mat dst_data, int patch_size);
    Mat remapLabel(Mat label_img);


    Mat writeDstData(Mat clean_label_img);


    vector<vector<int> > featureDetection(int patch_size, Mat changed_label_img,  vector<vector<int> > *sum_one_dimention_scanning, vector<vector<int> > *sum_xy, vector<vector<int> > *sum_boundary, vector<vector<double> > *sum_ave_keypoint, vector<vector<int> > *sum_mean_vector);
    void calcMeanVector(int x, int y, int min_label_word, vector<int> label_one_dimention_scanning, vector<vector<int> > *sum_mean_vector);

    vector<vector<int> > saveFeaturePoint (int x, int y,int min_label_word,vector<int> one_dimention_scanning,vector<int> word_list, int tmp_boundary, vector<vector<int> > *sum_label_one_dimention_scanning, vector<vector<int> > *sum_xy, vector<vector<int> > *sum_boundary, vector<vector<double> > *sum_ave_keypoint, vector<vector<int> > sum_min_label_word);

    //vector<vector<int> > featureDetection(int patch_size, Mat label_img, Mat changed_label_img, vector<vector<int> > *sum_one_dimention_scanning, vector<vector<int> > *sum_xy, vector<vector<int> > *sum_boundary, vector<vector<double> > *sum_ave_keypoint);
    //vector<vector<int> > saveFeaturePoint (int x, int y,int min_label_word,vector<int> label_one_dimention_scanning,vector<int> word_list, int tmp_boundary, vector<vector<int> > *sum_one_dimention_scanning, vector<vector<int> > *sum_xy, vector<vector<int> > *sum_boundary, vector<vector<double> > *sum_ave_keypoint, vector<vector<int> > sum_min_label_word);


    Mat writeFeaturePoint(Mat template_img, vector<vector<int> > *sum_xy, vector<vector<int> > *sum_boundary);
    vector<vector<int> > featureDescription(vector<vector<int> > *sum_label_one_dimention_scanning, Mat label_img );
    //vector<vector<int> > featureDescription(vector<vector<int> > *sum_one_dimention_scanning, Mat label_img );
    Mat inputCreateLabelImg(Mat input_hsv);
    void featureMatching(Mat input_img, Mat template_img, vector<vector<int> > keypoint_binary, vector<vector<int> > sum_label_one_dimention_scanning, vector<vector<int> > sum_xy, vector<vector<int> > sum_boundary, vector<vector<double> > sum_ave_keypoint, vector<vector<int> > sum_min_label_word, vector<vector<int> > sum_mean_vector);
    //void featureMatching(Mat input_img, Mat template_img, vector<vector<int> > keypoint_binary, vector<vector<int> > sum_one_dimention_scanning, vector<vector<int> > sum_xy, vector<vector<int> > sum_boundary, vector<vector<double> > sum_ave_keypoint, vector<vector<int> > sum_min_label_word);

    vector<int> oned_intCsvReader(string filePath);
    vector<vector<int> > twod_intCsvReader(string filePath);
    vector<vector<double> > twod_doubleCsvReader(string filePath);

    void oned_intCsvWriter(vector<int> file, string filePath);
    void oned_floatCsvWriter(vector<float> file, string filePath);

    void oned_vertical_intCsvWriter(vector<int> file, string filePath);
    void oned_vertical_floatCsvWriter(vector<float> file, string filePath);

    void twod_intCsvWriter(vector<vector<int> > file, string filePath);
    void twod_doubleCsvWriter(vector<vector<double> > file, string filePath);

    int graphPlot();
};


