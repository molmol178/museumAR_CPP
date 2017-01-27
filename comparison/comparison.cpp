#define _USE_MATH_DEFINES
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/nonfree/nonfree.hpp>
#include <math.h>
#include <vector>
#include <time.h>
#include <fstream>

using namespace std;
using namespace cv;

int main(int argc, char** argv){
  //clock_t start = clock(); //処理時間計測開始


  // 時間計算のための周波数
  double f = 1000.0 / cv::getTickFrequency();
  int64 time_all = cv::getTickCount(); // 時間計測 Start


  int64 time_s; //スタート時間
  double time_detect; // 検出エンド時間
  double time_match; // マッチングエンド時間
  double time_desc;
  double time_alls;

  Mat input_img = imread(argv[1], 0);
  Mat template_img = imread(argv[2], 0);
  string feature_name = argv[3];

 // SIFT・SURFモジュールの初期化
  //initModule_contrib();
  initModule_nonfree();

  //Detector初期化
  Ptr<FeatureDetector> detector = FeatureDetector::create(feature_name);
  vector<KeyPoint> input_keypoints, template_keypoints;

  // DescriptorExtractorオブジェクトの生成
  Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create(feature_name);
  // 画像の特徴情報を格納するための変数
  Mat input_descriptor;
  Mat template_descriptor;

  detector->detect(input_img,input_keypoints);
  extractor->compute(input_img, input_keypoints, input_descriptor);

  time_s = cv::getTickCount(); // 時間計測 Start

  detector->detect(template_img, template_keypoints);
  time_detect = (cv::getTickCount() - time_s)*f; // 時間計測 Stop

  time_s = cv::getTickCount(); // 時間計測 Start

  extractor->compute(template_img, template_keypoints, template_descriptor);
  time_desc = (cv::getTickCount() - time_s)*f; // 時間計測 Stop

  cout << "keypoint_count " << template_keypoints.size() << endl;
  // DescriptorMatcherオブジェクトの生成
  //Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");
  Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
  //Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");
  vector<vector<DMatch> > knn_matches;

  time_s = cv::getTickCount(); // 時間計測 Start

  // 上位2点
  matcher->knnMatch(input_descriptor, template_descriptor, knn_matches, 2);
  //対応点を絞る
  const double match_par = 0.6; //対応点のしきい値
  vector<DMatch> good_matches;

  vector<Point2f> match_point1;
  vector<Point2f> match_point2;

  for (size_t i = 0; i < knn_matches.size(); ++i) {
    double dist1 = knn_matches[i][0].distance;
    double dist2 = knn_matches[i][1].distance;

     //良い点を残す（最も類似する点と次に類似する点の類似度から）
     if (dist1 <= dist2 * match_par) {
       good_matches.push_back(knn_matches[i][0]);
       match_point1.push_back(input_keypoints[knn_matches[i][0].queryIdx].pt);
       match_point2.push_back(template_keypoints[knn_matches[i][0].trainIdx].pt);
    }
  }
  time_match = (cv::getTickCount() - time_s)*f; // 時間計測 Stop


  time_alls = (cv::getTickCount() - time_all)*f; // 時間計測 Stop
  //cout << ","<< time_detect << "," << time_desc << "," << time_match << "," << time_alls << endl; 
  //cout << "," <<  input_keypoints.size() << "," << template_keypoints.size() << endl; 

  // マッチング結果画像の作成
  Mat result;
  drawMatches(input_img, input_keypoints, template_img, template_keypoints, good_matches, result);
  imwrite("matching.png", result);

  //clock_t end = clock(); //処理時間計測終了
  //cout << "duration = " << (double)(end - start) / CLOCKS_PER_SEC << "sec.\n";

/* 
  for(int i = 0; i < good_matches.size(); i ++ ){
    //cout << "good_match query " << good_matches[i].queryIdx << endl;
    //cout << "good_match train " << good_matches[i].trainIdx << endl;
  }
*/



  //matching ratioの計算
  Mat masks;

  if (match_point1.size() >= 4 && match_point2.size() >= 4) {
    //Mat H = findHomography(match_point1, match_point2, masks, CV_LMEDS);
    Mat H = findHomography(match_point1, match_point2, masks, CV_RANSAC, 3);

    //cout << "homography = " << H <<endl;
  }

  //while(true){
    //imshow("goodmatch", goodMatch);
  //}
  //RANSACで使われた対応点のみ抽出
  vector<DMatch> inlinerMatch;
  for (int i = 0; i < masks.rows; ++i) {
    uchar *inliner = masks.ptr<uchar>(i);
    if (inliner[0] == 1) {
      inlinerMatch.push_back(good_matches[i]);
    }
  }
  cout << "inlinerMatch count = "<< inlinerMatch.size() <<endl;
  cout << "allMatch count = " << good_matches.size() << endl;
  double inliner = inlinerMatch.size() * 1.0;
  double good = good_matches.size();
  double matching_ratio = inliner / good;
  cout << "matching ratio = " << matching_ratio << endl;

  //inlinerのみ表示
  Mat inliner_result;
  drawMatches(input_img, input_keypoints, template_img, template_keypoints, inlinerMatch, inliner_result);

  Mat text_plane = Mat::zeros(350, input_img.cols + template_img.cols,CV_8UC3);

  stringstream ss;
  cv::putText(text_plane, feature_name, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1, CV_AA);
  ss.str("");
  ss << "Detection & Description";
  cv::putText(text_plane, ss.str(), cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 255), 1, CV_AA);
  ss.str("");
  ss << "Time: " << time_detect << " [ms]";
  cv::putText(text_plane, ss.str(), cv::Point(10, 90), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 255), 1, CV_AA);
  ss.str("");
  ss << "Matching";
  cv::putText(text_plane, ss.str(), cv::Point(10, 120), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 255), 1, CV_AA);
  ss.str("");
  ss << "Time: " << time_match << " [ms]";
  cv::putText(text_plane, ss.str(), cv::Point(10, 150), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 255), 1, CV_AA);

  ss.str("");
  ss << "--Matches--";
  cv::putText(text_plane, ss.str(), cv::Point(10, 180), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0), 1, CV_AA);

  ss.str("");
  ss << "ALL Matches: " << good_matches.size();
  cv::putText(text_plane, ss.str(), cv::Point(10, 210), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0), 1, CV_AA);

  ss.str("");
  ss << "Inlier Matches: " << inlinerMatch.size();
  cv::putText(text_plane, ss.str(), cv::Point(10, 240), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0), 1, CV_AA);

  ss.str("");
  double ratio = .0;
  if (good_matches.size() != .0)
    ratio = inlinerMatch.size()*1.0 / good_matches.size();
  ss << "Matching ratio: " << ratio;
  cv::putText(text_plane, ss.str(), cv::Point(10, 270), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0), 1, CV_AA);

  ss.str("");
  ss << "Template keypoints: " << template_keypoints.size();
  cv::putText(text_plane, ss.str(), cv::Point(10, 300), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 255), 1, CV_AA);

  ss.str("");
  ss << "Input keypoints: " << input_keypoints.size();
  cv::putText(text_plane, ss.str(), cv::Point(10, 330), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 255), 1, CV_AA);




  Mat sum_img(inliner_result.cols + text_plane.cols, inliner_result.rows, CV_8UC3);
  vconcat(inliner_result, text_plane, sum_img);
  imwrite("inliner_matching.png", sum_img);



  return 0;
}
