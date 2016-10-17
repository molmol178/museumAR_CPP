#define _USE_MATH_DEFINES
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include <vector>

using namespace std;
using namespace cv;

Mat template_splitRegion(Mat template_hsv);
Mat re_label(Mat label_template_img);
void featureDetection(Mat label_template_img, vector<vector<int> > *sum_one_dimention_scanning, vector<vector<int> > *sum_xy, vector<vector<int> > *sum_boundary, vector<vector<double> > *sum_ave_keypoint);
void saveFeaturePoint (int x, int y, vector<int> one_dimention_scanning, vector<int> word_list, int tmp_boundary, vector<vector<int> > *sum_one_dimention_scanning, vector<vector<int> > *sum_xy, vector<vector<int> > *sum_boundary, vector<vector<double> > *sum_ave_keypoint);


int main(int argc, char** argv){
  Mat template_img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  Mat template_hsv;
  Mat label_template_img;
  Mat dst_data;

//featureDetection variables
  vector<vector<int> > sum_one_dimention_scanning;
  vector<vector<int> > sum_xy;
  vector<vector<int> > sum_boundary;
  vector<vector<double> > sum_ave_keypoint;

  cvtColor(template_img, template_hsv, CV_BGR2HSV);

  label_template_img = template_splitRegion(template_hsv);
  imwrite("label_template_img.tif", label_template_img);

  dst_data = re_label(label_template_img);
  imwrite("dst_data.jpg", dst_data);

  cout << "feature detection" << endl;
  featureDetection(dst_data, &sum_one_dimention_scanning, &sum_xy, &sum_boundary, &sum_ave_keypoint);
  cout << "finish feature detection" << endl;

  //featureDescription(sum_one_dimention_scanning, sum_xy, dst_data);
  return 0;
}

Mat template_splitRegion(Mat template_hsv){
  int template_x = template_hsv.cols;
  int template_y = template_hsv.rows;
  vector<Mat> planes;
  Mat v_value = Mat(template_y, template_x, CV_8UC1);
  int v_count_list[256] = {0};

  split(template_hsv, planes);
  v_value = planes[2];
  //imwrite("v_value.png", v_value);
  //waitKey(0);

  for(int y = 0; y < template_y; y++ ){
    for (int x = 0; x < template_x; x++){
      int v = v_value.at<unsigned char>(y,x);
      v_count_list[v] += 1;
    }
  }

  float separation_list[256];
  int tmp_range = 15;

  for(int i = 0 + tmp_range; i < 256 - tmp_range; i++){
    float sum_k = 0;
    float sum_j = 0;
    float sum_l = 0;
    float sum_vj = 0;
    float sum_vl = 0;
    float sum_o1_numerator = 0;
    float sum_o2_numerator = 0;

    //cout << "---------------calc start----------------------------" << endl;
    for(int k = i - tmp_range; k < i+ tmp_range; k++){
      if(k != i){
        sum_k += v_count_list[k];
      }
    }
    for(int j = i - tmp_range; j < i - 1; j++){
        sum_j += v_count_list[j];
        sum_vj += v_count_list[j] * j;
    }
    for(int l = i + 1; l < i+ tmp_range; l++){
        sum_l += v_count_list[l];
        sum_vl += v_count_list[l] * l;
    }

    //cout << "sum_k =  " << sum_k << ", sum_j = "<< sum_j << ", sum_vj =" << sum_vj << ", sum_l = " << sum_l << ", sum_vl = " << sum_vl <<endl;

    float w1 = 0;
    float w2 = 0;
    float u1 = 0;
    float u2 = 0;

    //floation point exception 例外処理
    if(sum_k <= 0){
      w1 = 0;
      w2 = 0;
    }else{
      w1 = sum_j / sum_k;
      w2 = sum_l / sum_k;
    }
    if(sum_j <= 0){
      u1 = 0;
    }else{
      u1 = sum_vj / sum_j;
    }
    if(sum_l <= 0){
      u2 = 0;
    }else{
      u2 = sum_vl / sum_l;
    }
    //cout << "w1 =  " << w1 << ", w2 = "<< w2 << ", u1 =" << u1 << ", u2 = " << u2 <<endl;

    for(int j4o1 = i - tmp_range; j4o1 < i + 1; j4o1++){
        sum_o1_numerator += v_count_list[j4o1] * pow(j4o1 - u1, 2.0);
    }
    for(int l4o2 = i - 1; l4o2 < i + tmp_range; l4o2++){
        sum_o2_numerator += v_count_list[l4o2] * pow(l4o2 - u2, 2.0);
    }

    float o1 = 0;
    float o2 = 0;

    //floating point exception 例外処理
    if (sum_j <= 0){
      o1 = 0;
    }else{
      o1 = sum_o1_numerator / sum_j;
    }
    if (sum_l <= 0){
      o2 = 0;
    }else{
      o2 = sum_o2_numerator / sum_l;
    }

    float o12 = w1 * w2 * pow(u1 - u2, 2.0) + (w1 * o1 + w2 * o2);

    //cout << "o1 =  " << o1 << ", o2 = "<< o2 << ", o12 =" << o12 <<endl;

    //floating point exception 例外処理
    float n;
    if (o12 <= 0){
      n = 0;
    }else{
       n = w1 * w2 * pow(u1 - u2, 2.0) / o12;
    }
    //cout << "n = " << n << endl;
    //cout << "--------------------calc end-----------------------" << endl;
    separation_list[i] = n;
  }
  cout << "separation_list" <<endl;
  for(int i = 0; i < sizeof(separation_list) / sizeof(separation_list[0]); i++){
    cout << separation_list[i] << ", ";
  }
  cout << endl;
  cout << endl;
  cout << endl;


  int label_list[256];
  int b = 1;
  label_list[0] = 0;
  label_list[255] = 0;
  for(int a = 0 + 1; a < 256 - 1; a++){
    label_list[a] = b;
    if(separation_list[a] > separation_list[a-1] &&
       separation_list[a] > separation_list[a+1] &&
       separation_list[a] > 2 / M_PI ){
        b++;
        label_list[a] = b;
    }
  }

  cout << "label_list" << endl;
  for(int i = 0; i < sizeof(label_list)/ sizeof(label_list[0]); i++){
    cout << label_list[i] << ", ";
  }
  cout << endl;


  Mat label_template_img = Mat(template_y, template_x, CV_8UC1);

  for(int ty = 0; ty < template_y; ty++){
    for(int tx = 0; tx < template_x; tx++){
      label_template_img.at<unsigned char>(ty,tx) = label_list[v_value.at<unsigned char>(ty,tx)];
    }
  }

  return label_template_img;

}

