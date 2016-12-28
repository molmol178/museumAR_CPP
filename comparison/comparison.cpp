#define _USE_MATH_DEFINES
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include <vector>
#include <time.h>
//#include "asift.cpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv){
  clock_t start = clock(); //処理時間計測開始

  //ASiftDetector as;


  Mat input_img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  Mat template_img = imread(argv[2], CV_LOAD_IMAGE_COLOR);
  //Mat input_img = imread(argv[1], 0);
  //Mat template_img = imread(argv[2], 0);


  // SIFT・SURFモジュールの初期化
  //initModule_nonfree();

  Ptr<FeatureDetector> detector = FeatureDetector::create("ORB");
  vector<KeyPoint> input_keypoints, template_keypoints;
  detector->detect(input_img,input_keypoints);
  detector->detect(template_img, template_keypoints);

  //cvtColor(input_img, gray_input, CV_BGR2GRAY);
  //cvtColor(template_img, gray_template, CV_BGR2GRAY);
  // DescriptorExtractorオブジェクトの生成
  Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create("ORB");
  // 画像の特徴情報を格納するための変数
  Mat input_descriptor;
  // 特徴記述の計算を実行
  extractor->compute(input_img, input_keypoints, input_descriptor);
  // 画像の特徴情報を格納するための変数
  Mat template_descriptor;
  // 特徴記述の計算を実行
  extractor->compute(template_img, template_keypoints, template_descriptor);

  // DescriptorMatcherオブジェクトの生成
  //Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");
  //Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
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
  imwrite("matching.png", result);

  clock_t end = clock(); //処理時間計測終了
  cout << "duration = " << (double)(end - start) / CLOCKS_PER_SEC << "sec.\n";

  return 0;
}
