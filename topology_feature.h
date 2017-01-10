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

namespace TopologyFeature{
    //calcCentroid用の構造体
    struct Value_xy{
      int value; //画素値
      Point focus_pt; //座標
    };

    struct Centroids{
      int value;//画素値
      Point centroids; //重心座標
    };

    struct Featurepoints{
      int value;//画素値
      Point coordinate; //座標
      int boundary;//境界 2 or 3
      double ave_keypoint;//平均値
      Point mean_vector;//平均ベクトル
      int min_label_word;//領域が小さい方のラベル値
      vector<int> one_dimention_scanning;//特徴点の周囲16点
    };

    Mat template_splitRegion(int tmp_range, Mat template_hsv, vector<int> v_count_list, int flag);
    Mat re_label(Mat label_template_img);
    Mat cleanLabelImage(Mat dst_data, int patch_size);
    Mat remapLabel(Mat label_img);
    Mat writeDstData(Mat clean_label_img);
    vector<Centroids> calcCentroids(string centroids_path, Mat img);
    void calib_input_featurepoint(vector<Centroids> centroids, vector<Featurepoints> featurepoint, Mat input_img);


    vector<Featurepoints> featureDetection(int patch_size, Mat changed_label_img);

    void calcMeanVector(int x, int y, int min_label_word, vector<int> label_one_dimention_scanning, Featurepoints *tmp_featurepoint);


    vector<Featurepoints> saveFeaturePoint(int x, int y, int min_label_word, vector<int> one_dimention_scanning, vector<int> word_list, int tmp_boundary, vector<Featurepoints> featurepoint, int scanning_center);


    void writeFeaturePoint(Mat template_img, vector<Featurepoints> featurepoint, string filepath);
    vector<vector<int> > featureDescription(vector<Featurepoints> featurepoint, Mat label_img );
    Mat inputCreateLabelImg(Mat input_hsv);
    void featureMatching(Mat template_img, Mat input_img, vector<vector<int> > template_keypoint_binary, vector<vector<int> > input_keypoint_binary,vector<Featurepoints> template_featurepoint, vector<Featurepoints> input_featurepoint);
    void calc_Homography(vector<Point2f> template_pt, vector<Point2f> input_pt, vector<DMatch> goodMatch);
    vector<int> oned_intCsvReader(string filePath);
    vector<vector<int> > twod_intCsvReader(string filePath);
    vector<vector<double> > twod_doubleCsvReader(string filePath);
    vector<vector<float> > twod_floatCsvReader(string filePath);


    void oned_intCsvWriter(vector<int> file, string filePath);
    void oned_floatCsvWriter(vector<float> file, string filePath);

    void oned_vertical_intCsvWriter(vector<int> file, string filePath);
    void oned_vertical_floatCsvWriter(vector<float> file, string filePath);

    void twod_intCsvWriter(vector<vector<int> > file, string filePath);
    void twod_doubleCsvWriter(vector<vector<double> > file, string filePath);
    void twod_floatCsvWriter(vector<vector<float> > file, string filePath);
    int graphPlot();
};


