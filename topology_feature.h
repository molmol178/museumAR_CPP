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
      Point calib_centroids;//補正した重心(calib_featurepoint()用)
    };

    struct Featurepoints{
      int value;//画素値
      Point coordinate; //座標
      int boundary;//境界 2 or 3
      double ave_keypoint;//平均値
      Point mean_vector;//平均ベクトル
      int min_label_word;//領域が小さい方のラベル値
      vector<int> one_dimention_scanning;//特徴点の周囲16点
      Point calib_coordinate; //補正した座標(calib_featurepoint用)
    };

    struct valueAndVector{
      int beginValue;//始点の画素値
      int endValue;//終点の画素値
      Point beginXY;//始点の座標
      Point begin2endVector;//始点と終点のベクトル
      double vectorSize;//始点と終点のベクトルの大きさ
    };

    struct cent2feature{
      int value;
      Point vectorC2F;
      Point pure_featurepoint;
      double vectorSize;
    };

    struct matchPoint{
      Point template_match;
      Point input_match;
    };
    struct keypoint{
      Point xy;
      vector<int> binary;
    };

    struct simiCos{
      int beginValue;//始点の画素値
      int endValue;//終点の画素値
      Point coordinate; //座標
      double simi_cos;//コサインのシミラリティ
    };

    Mat template_splitRegion(int tmp_range, Mat template_hsv, vector<int> v_count_list, int flag);
    Mat re_label(Mat label_template_img);
    Mat cleanLabelImage(Mat dst_data, int patch_size);
    Mat remapLabel(Mat label_img);
    Mat writeDstData(Mat clean_label_img);
    vector<Centroids> calcCentroids(string centroids_path, Mat img);
    void calib_featurepoint(vector<Centroids> centroids, vector<Featurepoints> featurepoint,Mat img, vector<Centroids> *p_relative_centroids, vector<Featurepoints> *p_relative_featurepoint );


    vector<Featurepoints> featureDetection(int patch_size, Mat changed_label_img);

    void calcMeanVector(int x, int y, int min_label_word, vector<int> label_one_dimention_scanning, Featurepoints *tmp_featurepoint);


    vector<Featurepoints> saveFeaturePoint(int x, int y, int min_word, int min_label_word, vector<int> one_dimention_scanning, vector<int> word_list, int tmp_boundary, vector<Featurepoints> featurepoint, int scanning_center);


    void writeFeaturePoint(Mat template_img, vector<Featurepoints> featurepoint, string filepath);
    vector<keypoint> featureDescription(vector<Featurepoints> featurepoint, Mat label_img );
    Mat inputCreateLabelImg(Mat input_hsv);
    //void featureMatching(Mat template_img, Mat input_img, vector<vector<int> > template_keypoint_binary, vector<vector<int> > input_keypoint_binary,vector<Featurepoints> template_featurepoint, vector<Featurepoints> input_featurepoint);
    vector<cent2feature> calc_relative_centroids2featurepointVector(vector<Centroids> template_relative_centroids, vector<Featurepoints> template_relative_featurepoint);

    void featureMatching(Mat template_img, Mat input_img, vector<keypoint> template_keypoint_binary, vector<keypoint> input_keypoint_binary,vector<cent2feature> template_vector, vector<cent2feature> input_vector);

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