Mat re_label(Mat label_template_img){
  int template_x = label_template_img.cols;
  int template_y = label_template_img.rows;

  Mat dst_data = Mat(template_y, template_x, CV_8UC1);
  int next_label = 1;
  int area_template = template_y * template_x;
  int table[area_template];

  for (int i = 0; i < area_template; i++){
    table[i] = i;
  }

  for (int y = 0; y < template_y; y++){
    for (int x = 0; x < template_x; x++){
      if (x == 0 && y == 0){
        dst_data.at<unsigned char>(y,x) = next_label;
        next_label++;
        continue;
      }
      if (y == 0){
        if (label_template_img.at<unsigned char>(y,x) == label_template_img.at<unsigned char>(y,x-1)){
          dst_data.at<unsigned char>(y,x) = dst_data.at<unsigned char>(y, x-1);
        }else{
          dst_data.at<unsigned char>(y,x) = next_label;
          next_label++;
          continue;
        }
      }
      if (x == 0){
        if (label_template_img.at<unsigned char>(y,x) == label_template_img.at<unsigned char>(y-1,x)){
          dst_data.at<unsigned char>(y,x) = dst_data.at<unsigned char>(y-1,x);
        }else{
          dst_data.at<unsigned char>(y,x) = next_label;
          next_label++;
          continue;
          }
      }

      int s1 = label_template_img.at<unsigned char>(y,x-1);
      int s2 = label_template_img.at<unsigned char>(y-1,x);

      if (label_template_img.at<unsigned char>(y,x) == s2 &&
          label_template_img.at<unsigned char>(y,x) == s1){

        int ai = dst_data.at<unsigned char>(y,x-1);
        int bi = dst_data.at<unsigned char>(y-1,x);

        if (ai != bi){
          while(table[ai] != ai){
            ai = table[ai];
          }
          while(table[bi] != bi){
            bi = table[bi];
          }
          if (ai != bi){
            table[bi] = ai;
          }
        }
        if (ai < bi){
          dst_data.at<unsigned char>(y,x) = ai;
        }else{
          dst_data.at<unsigned char>(y,x) = bi;
        }
        continue;
      }

      if (label_template_img.at<unsigned char>(y,x) == s2){
        dst_data.at<unsigned char>(y,x) =  dst_data.at<unsigned char>(y-1,x);
        continue;
      }

       if (label_template_img.at<unsigned char>(y,x) == s1){
        dst_data.at<unsigned char>(y,x) =  dst_data.at<unsigned char>(y,x-1);
        continue;
      }

      dst_data.at<unsigned char>(y,x) = next_label;
      next_label++;
    }
  }

  int index;
  for (int ix = 0; ix < next_label; ix++){
    index = ix;
    while(table[index] != index){
      index = table[index];
    }
    table[ix] = index;
  }

  int label[next_label];
  index = 1;

  for (int jx = 0; jx < next_label; jx++){
    if (table[jx] == jx){
      label[jx] = index;
      index++;
    }
  }

  for (int y = 0; y < template_y; y++){
    for (int x = 0; x < template_x; x++){
      int tables = table[dst_data.at<unsigned char>(y,x)];
      dst_data.at<unsigned char>(y,x) = label[tables];
    }
  }
  return dst_data;
}

