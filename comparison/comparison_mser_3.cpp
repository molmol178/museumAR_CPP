#define _USE_MATH_DEFINES
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
//#include <opencv2/nonfree/nonfree.hpp>
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/xfeatures2d.hpp"
//#include "opencv2/features2d.hpp"
#include <math.h>
#include <vector>
#include <time.h>
#include <fstream>

using namespace std;
using namespace cv;

int main(int argc, char** argv){
  clock_t start = clock(); //処理時間計測開始


  // 時間計算のための周波数
  double f = 1000.0 / cv::getTickFrequency();

  int64 time_s; //スタート時間
  double time_detect; // 検出エンド時間
  double time_match; // マッチングエンド時間


  Mat target = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  Mat scene= imread(argv[2], CV_LOAD_IMAGE_COLOR);
  Mat t_gray, s_gray, dst;
  //cvtColor(target, t_gray, CV_BGR2GRAY);
  //cvtColor(scene, s_gray, CV_BGR2GRAY);
  t_gray = target.clone();
  s_gray = scene.clone();
  hconcat(target, scene, dst);

    // 特徴点検出と特徴量計算

//cv::Ptr<cv::Feature2D> feature;
//cv::Ptr<cv::Feature2D> feature1;
//cv::xfeatures2d::initModule_xfeatures2d();

std::stringstream ss;
cv::Ptr<xfeatures2d::SIFT>feature = xfeatures2d::SIFT::create();
cv::Ptr<MSER> feature1 = cv::MSER::create();

ss << "SIFT";
//feature1 = cv::MSER::create();
ss << "MSER";
std::cout << "--- 計測（" << ss.str() << "） ---" << std::endl;


//******************************
// キーポイント検出と特徴量記述
//******************************
std::vector<cv::KeyPoint> kpts1, kpts2;
cv::Mat desc1, desc2;


//feature1->detectAndCompute(t_gray, cv::noArray(), kpts1, desc1);
feature1->detect(t_gray, kpts1);
feature->compute(t_gray, kpts1, desc1);

time_s = cv::getTickCount(); // 時間計測 Start
feature1->detect(s_gray, kpts2);
feature->compute(s_gray, kpts2, desc2);
time_detect = (cv::getTickCount() - time_s)*f; // 時間計測 Stop

if (desc2.rows == 0){
std::cout << "WARNING: 特徴点検出できず" << std::endl;
return 0;
}

//*******************
// 特徴量マッチング
//*******************
//BFMatcher matchtype = feature->defaultNorm(); // SIFT, SURF: NORM_L2
 // BRISK, ORB, KAZE, A-KAZE: NORM_HAMMING
 cv::BFMatcher matcher(feature->defaultNorm());
 std::vector<std::vector<cv::DMatch > > knn_matches;


 time_s = cv::getTickCount(); // 時間計測 Start
 // 上位2点
 matcher.knnMatch(desc1, desc2, knn_matches, 2);
 time_match = (cv::getTickCount() - time_s)*f; // 時間計測 Stop


 //***************
 // 対応点を絞る
 //***************
 const float match_par = .6f; //対応点のしきい値
 std::vector<cv::DMatch> good_matches;

 std::vector<cv::Point2f> match_point1;
 std::vector<cv::Point2f> match_point2;

 for (size_t i = 0; i < knn_matches.size(); ++i) {
 float dist1 = knn_matches[i][0].distance;
 float dist2 = knn_matches[i][1].distance;

 //良い点を残す（最も類似する点と次に類似する点の類似度から）
 if (dist1 <= dist2 * match_par) {
 good_matches.push_back(knn_matches[i][0]);
 match_point1.push_back(kpts1[knn_matches[i][0].queryIdx].pt);
 match_point2.push_back(kpts2[knn_matches[i][0].trainIdx].pt);
 }
 }

 //ホモグラフィ行列推定
 cv::Mat masks;
 cv::Mat H;
 if (match_point1.size() != 0 && match_point2.size() != 0) {
 H = cv::findHomography(match_point1, match_point2, masks, cv::RANSAC, 3.f);
 }

 //RANSACで使われた対応点のみ抽出
 std::vector<cv::DMatch> inlierMatches;
 for (int i = 0; i < masks.rows; ++i) {
 uchar *inlier = masks.ptr<uchar>(i);
 if (inlier[0] == 1) {
 inlierMatches.push_back(good_matches[i]);
 }
 }
 //特徴点の表示
 cv::drawMatches(target, kpts1, scene, kpts2, good_matches, dst);
 imwrite("allmatch_mser_sift.png", dst);

 //インライアの対応点のみ表示
 cv::drawMatches(target, kpts1, scene, kpts2, inlierMatches, dst);
 imwrite("inlier_mser_sift.png", dst);

  double ratio = inlierMatches.size()*1.0 / good_matches.size();
 std::cout << "検出時間: " << time_detect << " [ms]" << std::endl;
 std::cout << "照合時間: " << time_match << " [ms]" << std::endl;
 std::cout << "Good Matches: " << good_matches.size() << std::endl;
 std::cout << "Inlier: " << inlierMatches.size() << std::endl;
 std::cout << "Inlier ratio: " << ratio << std::endl;
 std::cout << "target Keypoints: " << kpts1.size() << std::endl;
 std::cout << "scene Keypoints: " << kpts2.size() << std::endl;

  return 0;
}
