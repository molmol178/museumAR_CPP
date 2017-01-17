#define _USE_MATH_DEFINES
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include <vector>
#include <time.h>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
using namespace cv;

int main(int argc, char** argv){
  clock_t start = clock(); //処理時間計測開始

  vector<Point2f> match_point1;
  vector<Point2f> match_point2;
  Point2f point1;
  Point2f point2;

  //ASIFTで求めたマッチングペアをtxtファイルからそれぞれPoint2fの中に代入

  ifstream file("demo_ASIFT_src/matchings.txt");
  string buffer;
  int cnt = 0;
  while(getline(file, buffer)){
    istringstream stream(buffer);
    string token;
    while(getline(stream, token, ' ')){
      cnt++;
      float tmp = stof(token);
      if(cnt == 1){
        point1.x = tmp;
      }else if(cnt == 2){
        point1.y = tmp;
      }else if(cnt == 3){
        point2.x = tmp;
      }else if(cnt == 4){
        point2.y = tmp;
      }
    }
    cnt = 0;
    match_point1.push_back(point1);
    match_point2.push_back(point2);
  }

  vector<DMatch> good_matches;
  DMatch tmp;
  for(int i = 0; i < match_point1.size(); i++){
    cout << match_point1[i].x << "  " << match_point1[i].y << "  " << match_point2[i].x << "  " << match_point2[i].y << endl;
    tmp.queryIdx = i;
    tmp.trainIdx = i;
    good_matches.push_back(tmp);
  }


  //matching ratioの計算
  Mat masks;

  if (match_point1.size() >= 4 && match_point2.size() >= 4) {
    Mat H = findHomography(match_point1, match_point2, masks, CV_LMEDS);
    cout << "homography = " << H <<endl;
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
  double inliner = inlinerMatch.size();
  double good = good_matches.size();
  double matching_ratio = inliner / good;
  cout << "matching ratio = " << matching_ratio << endl;


  return 0;
}