void featureDetection(Mat label_template_img, vector<vector<int> > *sum_one_dimention_scanning, vector<vector<int> > *sum_xy, vector<vector<int> > *sum_boundary, vector<vector<double> > *sum_ave_keypoint){

  vector<vector<int> > this_sum_one_dimention_scanning = *sum_one_dimention_scanning;
  vector<vector<int> > this_sum_xy = *sum_xy;
  vector<vector<int> > this_sum_boundary = *sum_boundary;
  vector<vector<double> > this_sum_ave_keypoint = *sum_ave_keypoint;

  int x_size = label_template_img.cols;
  int y_size = label_template_img.rows;
  int n = 16;
  int one_n = 16;

  Mat scanning_filter = Mat(n, n, CV_8UC1);
  int scan_x = scanning_filter.cols;
  int scan_y = scanning_filter.rows;

  for(int y = 0; y < y_size - scan_y; y += 3){
    for(int x = 0; x < x_size - scan_x; x += 3){
      for(int s_y = 0; s_y < scan_y; s_y++){
        for(int s_x = 0; s_x < scan_x; s_x++){
          scanning_filter.at<unsigned char>(s_y, s_x) = label_template_img.at<unsigned char>(s_y + y, s_x + x);
          int one_d_data[] = {scanning_filter.at<unsigned char>(0,3),scanning_filter.at<unsigned char>(0,4),scanning_filter.at<unsigned char>(1,5),scanning_filter.at<unsigned char>(2,6),scanning_filter.at<unsigned char>(3,6),scanning_filter.at<unsigned char>(4,6),scanning_filter.at<unsigned char>(5,5),scanning_filter.at<unsigned char>(6,4),scanning_filter.at<unsigned char>(6,3),scanning_filter.at<unsigned char>(6,2),scanning_filter.at<unsigned char>(5,1),scanning_filter.at<unsigned char>(4,0),scanning_filter.at<unsigned char>(3,0),scanning_filter.at<unsigned char>(2,0),scanning_filter.at<unsigned char>(1,1),scanning_filter.at<unsigned char>(0,2)};

          vector<int> one_dimention_scanning(one_d_data,end(one_d_data));
          int scanning_center = {scanning_filter.at<unsigned char>(3,3)};
          int one_d_count = 0;

          for (int one_d = 0; one_d < one_n - 1; one_d++){
            if(one_dimention_scanning[one_d] != one_dimention_scanning[one_d + 1]){
              one_d_count ++;
            }
          }

          //one_dimention_scanningの要素の種類とその数をリスト
          vector<int> word_list;
          vector<int> cnt_list;
          int one_d_size = one_dimention_scanning.size();

          word_list.push_back(one_dimention_scanning[0]);
          cnt_list.push_back(1);

          for(int o = 1; o < one_d_size; o++){
            int match = 0;
            for (int w = 0; w < word_list.size(); w++){
              if (word_list[w] == one_dimention_scanning[o]){
                cnt_list[w] += 1;
                match = 1;
              }
            }
              if(match == 0){
                word_list.push_back(one_dimention_scanning[o]);
                cnt_list.push_back(1);
              }
         }
/*
         cout << "-------------------one_d_lists----------------------" << endl;
         cout << "one_d_count = " << one_d_count <<endl;
         cout << "one_dimention_scanning = ";
         for (int a = 0; a < one_d_size; a++){
           cout << one_dimention_scanning[a] << ", ";
         }
         cout << endl;
         cout << "word_list = ";
         for (int i = 0; i < word_list.size(); i++){
             cout << word_list[i] << ", ";
         }
         cout << endl;
         cout << "cnt_list  = ";
         for (int j = 0; j < cnt_list.size(); j++){
             cout << cnt_list[j] << ", ";
         }
         cout << endl;
*/


          //cnt_listの最小値をとる
          int min_cnt = *min_element(cnt_list.begin(), cnt_list.end());
          //cnt_listの最小値のためのイテレータ
          vector<int>::iterator min_cntIt = min_element(cnt_list.begin(), cnt_list.end());
          //cnt_listの最小値のインデックス
          size_t cnt_minIndex = distance(cnt_list.begin(), min_cntIt);

          vector<int> element_check(one_dimention_scanning[0]);
          for (int i = 1; i <= one_dimention_scanning.size(); i++){
            for (int j = 0; j < element_check.size(); j++){
                if(element_check[j] != one_dimention_scanning[i]){
                  element_check.push_back(one_dimention_scanning[i]);
                }
            }
          }

          //要素数が2,短い方のラベルが中心画素と同じ
          if(one_d_count == 1){
            if(element_check.size() == 2 && scanning_center == word_list[cnt_minIndex]){
              //最も小さいラベルが120度以下
              if(min_cnt <= one_n / 3){
                //word_listの合計を求める
                int tmp_boundary = 2;
                saveFeaturePoint(x, y, one_dimention_scanning, word_list, tmp_boundary, &this_sum_one_dimention_scanning, &this_sum_xy, &this_sum_boundary, &this_sum_ave_keypoint);
              }
            }
          }
          else if(one_d_count == 2){
            if(element_check.size() == 3){
              int tmp_boundary = 3;
              saveFeaturePoint(x, y, one_dimention_scanning,word_list, tmp_boundary, &this_sum_one_dimention_scanning, &this_sum_xy, &this_sum_boundary, &this_sum_ave_keypoint);
              }
            if (one_dimention_scanning[0] == one_dimention_scanning[-1] && scanning_center == word_list[cnt_minIndex]){
              //最も小さいラベルが120度以下
              if(min_cnt <= one_n / 3){
              int tmp_boundary = 2;
              saveFeaturePoint(x, y, one_dimention_scanning,word_list, tmp_boundary, &this_sum_one_dimention_scanning, &this_sum_xy, &this_sum_boundary, &this_sum_ave_keypoint);
              }
            }
        }
        else if(one_d_count == 3){
            if (one_dimention_scanning[0] == one_dimention_scanning[-1]){
              //最も小さいラベルが120度以下
              int tmp_boundary = 3;
              saveFeaturePoint(x, y, one_dimention_scanning,word_list, tmp_boundary, &this_sum_one_dimention_scanning, &this_sum_xy, &this_sum_boundary, &this_sum_ave_keypoint);
            }
         }
        }
      }
    }
  }

  cout << "this_sum_one_dimention_sacnning " << endl;
  for (int i = 0; i < this_sum_one_dimention_scanning.size(); i++){
    for (int j = 0; j < this_sum_one_dimention_scanning[0].size(); j++){
      cout << this_sum_one_dimention_scanning[i][j] << ", ";
    }
    cout << endl;
  }

  //return
  *sum_one_dimention_scanning = this_sum_one_dimention_scanning;
  *sum_xy = this_sum_xy;
  *sum_boundary = this_sum_boundary;
  *sum_ave_keypoint = this_sum_ave_keypoint;
}

void saveFeaturePoint (int x, int y, vector<int> one_dimention_scanning,vector<int> word_list, int tmp_boundary, vector<vector<int> > *sum_one_dimention_scanning, vector<vector<int> > *sum_xy, vector<vector<int> > *sum_boundary, vector<vector<double> > *sum_ave_keypoint){

  vector<vector<int> > this_sum_one_dimention_scanning = *sum_one_dimention_scanning;
  vector<vector<int> > this_sum_xy = *sum_xy;
  vector<vector<int> > this_sum_boundary = *sum_boundary;
  vector<vector<double> > this_sum_ave_keypoint = *sum_ave_keypoint;

  vector<int> tmp_sum_xy;
  vector<int> tmp_sum_boundary;
  vector<double> tmp_sum_keypoint;

  float words_sum = 0;
  for (int words = 0; words < word_list.size(); words++){
    words_sum += word_list[words];
  }

  //word_listの平均を求める
  float ave_keypoint = words_sum / word_list.size();
  tmp_sum_keypoint.push_back(ave_keypoint);
  this_sum_ave_keypoint.push_back(tmp_sum_keypoint);

  this_sum_one_dimention_scanning.push_back(one_dimention_scanning);

  tmp_sum_xy.push_back(x+3);
  tmp_sum_xy.push_back(y+3);
  this_sum_xy.push_back(tmp_sum_xy);

  tmp_sum_boundary.push_back(tmp_boundary);
  this_sum_boundary.push_back(tmp_sum_boundary);
}

