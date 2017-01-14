#define _USE_MATH_DEFINES
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include <vector>
#include <fstream>
#include "topology_feature.h"
using namespace std;
using namespace cv;

/*
---------------------------------------------------------------------------------------------------
ヒストグラム分割によるラベリング
templateとinputで同じ領域で別れるようにする
---------------------------------------------------------------------------------------------------
*/
Mat TopologyFeature::template_splitRegion(int separate_range, Mat template_hsv, vector<int> v_count_list ,int flag){

  int template_x = template_hsv.cols;
  int template_y = template_hsv.rows;
  vector<Mat> planes;
  Mat v_value = Mat(template_y, template_x, CV_8UC1);
  //vector<int> v_count_list(255, 0);

  split(template_hsv, planes);
  v_value = planes[2];
  //cvtColor(template_hsv, v_value, CV_RGB2GRAY);
/*
  for(int y = 0; y < template_y; y++ ){
    for (int x = 0; x < template_x; x++){
      int v = v_value.at<unsigned char>(y,x);
      v_count_list[v] += 1;
    }
  }

  string v_count_path = "template_img_out/template_v_count_list.csv";
  oned_intCsvWriter(v_count_list, v_count_path);

  //for gnuplot
  string v_path = "graphPlot/template_v_count_list.csv";
  oned_vertical_intCsvWriter(v_count_list, v_path);
*/

  vector<float> separation_list(255,0);
  int tmp_range = separate_range;

  for(int i = 0 + tmp_range; i < 255 - tmp_range; i++){
    float sum_k = 0;
    float sum_j = 0;
    float sum_l = 0;
    float sum_vj = 0;
    float sum_vl = 0;
    float sum_o1_numerator = 0;
    float sum_o2_numerator = 0;

    //cout << "---------------calc start----------------------------" << endl;
    for(int k = i - tmp_range; k < i + tmp_range; k++){
      if(k != i){
        sum_k += v_count_list[k];
      }
    }
    for(int j = i - tmp_range; j < i - 1; j++){
        sum_j += v_count_list[j];
        sum_vj += v_count_list[j] * j;
    }
    for(int l = i + 1; l < i + tmp_range; l++){
        sum_l += v_count_list[l];
        sum_vl += v_count_list[l] * l;
    }

    //cout << "sum_k =  " << sum_k << ", sum_j = "<< sum_j << ", sum_vj =" << sum_vj << ", sum_l = " << sum_l << ", sum_vl = " << sum_vl <<endl;

    float w1 = 0;
    float w2 = 0;
    float u1 = 0;
    float u2 = 0;

    //floating point exception 例外処理
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

    for(int j4o1 = i - tmp_range; j4o1 < i - 1; j4o1++){
        sum_o1_numerator += v_count_list[j4o1] * pow(j4o1 - u1, 2.0);
    }
    for(int l4o2 = i + 1; l4o2 < i + tmp_range; l4o2++){
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

    float o12 = 0;
    o12 = w1 * w2 * pow(u1 - u2, 2.0) + (w1 * o1 + w2 * o2);

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
  if (flag == 0){
    string sep_path = "template_img_out/template_separation_list.csv";
    oned_floatCsvWriter(separation_list, sep_path);
    //for gnuplot
    string sep_vertical_path = "graphPlot/template_separation_list.csv";
    oned_vertical_floatCsvWriter(separation_list, sep_vertical_path);
  }else if(flag == 1){
    string sep_path = "input_img_out/input_separation_list.csv";
    oned_floatCsvWriter(separation_list, sep_path);
    //for gnuplot
    string sep_vertical_path = "graphPlot/input_separation_list.csv";
    oned_vertical_floatCsvWriter(separation_list, sep_vertical_path);
  }

  vector<int> label_list(255,0);
  vector<float> tmp_sep_list;
  vector<int> sep_index;
  vector<int> sep_max_index;
  int b = 1;
  for(int a = 0 + 1; a < 255 - 1; a++){
    /*
    label_list[a] = b;
    if(separation_list[a] > separation_list[a-1] &&
       separation_list[a] > separation_list[a+1] &&
       separation_list[a] > 2 / M_PI ){
        b++;
        label_list[a] = b;
    }
    */
    if(separation_list[a] > 2/ M_PI){
      tmp_sep_list.push_back(separation_list[a]);
      sep_index.push_back(a);
      if(separation_list[a+1] < 2/ M_PI){
        //tmp_sep_listの中の最大値を分離点とする．
        vector<float>::iterator max_tmp_sepIt = max_element(tmp_sep_list.begin(), tmp_sep_list.end());
        size_t tmp_sep_maxIndex = distance(tmp_sep_list.begin(), max_tmp_sepIt);
        sep_max_index.push_back(sep_index[tmp_sep_maxIndex]);

        cout << "tmp_sep_list" << endl;
        for(int i = 0; i < tmp_sep_list.size(); i++){
          cout << tmp_sep_list[i] << ", ";
        }
        cout <<endl;
        cout << "sep_max_index" << endl;
        for(int i = 0; i < sep_max_index.size(); i++){
          cout << sep_max_index[i] << ", ";
        }
        cout <<endl;
        cout << endl;

        tmp_sep_list.erase(tmp_sep_list.begin(), tmp_sep_list.end());
        sep_index.erase(sep_index.begin(), sep_index.end());

      }
    }
  }
  for(int i = 0; i < label_list.size(); i++){
    for(int j = 0; j < sep_max_index.size(); j++){
      label_list[i] = b;
      if(i == sep_max_index[j]){
        b += 1;
      }
    }
  }

  label_list[0] = label_list[1];
  label_list[255] = label_list[254];

  cout << "label_list" << endl;
  for(int i = 0; i < label_list.size(); i++){
    cout << label_list[i] << ", ";
  }
  cout << endl;

  string label_path = "template_img_out/label_list.csv";
  oned_intCsvWriter(label_list, label_path);

  //for gnuplot
  if(flag == 0){
    string label_vertical_path = "graphPlot/label_list.csv";
    oned_vertical_intCsvWriter(label_list, label_vertical_path);
  }else if(flag == 1){
    string label_vertical_path = "graphPlot/input_label_list.csv";
    oned_vertical_intCsvWriter(label_list, label_vertical_path);
  }

  Mat label_template_img = Mat(template_y, template_x, CV_8UC1);

  for(int ty = 0; ty < template_y; ty++){
    for(int tx = 0; tx < template_x; tx++){
      label_template_img.at<unsigned char>(ty,tx) = label_list[v_value.at<unsigned char>(ty,tx)];
    }
  }
  return label_template_img;
}

/*
---------------------------------------------------------------------------------------------------
再ラベリング
１つの領域にユニークなラベル値が連番でつくようにする
---------------------------------------------------------------------------------------------------
*/
Mat TopologyFeature::re_label(Mat label_template_img){
  int template_x = label_template_img.cols;
  int template_y = label_template_img.rows;

  Mat dst_data = Mat(template_y, template_x, CV_16U);
  int next_label = 1;
  int area_template = template_y * template_x;
  int table[area_template];

  for (int i = 0; i < area_template; i++){
    table[i] = i;
  }

  for (int y = 0; y < template_y; y++){
    for (int x = 0; x < template_x; x++){
      if (x == 0 && y == 0){
        dst_data.at<unsigned short>(y,x) = next_label;
        next_label++;
        continue;
      }
      if (y == 0){
        if (label_template_img.at<unsigned char>(y,x) == label_template_img.at<unsigned char>(y,x-1)){
          dst_data.at<unsigned short>(y,x) = dst_data.at<unsigned short>(y, x-1);
        }else{
          dst_data.at<unsigned short>(y,x) = next_label;
          next_label++;
        }
        continue;
      }
      if (x == 0){
        if (label_template_img.at<unsigned char>(y,x) == label_template_img.at<unsigned char>(y-1,x)){
          dst_data.at<unsigned short>(y,x) = dst_data.at<unsigned short>(y-1,x);
        }else{
          dst_data.at<unsigned short>(y,x) = next_label;
          next_label++;
        }
        continue;
      }

      int s1 = label_template_img.at<unsigned char>(y,x-1);
      int s2 = label_template_img.at<unsigned char>(y-1,x);

      if (label_template_img.at<unsigned char>(y,x) == s2 &&
          label_template_img.at<unsigned char>(y,x) == s1){

        int ai = dst_data.at<unsigned short>(y,x-1);
        int bi = dst_data.at<unsigned short>(y-1,x);

        if (ai != bi){
          int cnt = 1;
          while(table[ai] != ai){
            ai = table[ai];
            cnt++;
          }
          while(table[bi] != bi){
            bi = table[bi];
          }
          if (ai != bi){
            table[bi] = ai;
          }
        }
        if (ai < bi){
          dst_data.at<unsigned short>(y,x) = ai;
        }else{
          dst_data.at<unsigned short>(y,x) = bi;
        }
        continue;
      }

      if (label_template_img.at<unsigned char>(y,x) == s2){
        dst_data.at<unsigned short>(y,x) =  dst_data.at<unsigned short>(y-1,x);
        continue;
      }

       if (label_template_img.at<unsigned char>(y,x) == s1){
        dst_data.at<unsigned short>(y,x) =  dst_data.at<unsigned short>(y,x-1);
        continue;
      }

      dst_data.at<unsigned short>(y,x) = next_label;
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
      int tables = table[dst_data.at<unsigned short>(y,x)];
      dst_data.at<unsigned short>(y,x) = label[tables];
    }
  }

  return dst_data;
}

/*
---------------------------------------------------------------------------------------------------
パッチサイズよりも小さい領域を周りのラベル値で埋める
---------------------------------------------------------------------------------------------------
*/
Mat TopologyFeature::cleanLabelImage(Mat dst_data, int patch_size){

  int x_size = dst_data.cols;
  int y_size = dst_data.rows;
  //距離画像
  Mat tmp_dst_data = Mat::zeros(y_size, x_size, CV_8UC1);

  //dst_dataの最大値を求める(maxVal)
  double maxVal;
  minMaxLoc(dst_data, NULL, &maxVal);

  //maxValの大きさの配列を作る
  vector<int> label_list(maxVal, 0);
  int label;

  for(int y = 0; y < y_size; y++){
    for(int x = 0; x < x_size; x++){
      label = dst_data.at<unsigned short>(y, x);
      label_list[label] += 1;
    }
  }


  //パッチサイズよりも小さい領域を0にする
  int dst_xy;
  for(int y = 0; y < y_size; y++){
    for(int x = 0; x < x_size; x++){
      dst_xy = dst_data.at<unsigned short>(y, x);
      if (label_list[dst_xy] < patch_size/2 * patch_size/2 * M_PI){
        dst_data.at<unsigned short>(y,x) = 0;
      }
    }
  }
  //Mat cleaned_dst_data;
  //cleaned_dst_data = writeDstData(dst_data);
  //imwrite("cleaned_dst_data.tiff" , cleaned_dst_data );

  //１回目のスキャン．左上から右下へ
  int tmp_dst_xy;
  int dst_up;
  int dst_left;
  int tmp_dst_up;
  int tmp_dst_left;

  for(int y = 1; y < y_size; y++){
    for(int x = 1; x < x_size; x++){

      if(dst_data.at<unsigned short>(y, x) == 0){
        dst_xy = dst_data.at<unsigned short>(y, x);
        tmp_dst_xy = tmp_dst_data.at<int>(y, x);
        dst_up = dst_data.at<unsigned short>(y-1, x);
        dst_left = dst_data.at<unsigned short>(y, x-1);
        tmp_dst_up = tmp_dst_data.at<int>(y-1, x);
        tmp_dst_left = tmp_dst_data.at<int>(y, x-1);

        //上と左の画素を見て面積の大きい方をラベル画像に代入
        if(label_list[dst_up] >= label_list[dst_left]){
          dst_data.at<unsigned short>(y,x) = dst_up;
        }else{
          dst_data.at<unsigned short>(y,x) = dst_left;
        }
        //上と左の画素を見て値の小さい方に+1して代入
        if(tmp_dst_up >= tmp_dst_left){
           tmp_dst_data.at<int>(y,x) = tmp_dst_left + 1;
        }else{
          tmp_dst_data.at<int>(y,x) = tmp_dst_up + 1;
        }
      }
    }
  }


  //２回目のスキャン．右下から左上へ
  int dst_down;
  int dst_right;
  int tmp_dst_down;
  int tmp_dst_right;

  for(int y = y_size - 2; y > 1; y--){
    for(int x = x_size - 2; x > 1; x--){

      if(tmp_dst_data.at<int>(y, x) > 0){
        dst_xy = dst_data.at<unsigned short>(y, x);
        tmp_dst_xy = tmp_dst_data.at<int>(y, x);
        dst_down = dst_data.at<unsigned short>(y+1, x);
        dst_right = dst_data.at<unsigned short>(y, x+1);
        tmp_dst_down = tmp_dst_data.at<int>(y+1, x);
        tmp_dst_right = tmp_dst_data.at<int>(y, x+1);

        //距離画像に対して１回目のスキャンのときの値より大きい値を代入できる時
        if(tmp_dst_xy > tmp_dst_down + 1
          || tmp_dst_xy > tmp_dst_right + 1){

          //下と右の画素を見て面積の大きい方をラベル画像に代入
          if(label_list[dst_down] == label_list[dst_right]){//右と下が同じ時
            if(label_list[dst_down] >= label_list[dst_right]){
              dst_data.at<unsigned short>(y, x) = dst_down;
            }else{
              dst_data.at<unsigned short>(y, x) = dst_right;
            }
          }else{ //dst_dataに近い方のラベルを代入，tmp_dst_dataに下と右の画素を見て値の小さい方に+1して代入
            if(tmp_dst_down >= tmp_dst_right){
              dst_data.at<unsigned short>(y, x) = dst_right;
              tmp_dst_data.at<int>(y, x) = tmp_dst_right + 1;
            }else{
              dst_data.at<unsigned short>(y, x) = dst_down;
              tmp_dst_data.at<int>(y, x) = tmp_dst_down + 1;
            }
          }
        }
      }
    }
  }

  return dst_data;
}
/*
---------------------------------------------------------------------------------------------------
画像の内面積が大きい領域から順にラベルを付け直す
---------------------------------------------------------------------------------------------------
*/
Mat TopologyFeature::remapLabel(Mat label_img){

  int y_size = label_img.rows;
  int x_size = label_img.cols;
  Mat remap_label_img = Mat(y_size, x_size, CV_16UC1);

  //label_imgの最大値を求める(maxVal)
  double maxVal;
  minMaxLoc(label_img, NULL, &maxVal);

  //maxValの大きさの配列を作る
  vector<int> label_list(maxVal, 0);
  int label;

  //label_imgの面積を求めて配列に格納
  for(int y = 0; y < y_size; y++){
    for(int x = 0; x < x_size; x++){
      label = label_img.at<unsigned short>(y, x);
      label_list[label] += 1;
    }
  }

  int max_list = *max_element(label_list.begin(), label_list.end());
  cout << "max label_list = " << max_list  <<endl;
  vector<int> asc_index_label_list;

  while(max_list != 0){
    for(int i = 0; i < label_list.size(); i++){
      if(label_list[i] == max_list){
        asc_index_label_list.push_back(i);
      }
      if(i == label_list.size() -1){
        max_list = max_list -1;
      }
    }
  }
  for(int i = 0; i < asc_index_label_list.size(); i++){
    for(int y = 0; y < y_size; y++){
      for(int x = 0; x < x_size; x++){
        if(label_img.at<unsigned short>(y,x) == asc_index_label_list[i]){
          remap_label_img.at<unsigned short>(y,x) = i;
        }
      }
    }
  }
  return remap_label_img;
}

/*
---------------------------------------------------------------------------------------------------
unsigned short型の画像にランダムな値を入れてunsigned charで書き出せるようにする
---------------------------------------------------------------------------------------------------
*/
Mat TopologyFeature::writeDstData(Mat clean_label_img){
  int y_size = clean_label_img.rows;
  int x_size = clean_label_img.cols;
  Mat changed_label_img = Mat(y_size, x_size, CV_8UC1);

  double maxVal;
  minMaxLoc(clean_label_img, NULL, &maxVal);

  vector<int> label_list(maxVal, 0);
  int label;


  for(int y = 0; y < y_size; y++){
    for(int x = 0; x < x_size; x++){
      label = clean_label_img.at<unsigned char>(y, x);
      label_list[label] += 1;
    }
  }

  for(int i = 0; i < label_list.size(); i++){
    int lucky = 1 + rand() % (255 - 1);
    for(int y = 0; y < y_size; y++){
      for(int x = 0; x < x_size; x++){
          if(clean_label_img.at<unsigned char>(y, x) == i){
            changed_label_img.at<unsigned char>(y, x) = lucky;
            //changed_label_img.at<unsigned char>(y, x) = i;
          }
      }
    }
  }
  return changed_label_img;
}
/*
---------------------------------------------------------------------------------------------------
各領域の重心とその領域のラベルを取得
---------------------------------------------------------------------------------------------------
*/
vector<TopologyFeature::Centroids> TopologyFeature::calcCentroids(string centroids_path, Mat img){

  int y_size = img.rows;
  int x_size = img.cols;

  vector<Value_xy> value_xy;
  Value_xy tmp_value_xy;

  for(int y = 0; y < y_size; y++){
    for(int x = 0; x < x_size; x++){
        tmp_value_xy.value = img.at<unsigned char>(y,x);
        tmp_value_xy.focus_pt.x = x;
        tmp_value_xy.focus_pt.y = y;
        value_xy.push_back(tmp_value_xy);
    }
  }
  vector<int> word_list;
  vector<int> cnt_list;

  word_list.push_back(value_xy[0].value);
  cnt_list.push_back(1);

  for(int o = 1; o < value_xy.size(); o++){
    int match = 0;
    for (int w = 0; w < word_list.size(); w++){
      if (word_list[w] == value_xy[o].value){
        cnt_list[w] += 1;
        match = 1;
      }
    }
    if(match == 0){
      word_list.push_back(value_xy[o].value);
      cnt_list.push_back(1);
    }
  }
  /*
  for(int i = 0; i < word_list.size(); i++){
    cout << "word = " << word_list[i] << " cnt = " << cnt_list[i]<<endl;
  }
  */

  int y_numerator;
  int x_numerator;
  int value;
  double denominator;
  vector<Centroids> centroids;
  Centroids tmp_centroids;

  for(int w = 0; w < word_list.size(); w++){
    for(int c = 0; c < value_xy.size(); c++){
      if(word_list[w] == value_xy[c].value){
        denominator = cnt_list[w] * word_list[w];
        x_numerator += value_xy[c].focus_pt.x * word_list[w];
        y_numerator += value_xy[c].focus_pt.y * word_list[w];
      }
    }
    tmp_centroids.value = word_list[w];
    if(denominator > 0){
      tmp_centroids.centroids.x = x_numerator / denominator;
      tmp_centroids.centroids.y = y_numerator / denominator;
    }else{
      tmp_centroids.centroids.x = 0;
      tmp_centroids.centroids.y = 0;
    }
    centroids.push_back(tmp_centroids);
    x_numerator = 0;
    y_numerator = 0;
  }


  Mat writeimg = img.clone();
  //cout << "centroids" <<endl;
  for(int i = 0; i < centroids.size(); i++){
    circle(writeimg, Point(centroids[i].centroids.x, centroids[i].centroids.y), 2 ,Scalar(10), 1,4 );
    //cout << "value = " << centroids[i].value <<" x = " << centroids[i].centroids.x << " y = " << centroids[i].centroids.y << endl;
  }

  imwrite(centroids_path, writeimg);
  return centroids;
}
/*
---------------------------------------------------------------------------------------------------
トポロジの特徴を用いた特徴点検出
---------------------------------------------------------------------------------------------------
*/
vector<TopologyFeature::Featurepoints> TopologyFeature::featureDetection(int patch_size, Mat changed_label_img){


  int x_size = changed_label_img.cols;
  int y_size = changed_label_img.rows;
  int n = patch_size;
  int one_n = 16; //patch_sizeによって変わる

  vector<Featurepoints> featurepoint;

  Mat scanning_filter(n, n, CV_8U);

  int scan_x = scanning_filter.cols;
  int scan_y = scanning_filter.rows;
  int featurepoint_cnt = 0;


  for(int y = 0; y < y_size - scan_y; y++){
    for(int x = 0; x < x_size - scan_x; x++){
      for(int s_y = 0; s_y < scan_y; s_y++){
        for(int s_x = 0; s_x < scan_x; s_x++){
          scanning_filter.at<unsigned char>(s_y, s_x) = changed_label_img.at<unsigned char>(s_y + y, s_x + x);
        }
      }
          //patch_sizeによって変わる
          int one_d_data[] = {scanning_filter.at<unsigned char>(0,3),scanning_filter.at<unsigned char>(0,4),scanning_filter.at<unsigned char>(1,5),scanning_filter.at<unsigned char>(2,6),scanning_filter.at<unsigned char>(3,6),scanning_filter.at<unsigned char>(4,6),scanning_filter.at<unsigned char>(5,5),scanning_filter.at<unsigned char>(6,4),scanning_filter.at<unsigned char>(6,3),scanning_filter.at<unsigned char>(6,2),scanning_filter.at<unsigned char>(5,1),scanning_filter.at<unsigned char>(4,0),scanning_filter.at<unsigned char>(3,0),scanning_filter.at<unsigned char>(2,0),scanning_filter.at<unsigned char>(1,1),scanning_filter.at<unsigned char>(0,2)};

          vector<int> one_dimention_scanning(one_d_data,end(one_d_data));
          //patch_sizeによって変わる
          int scanning_center = {scanning_filter.at<unsigned char>(3,3)};
          int one_d_count = 0;


          //one_d_count パッチの切れ目を数える
          for (int one_d = 0; one_d < one_n - 1; one_d++){
            if(one_dimention_scanning[one_d] != one_dimention_scanning[one_d + 1]){
              one_d_count ++;
            }
          }
          //パッチ上のピクセルがプレーンな点は処理しない
          if(one_d_count != 0){
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

            //cnt_listの最小値をとる
            int min_cnt = *min_element(cnt_list.begin(), cnt_list.end());
            //cnt_listの最小値のためのイテレータ
            vector<int>::iterator min_cntIt = min_element(cnt_list.begin(), cnt_list.end());
            //cnt_listの最小値のインデックス
            size_t cnt_minIndex = distance(cnt_list.begin(), min_cntIt);

            //cnt_listの最小値をとる
            int min_word = *min_element(word_list.begin(), word_list.end());


            //要素数が2,短い方のラベルが中心画素と同じ
            if(one_d_count == 1){
              if(word_list.size() == 2 && scanning_center == word_list[cnt_minIndex]){
                //最も小さいラベルが120度以下
                if(min_cnt <= one_n / 3){
                  featurepoint_cnt ++;
                  //word_listの合計を求める
                  int tmp_boundary = 2;
                  int min_label_word = word_list[cnt_minIndex];
                  featurepoint = saveFeaturePoint(x, y, min_word, min_label_word, one_dimention_scanning, word_list, tmp_boundary, featurepoint, scanning_center);
                }
              }
            }
            else if(one_d_count == 2){
              if(word_list.size() == 3){
                  featurepoint_cnt ++;
                int tmp_boundary = 3;
                int min_label_word = word_list[cnt_minIndex];
                featurepoint = saveFeaturePoint(x, y, min_word, min_label_word, one_dimention_scanning, word_list, tmp_boundary, featurepoint, scanning_center);
                }
              //patch_sizeによって変わる
              if (one_dimention_scanning[0] == one_dimention_scanning[15] && scanning_center == word_list[cnt_minIndex]){
                //最も小さいラベルが120度以下
                if(min_cnt <= one_n / 3){
                  featurepoint_cnt ++;
                int tmp_boundary = 2;
                int min_label_word = word_list[cnt_minIndex];
                featurepoint = saveFeaturePoint(x, y, min_word, min_label_word, one_dimention_scanning, word_list, tmp_boundary, featurepoint, scanning_center);
                }
              }
          }
          else if(one_d_count == 3){
              //patch_sizeによって変わる
              if (one_dimention_scanning[0] == one_dimention_scanning[15]){
                  featurepoint_cnt ++;
                //最も小さいラベルが120度以下
                int tmp_boundary = 3;
                int min_label_word = word_list[cnt_minIndex];
                featurepoint = saveFeaturePoint(x, y, min_word, min_label_word, one_dimention_scanning, word_list, tmp_boundary, featurepoint, scanning_center);
              }
           }
        }
    }
  }
  cout << "featurepoint count = " << featurepoint_cnt << endl;
  return featurepoint;
}

/*
---------------------------------------------------------------------------------------------------
検出した特徴点を保存する
---------------------------------------------------------------------------------------------------
*/
vector<TopologyFeature::Featurepoints> TopologyFeature::saveFeaturePoint(int x, int y, int min_word, int min_label_word, vector<int> one_dimention_scanning, vector<int> word_list, int tmp_boundary, vector<Featurepoints> featurepoint, int scanning_center){


  Featurepoints tmp_featurepoint;

  tmp_featurepoint.one_dimention_scanning = one_dimention_scanning;
  //tmp_featurepoint.value = scanning_center;
  tmp_featurepoint.value = min_word;


  float words_sum = 0;
  for (int words = 0; words < word_list.size(); words++){
    words_sum += word_list[words];
  }

  //word_listの平均を求める
  float ave_keypoint = words_sum / word_list.size();
  tmp_featurepoint.ave_keypoint = ave_keypoint;

  //patch_sizeによって変わる
  tmp_featurepoint.coordinate.y = y+3;
  tmp_featurepoint.coordinate.x = x+3;

  tmp_featurepoint.boundary = tmp_boundary;

  tmp_featurepoint.min_label_word = min_label_word;

  calcMeanVector(x, y, min_label_word, one_dimention_scanning, &tmp_featurepoint);

  featurepoint.push_back(tmp_featurepoint);
  return featurepoint;
}

/*
---------------------------------------------------------------------------------------------------
検出した特徴点の小さい方のラベルがキーポイントの中心に対して作る平均ベクトルを求める．
---------------------------------------------------------------------------------------------------
*/
void TopologyFeature::calcMeanVector(int x, int y, int min_label_word, vector<int> label_one_dimention_scanning, Featurepoints *tmp_featurepoint){


  vector<vector<int> > sum_min_label_coordinate;
  vector<int> tmp_min_label_coordinate;

  //cout << endl << endl;
  //cout << "--------------------------------------------------------------------" << endl;
  //cout<< "min label word = " << min_label_word << endl;

  for(int i = 0; i < label_one_dimention_scanning.size(); i++){
    //cout << "label_one_dimention_scanning" << endl;
    //cout << label_one_dimention_scanning[i] << ", " << endl;
    //cout << "x = " << x << "y = " << y <<endl;
    //cout << "i = " << i << endl;

    //cout << "tmp_min_label_coordinate" <<endl;
    //for(int j = 0; j < tmp_min_label_coordinate.size(); j++){
    //  cout << tmp_min_label_coordinate[j] << ", ";
    //}
    //cout << endl;
    if(min_label_word ==  label_one_dimention_scanning[i]){
      if(i == 0){
        tmp_min_label_coordinate.push_back(y+0);
        tmp_min_label_coordinate.push_back(x+3);
        sum_min_label_coordinate.push_back(tmp_min_label_coordinate);
        tmp_min_label_coordinate.erase(tmp_min_label_coordinate.begin(), tmp_min_label_coordinate.end());
      }
      else if(i == 1){
        tmp_min_label_coordinate.push_back(y+0);
        tmp_min_label_coordinate.push_back(x+4);
        sum_min_label_coordinate.push_back(tmp_min_label_coordinate);
        tmp_min_label_coordinate.erase(tmp_min_label_coordinate.begin(), tmp_min_label_coordinate.end());
      }
      else if(i == 2){
        tmp_min_label_coordinate.push_back(y+1);
        tmp_min_label_coordinate.push_back(x+5);
        sum_min_label_coordinate.push_back(tmp_min_label_coordinate);
        tmp_min_label_coordinate.erase(tmp_min_label_coordinate.begin(), tmp_min_label_coordinate.end());
      }
      else if(i == 3){
        tmp_min_label_coordinate.push_back(y+2);
        tmp_min_label_coordinate.push_back(x+6);
        sum_min_label_coordinate.push_back(tmp_min_label_coordinate);
        tmp_min_label_coordinate.erase(tmp_min_label_coordinate.begin(), tmp_min_label_coordinate.end());
      }
      else if(i == 4){
        tmp_min_label_coordinate.push_back(y+3);
        tmp_min_label_coordinate.push_back(x+6);
        sum_min_label_coordinate.push_back(tmp_min_label_coordinate);
        tmp_min_label_coordinate.erase(tmp_min_label_coordinate.begin(), tmp_min_label_coordinate.end());
      }
      else if(i == 5){
        tmp_min_label_coordinate.push_back(y+4);
        tmp_min_label_coordinate.push_back(x+6);
        sum_min_label_coordinate.push_back(tmp_min_label_coordinate);
        tmp_min_label_coordinate.erase(tmp_min_label_coordinate.begin(), tmp_min_label_coordinate.end());
      }
      else if(i == 6){
        tmp_min_label_coordinate.push_back(y+5);
        tmp_min_label_coordinate.push_back(x+5);
        sum_min_label_coordinate.push_back(tmp_min_label_coordinate);
        tmp_min_label_coordinate.erase(tmp_min_label_coordinate.begin(), tmp_min_label_coordinate.end());
      }
      else if(i == 7){
        tmp_min_label_coordinate.push_back(y+6);
        tmp_min_label_coordinate.push_back(x+4);
        sum_min_label_coordinate.push_back(tmp_min_label_coordinate);
        tmp_min_label_coordinate.erase(tmp_min_label_coordinate.begin(), tmp_min_label_coordinate.end());
      }
      else if(i == 8){
        tmp_min_label_coordinate.push_back(y+6);
        tmp_min_label_coordinate.push_back(x+3);
        sum_min_label_coordinate.push_back(tmp_min_label_coordinate);
        tmp_min_label_coordinate.erase(tmp_min_label_coordinate.begin(), tmp_min_label_coordinate.end());
      }
      else if(i == 9){
        tmp_min_label_coordinate.push_back(y+6);
        tmp_min_label_coordinate.push_back(x+2);
        sum_min_label_coordinate.push_back(tmp_min_label_coordinate);
        tmp_min_label_coordinate.erase(tmp_min_label_coordinate.begin(), tmp_min_label_coordinate.end());
      }
      else if(i == 10){
        tmp_min_label_coordinate.push_back(y+5);
        tmp_min_label_coordinate.push_back(x+1);
        sum_min_label_coordinate.push_back(tmp_min_label_coordinate);
        tmp_min_label_coordinate.erase(tmp_min_label_coordinate.begin(), tmp_min_label_coordinate.end());
      }
      else if(i == 11){
        tmp_min_label_coordinate.push_back(y+4);
        tmp_min_label_coordinate.push_back(x+0);
        sum_min_label_coordinate.push_back(tmp_min_label_coordinate);
        tmp_min_label_coordinate.erase(tmp_min_label_coordinate.begin(), tmp_min_label_coordinate.end());
      }
      else if(i == 12){
        tmp_min_label_coordinate.push_back(y+3);
        tmp_min_label_coordinate.push_back(x+0);
        sum_min_label_coordinate.push_back(tmp_min_label_coordinate);
        tmp_min_label_coordinate.erase(tmp_min_label_coordinate.begin(), tmp_min_label_coordinate.end());
      }
      else if(i == 13){
        tmp_min_label_coordinate.push_back(y+2);
        tmp_min_label_coordinate.push_back(x+0);
        sum_min_label_coordinate.push_back(tmp_min_label_coordinate);
        tmp_min_label_coordinate.erase(tmp_min_label_coordinate.begin(), tmp_min_label_coordinate.end());
      }
      else if(i == 14){
        tmp_min_label_coordinate.push_back(y+1);
        tmp_min_label_coordinate.push_back(x+1);
        sum_min_label_coordinate.push_back(tmp_min_label_coordinate);
        tmp_min_label_coordinate.erase(tmp_min_label_coordinate.begin(), tmp_min_label_coordinate.end());
      }
      else if(i == 15){
        tmp_min_label_coordinate.push_back(y+0);
        tmp_min_label_coordinate.push_back(x+2);
        sum_min_label_coordinate.push_back(tmp_min_label_coordinate);
        tmp_min_label_coordinate.erase(tmp_min_label_coordinate.begin(), tmp_min_label_coordinate.end());
      }
    }
  }
  /*
  cout << "calc vector=========================================================" << endl;;
  cout << "sum_min_label_coordinate" << endl;
  for(int i = 0; i < sum_min_label_coordinate.size(); i++){
    for(int j = 0; j < sum_min_label_coordinate[i].size(); j++){
      cout << sum_min_label_coordinate[i][j] << ", " ;
    }
    cout << endl;
  }
  */
  //calc mean vector
  int y_vector = 0;
  int x_vector = 0;
  int sum_y_vector = 0;
  int sum_x_vector = 0;
  int cnt_vector = 0;
  for(int j = 0; j < sum_min_label_coordinate.size(); j ++){
    //中心とパッチの小さい方のベクトルを計算
    y_vector = sum_min_label_coordinate[j][0] - (y + 3);
    x_vector = sum_min_label_coordinate[j][1] - (x + 3);
    sum_y_vector += y_vector;
    sum_x_vector += x_vector;
    cnt_vector ++;
  }
  tmp_featurepoint->mean_vector.y = sum_y_vector/ cnt_vector;
  tmp_featurepoint->mean_vector.x = sum_x_vector/ cnt_vector;
  //return tmp_featurepoint.mean_vector;
  /*
  cout << "sum_mean_vector" << endl;
  for(int i = 0; i < mean_vector.size(); i++){
    for(int j = 0; j < mean_vector[0].size(); j++){
      cout << mean_vector[i][j] << ", " ;
    }
    cout << endl;
  }
  */
  //return
  //*sum_mean_vector = mean_vector;
}


/*
---------------------------------------------------------------------------------------------------
検出した特徴点を画像上にマッピング
---------------------------------------------------------------------------------------------------
*/

void TopologyFeature::writeFeaturePoint(Mat template_img, vector<Featurepoints> featurepoint, string filepath){

  for(int i = 0; i< featurepoint.size(); i++){
    vector<int> top_left(2,0);
    vector<int> bottom_right(2,0);

    top_left[0] = featurepoint[i].coordinate.y - 3;
    top_left[1] = featurepoint[i].coordinate.x - 3;
    bottom_right[0] = featurepoint[i].coordinate.y + 3;
    bottom_right[1] = featurepoint[i].coordinate.x + 3;

    if(featurepoint[i].boundary == 2){
      template_img.at<unsigned char>(featurepoint[i].coordinate) = 150;
      rectangle(template_img, Point(top_left[1],top_left[0]), Point(bottom_right[1],bottom_right[0]), Scalar(150), 1);

    }
    else if(featurepoint[i].boundary == 3){
      template_img.at<unsigned char>(featurepoint[i].coordinate) = 70;
      rectangle(template_img, Point(top_left[1],top_left[0]), Point(bottom_right[1],bottom_right[0]), Scalar(70), 1);
    }
  }
  imwrite(filepath, template_img);
}
/*
---------------------------------------------------------------------------------------------------
templateとinputの重心座標を使ってinputの特徴点座標のキャリブレーション(回転不変に)
---------------------------------------------------------------------------------------------------
*/
void TopologyFeature::calib_featurepoint(vector<Centroids> centroids, vector<Featurepoints> featurepoint, Mat img, vector<Centroids> *p_relative_centroids, vector<Featurepoints> *p_relative_featurepoint){

  double maxVal;
  double minVal;
  minMaxLoc(img, &minVal, &maxVal);

  int min_index;
  for(int i = 0; i < centroids.size(); i++) {
    if(centroids[i].value == minVal){
      min_index = i;
    }
  }

  vector<valueAndVector> valAndVector;
  valueAndVector tmp_valAndVector;


  for(int i = 0; i < centroids.size(); i++){
    if(centroids[i].value != centroids[min_index].value){
      tmp_valAndVector.begin2endVector.x = centroids[i].centroids.x - centroids[min_index].centroids.x;
      tmp_valAndVector.begin2endVector.y = centroids[i].centroids.y - centroids[min_index].centroids.y;
      tmp_valAndVector.vectorSize = sqrt(pow(tmp_valAndVector.begin2endVector.x ,2.0) + pow(tmp_valAndVector.begin2endVector.y, 2.0));

      tmp_valAndVector.beginXY.x = centroids[min_index].centroids.x;
      tmp_valAndVector.beginXY.y = centroids[min_index].centroids.y;

      tmp_valAndVector.beginValue = centroids[min_index].value;
      tmp_valAndVector.endValue = centroids[i].value;
      valAndVector.push_back(tmp_valAndVector);
    }else{
      tmp_valAndVector.begin2endVector.x = 0;
      tmp_valAndVector.begin2endVector.y = 0;
      tmp_valAndVector.vectorSize = sqrt(pow(tmp_valAndVector.begin2endVector.x ,2.0) + pow(tmp_valAndVector.begin2endVector.y, 2.0));

      tmp_valAndVector.beginXY.x = centroids[min_index].centroids.x;
      tmp_valAndVector.beginXY.y = centroids[min_index].centroids.y;

      tmp_valAndVector.beginValue = centroids[min_index].value;
      tmp_valAndVector.endValue = centroids[min_index].value;
      valAndVector.push_back(tmp_valAndVector);
    }
  }
  vector<Centroids> relativeCent = *p_relative_centroids;
  vector<Featurepoints> relativeFP = *p_relative_featurepoint;

  Featurepoints tmp_relativeFP;
  Centroids tmp_relativeCent;


  //求めたベクトルの始点と終点のラベルが持つ重心と特徴点を始点からの相対位置(ベクトル)に変換
  for(int i = 0; i < valAndVector.size(); i++){
    for(int k = 0; k < centroids.size(); k++){
      if(valAndVector[i].beginValue == centroids[k].value){
        tmp_relativeCent.value = valAndVector[i].beginValue;
        tmp_relativeCent.calib_centroids.x = 0;
        tmp_relativeCent.calib_centroids.y = 0;
        tmp_relativeCent.centroids.x = centroids[k].centroids.x;
        tmp_relativeCent.centroids.y = centroids[k].centroids.y;
        relativeCent.push_back(tmp_relativeCent);
      }
      else{
        tmp_relativeCent.value = valAndVector[i].endValue;
        tmp_relativeCent.calib_centroids.x = centroids[k].centroids.x - valAndVector[i].beginXY.x;
        tmp_relativeCent.calib_centroids.y = centroids[k].centroids.y - valAndVector[i].beginXY.y;
        tmp_relativeCent.centroids.x = centroids[k].centroids.x;
        tmp_relativeCent.centroids.y = centroids[k].centroids.y;
        relativeCent.push_back(tmp_relativeCent);
      }
    }
    for(int j = 0; j < featurepoint.size(); j++){
      if(valAndVector[i].beginValue == featurepoint[j].value){
        tmp_relativeFP.value = featurepoint[j].value;
        tmp_relativeFP.calib_coordinate.x = featurepoint[j].coordinate.x - valAndVector[j].beginXY.x;
        tmp_relativeFP.calib_coordinate.y = featurepoint[j].coordinate.y - valAndVector[j].beginXY.y;
        tmp_relativeFP.coordinate.x = featurepoint[j].coordinate.x;
        tmp_relativeFP.coordinate.y = featurepoint[j].coordinate.y;

        tmp_relativeFP.boundary = featurepoint[j].boundary;
        tmp_relativeFP.ave_keypoint = featurepoint[j].ave_keypoint;
        tmp_relativeFP.mean_vector.x = featurepoint[j].mean_vector.x;
        tmp_relativeFP.mean_vector.y = featurepoint[j].mean_vector.y;
        tmp_relativeFP.min_label_word = featurepoint[j].min_label_word;
        tmp_relativeFP.one_dimention_scanning = featurepoint[j].one_dimention_scanning;

        relativeFP.push_back(tmp_relativeFP);
      }
      else{
        tmp_relativeFP.value = featurepoint[j].value;
        tmp_relativeFP.calib_coordinate.x = featurepoint[j].coordinate.x - valAndVector[j].beginXY.x;
        tmp_relativeFP.calib_coordinate.y = featurepoint[j].coordinate.y - valAndVector[j].beginXY.y;
        tmp_relativeFP.coordinate.x = featurepoint[j].coordinate.x;
        tmp_relativeFP.coordinate.y = featurepoint[j].coordinate.y;
        tmp_relativeFP.boundary = featurepoint[j].boundary;
        tmp_relativeFP.ave_keypoint = featurepoint[j].ave_keypoint;
        tmp_relativeFP.mean_vector.x = featurepoint[j].mean_vector.x;
        tmp_relativeFP.mean_vector.y = featurepoint[j].mean_vector.y;
        tmp_relativeFP.min_label_word = featurepoint[j].min_label_word;
        tmp_relativeFP.one_dimention_scanning = featurepoint[j].one_dimention_scanning;

        relativeFP.push_back(tmp_relativeFP);
      }
    }
  }

  *p_relative_centroids = relativeCent;
  *p_relative_featurepoint = relativeFP;

}
/*
---------------------------------------------------------------------------------------------------
検出した特徴点の特徴量を記述する
画像上のラベルの内特徴点にはどのラベルがあるかでバイナリを作成
---------------------------------------------------------------------------------------------------
*/
vector<TopologyFeature::keypoint> TopologyFeature::featureDescription(vector<Featurepoints> featurepoint, Mat label_img){

  double maxVal;
  minMaxLoc(label_img, NULL, &maxVal);
  vector<int> one_d_dst(maxVal, 0);

  //for(int i = 0; i < one_d_dst.size(); i++){
    //one_d_dst[i] = i;
  //}
  //vector<vector<int> > keypoint_binary(featurepoint.size(), vector<int>(one_d_dst.size(), 0));
  vector<keypoint> keypoint_binary;
  keypoint tmp_keypoint_binary;

  for(int y = 0; y < featurepoint.size(); y++){

    for(int i = 0; i < one_d_dst.size(); i++){
      tmp_keypoint_binary.binary.push_back(0);
    }
    tmp_keypoint_binary.xy.x = featurepoint[y].coordinate.x;
    tmp_keypoint_binary.xy.y = featurepoint[y].coordinate.y;
    for(int x = 0; x < featurepoint[y].one_dimention_scanning.size(); x++){
      int sum_scanning_yx = featurepoint[y].one_dimention_scanning[x];
      if(tmp_keypoint_binary.binary[sum_scanning_yx] == 0){
        tmp_keypoint_binary.binary[sum_scanning_yx] = 1;
      }
    }
    keypoint_binary.push_back(tmp_keypoint_binary);
    tmp_keypoint_binary.binary.erase(tmp_keypoint_binary.binary.begin(), tmp_keypoint_binary.binary.end());
  }

/*
  cout << "keypoint binary -----------"<<endl;
  for (int i = 0; i < keypoint_binary.size(); i++){
    cout << "x = " << keypoint_binary[i].xy.x << " y = " << keypoint_binary[i].xy.y << endl;
    for(int j = 0; j < keypoint_binary[i].binary.size(); j++){
      cout << keypoint_binary[i].binary[j] ;
    }
    cout << endl;
  }
*/
  return keypoint_binary;

}

/*
---------------------------------------------------------------------------------------------------
input画像用の関数
templateのヒストグラムの平均値と標準偏差にinputが成るように正規化
---------------------------------------------------------------------------------------------------
*/
Mat TopologyFeature::inputCreateLabelImg(Mat input_hsv){
  int input_y = input_hsv.rows;
  int input_x = input_hsv.cols;

  vector<Mat> planes;
  Mat input_v_value = Mat(input_y, input_x, CV_8UC1);
  vector<int> input_v_hist(255, 0);

  split(input_hsv, planes);
  input_v_value = planes[2];

  //equalizeHist(input_v_value, input_v_value);
  string template_v_list = "template_img_out/template_v_count_list.csv";
  vector<int> template_v_hist;
  template_v_hist = oned_intCsvReader(template_v_list);

  for(int y = 0; y < input_y; y++ ){
    for (int x = 0; x < input_x; x++){
      int v = input_v_value.at<unsigned char>(y,x);
      input_v_hist[v] += 1;
    }
  }
  string input_v_count_path = "input_img_out/input_v_count_list.csv";
  oned_intCsvWriter(input_v_hist, input_v_count_path);

  //for gnuplot
  string input_v_path = "graphPlot/input_v_count_list.csv";
  oned_vertical_intCsvWriter(input_v_hist, input_v_path);


  float sum_t = 0;
  float sum_i = 0;
  float sum_vt = 0;
  float sum_vi = 0;
  float sum_o_numerator = 0;
  float sum_oi_numerator = 0;

  for(int t = 0; t < template_v_hist.size(); t++){
    sum_vt += template_v_hist[t] * t;
    sum_t += template_v_hist[t];
  }
  float ave_t;
  if(sum_t < 0){
     ave_t = 0;
  }else{
     ave_t = sum_vt / sum_t;
  }
  for(int tt = 0; tt < template_v_hist.size(); tt++){
    sum_o_numerator += template_v_hist[tt] * pow(tt - ave_t, 2.0);
  }
  float ot;
  if(sum_t < 0){
     ot = 0;
  }else{
     ot = sum_o_numerator / sum_t;
  }
  float template_std_deviation = sqrt(ot);

  for(int i = 0; i < input_v_hist.size(); i++){
    sum_vi += input_v_hist[i] * i;
    sum_i += input_v_hist[i];
  }
  float ave_i;
  if(sum_i < 0){
     ave_i = 0;
  }else{
     ave_i = sum_vi / sum_i;
  }
  for(int ii = 0; ii < input_v_hist.size(); ii++){
    sum_oi_numerator += input_v_hist[ii] * pow(ii - ave_i, 2.0);
  }
  float oi;
  if(sum_i < 0){
     oi = 0;
  }else{
     oi = sum_oi_numerator / sum_i;
  }
  float input_std_deviation = sqrt(oi);

  cout << "-----------------confirmation average and std_deviation-----------------------------" << endl;
  cout << "===============std_deviation=============" << endl;
  cout << "template = " << template_std_deviation << " input = " << input_std_deviation << endl;
  cout << "===============average==============" << endl;
  cout << "template = " << ave_t << " input = " << ave_i << endl;

  vector<int> correct_input_v_hist(255, 0);
  float corre_input;
  for(int iii = 0; iii < correct_input_v_hist.size(); iii++){
    corre_input = template_std_deviation * ((input_v_hist[iii] - ave_i) / input_std_deviation) + ave_t;
    if(corre_input < 0){
      correct_input_v_hist[iii] = 0;
    }else{
      correct_input_v_hist[iii] = corre_input;
    }
  }

  string v_correct_path = "input_img_out/correct_input_v_count_list.csv";
  oned_intCsvWriter(correct_input_v_hist, v_correct_path);

  //for gnuplot
  string v_correct_vertical_path = "graphPlot/correct_input_v_count_list.csv";
  oned_vertical_intCsvWriter(correct_input_v_hist, v_correct_vertical_path);

  Mat label_input_img = Mat(input_y, input_x, CV_8UC1);

  int tmp_range = 20;
  int flag = 1;
  label_input_img = template_splitRegion(tmp_range, input_hsv, correct_input_v_hist, flag);

  return label_input_img;
}
/*
---------------------------------------------------------------------------------------------------
相対位置の重心とそのラベル値の特徴点のtemplate画像とinput画像間でのシミラリティを計算するためのベクトルを計算
---------------------------------------------------------------------------------------------------
*/
vector<TopologyFeature::cent2feature> TopologyFeature::calc_relative_centroids2featurepointVector(vector<Centroids> template_relative_centroids, vector<Featurepoints> template_relative_featurepoint){

  vector<cent2feature> template_vector;
  cent2feature tmp_template_vector;

  for(int i = 0; i < template_relative_centroids.size(); i++){
    for(int j = 0; j < template_relative_featurepoint.size(); j++){
      if(template_relative_centroids[i].value == template_relative_featurepoint[j].value){
        tmp_template_vector.value = template_relative_centroids[i].value;
        tmp_template_vector.vectorC2F.x = template_relative_featurepoint[j].calib_coordinate.x - template_relative_centroids[i].calib_centroids.x;
        tmp_template_vector.vectorC2F.y = template_relative_featurepoint[j].calib_coordinate.y - template_relative_centroids[i].calib_centroids.y;
        tmp_template_vector.pure_featurepoint.x = template_relative_featurepoint[j].coordinate.x;
        tmp_template_vector.pure_featurepoint.y = template_relative_featurepoint[j].coordinate.y;
        tmp_template_vector.vectorSize = sqrt(pow(tmp_template_vector.vectorC2F.x, 2.0) + pow(tmp_template_vector.vectorC2F.y, 2.0));
        template_vector.push_back(tmp_template_vector);
      }
    }
  }

  return template_vector;
}
/*
---------------------------------------------------------------------------------------------------
特徴点をマッチング
結果画像を出力
---------------------------------------------------------------------------------------------------
*/
void TopologyFeature::featureMatching(Mat template_img, Mat input_img, vector<keypoint> template_keypoint_binary, vector<keypoint> input_keypoint_binary,vector<cent2feature> template_vector, vector<cent2feature> input_vector){
  int input_y = input_img.rows;
  int input_x = input_img.cols;

  int template_y = template_img.rows;
  int template_x = template_img.cols;

  Mat sum_img = Mat(input_y, input_x + template_x, CV_8UC3);
  hconcat(input_img, template_img, sum_img);

  int numerator;
  double sizes;
  double simi_cos;

  int h = 0;
  //inlier and outlier matching
  DMatch tmp_goodMatch;
  vector<DMatch> goodMatch;

  //match points
  KeyPoint tmp_template_pt;
  KeyPoint tmp_input_pt;
  vector<Point2f> template_pt;
  vector<Point2f> input_pt;

  matchPoint tmp_match_point;
  vector<matchPoint> match_point;

  for(int x = 0; x < template_keypoint_binary.size(); x++){
    for(int y = 0; y < input_keypoint_binary.size(); y++){
      for(int z = 0; z < template_keypoint_binary[x].binary.size(); z++){
        h += template_keypoint_binary[x].binary[z] - input_keypoint_binary[y].binary[z];
        if(h != 0){
          break;
        }
      }
      if(h == 0){
        /*
              circle(sum_img, Point(input_x + template_keypoint_binary[x].xy.x, template_keypoint_binary[x].xy.y), 2, Scalar(200), 1, 4);
              circle(sum_img, Point(input_keypoint_binary[y].xy.x, input_keypoint_binary[y].xy.y), 2, Scalar(200), 1, 4);
              line(sum_img, Point(input_x + template_keypoint_binary[x].xy.x, template_keypoint_binary[x].xy.y), Point(input_keypoint_binary[y].xy.x, input_keypoint_binary[y].xy.y), Scalar(200), 1, 4 );

              //Homography matrix estimation
              tmp_template_pt.pt.x = template_keypoint_binary[x].xy.x;
              tmp_template_pt.pt.y = template_keypoint_binary[x].xy.y;
              template_pt.push_back(tmp_template_pt.pt);

              tmp_input_pt.pt.x = input_keypoint_binary[y].xy.x;
              tmp_input_pt.pt.y = input_keypoint_binary[y].xy.y;
              input_pt.push_back(tmp_input_pt.pt);

              //tmp_goodMatch.queryIdx = (int)template_yx[j][1];
              //tmp_goodMatch.trainIdx = (int)sum_xy[sums][1];
              goodMatch.push_back(tmp_goodMatch);
              */
              tmp_match_point.template_match.x = template_keypoint_binary[x].xy.x;
              tmp_match_point.template_match.y = template_keypoint_binary[x].xy.y;
              tmp_match_point.input_match.x = input_keypoint_binary[y].xy.x;
              tmp_match_point.input_match.y = input_keypoint_binary[y].xy.y;
              match_point.push_back(tmp_match_point);
      }
      h = 0;
    }
  }
  for(int l = 0; l < match_point.size(); l++){
    for(int i = 0; i < template_vector.size(); i++){
        for(int j = 0; j < input_vector.size(); j++){
            numerator = template_vector[i].vectorC2F.x * input_vector[j].vectorC2F.x + template_vector[i].vectorC2F.y * input_vector[j].vectorC2F.y;
            sizes = template_vector[i].vectorSize * input_vector[j].vectorSize;
            if(sizes >= 0){
              simi_cos = numerator / sizes;
            }else{
              simi_cos = 0;
            }
            //cout << "simi cos = " << simi_cos << endl;
            if(match_point[l].template_match.x == template_vector[i].pure_featurepoint.x
              && match_point[l].template_match.y == template_vector[i].pure_featurepoint.y
              && match_point[l].input_match.x == input_vector[j].pure_featurepoint.x
              && match_point[l].input_match.y == input_vector[j].pure_featurepoint.y){
            if(simi_cos == 1){
              circle(sum_img, Point(input_x + template_vector[i].pure_featurepoint.x, template_vector[i].pure_featurepoint.y), 2, Scalar(200), 1, 4);
              circle(sum_img, Point(input_vector[j].pure_featurepoint.x, input_vector[j].pure_featurepoint.y), 2, Scalar(200), 1, 4);
              line(sum_img, Point(input_x + template_vector[i].pure_featurepoint.x, template_vector[i].pure_featurepoint.y), Point(input_vector[j].pure_featurepoint.x, input_vector[j].pure_featurepoint.y), Scalar(200), 1, 4 );

              //Homography matrix estimation
              tmp_template_pt.pt.x = template_vector[i].pure_featurepoint.x;
              tmp_template_pt.pt.y = template_vector[i].pure_featurepoint.y;
              template_pt.push_back(tmp_template_pt.pt);

              tmp_input_pt.pt.x = input_vector[j].pure_featurepoint.x;
              tmp_input_pt.pt.y = input_vector[j].pure_featurepoint.y;
              input_pt.push_back(tmp_input_pt.pt);

              //tmp_goodMatch.queryIdx = (int)template_yx[j][1];
              //tmp_goodMatch.trainIdx = (int)sum_xy[sums][1];
              goodMatch.push_back(tmp_goodMatch);
            }
            }
        }
      }
  }

  imwrite("input_img_out/all_match_sum_img.tiff", sum_img);
  calc_Homography(template_pt, input_pt, goodMatch);
}
/*
---------------------------------------------------------------------------------------------------
Homographyを推定してマッチング率を算出（RANSACを使用）
---------------------------------------------------------------------------------------------------
*/
void TopologyFeature::calc_Homography(vector<Point2f> template_pt, vector<Point2f> input_pt, vector<DMatch> goodMatch){

  Mat masks;
  Mat H = findHomography(template_pt, input_pt, masks, RANSAC, 3);
  cout << "homography = " << H <<endl;

  //while(true){
    //imshow("goodmatch", goodMatch);
  //}
  //RANSACで使われた対応点のみ抽出
  vector<DMatch> inlinerMatch;
  for (size_t i = 0; i < masks.rows; ++i) {
    uchar *inliner = masks.ptr<uchar>(i);
    if (inliner[0] == 1) {
      inlinerMatch.push_back(goodMatch[i]);
    }
  }
  cout << "inlinerMatch count = "<< inlinerMatch.size() <<endl;
  cout << "allMatch count = " << goodMatch.size() << endl;
  double inliner = inlinerMatch.size();
  double good = goodMatch.size();
  double matching_ratio = inliner / good;
  cout << "matching ratio = " << matching_ratio << endl;
}

/*
---------------------------------------------------------------------------------------------------
出力されたcsvをいい感じに描画
---------------------------------------------------------------------------------------------------
*/
int TopologyFeature::graphPlot(){
  cout << "graph plotting ..." << endl;

  FILE *fp = popen("gnuplot", "w");
  if (fp == NULL)
  return -1;
  fputs("set datafile separator ','\n", fp);
  fputs("set terminal pdf\n", fp);
  fputs("set xrange [0:255]\n", fp);
  fputs("set yrange [0:2000]\n", fp);
  fputs("set y2range [0:8]\n", fp);
  fputs("set terminal png\n", fp);
  fputs("set ytics nomirror\n", fp);
  fputs("set y2tics\n", fp);
  fputs("set grid\n", fp);
  fputs("set title 'separability of template img'\n", fp);
  fputs("set ylabel 'histgram'\n", fp);
  fputs("set key top left\n", fp);
  //fputs("set style fill solid border lc rgb 'red'\n", fp);
  fputs("set output 'graphPlot/template_separability.pdf'\n", fp);
  fputs("plot 'graphPlot/template_v_count_list.csv' with boxes title 'histgram', 'graphPlot/template_separation_list.csv' with lines title 'separability' axes x1y2,  'graphPlot/label_list.csv' with lines title 'label list' axes x1y2 ,2/pi with lines title '2 / pi' axes x1y2 \n", fp);
  fputs("set output 'graphPlot/template_separability.png'\n", fp);
  fputs("plot 'graphPlot/template_v_count_list.csv' with boxes title 'histgram', 'graphPlot/template_separation_list.csv' with lines title 'separability' axes x1y2,  'graphPlot/label_list.csv' with lines title 'label list' axes x1y2 ,2/pi with lines title '2 / pi' axes x1y2 \n", fp);
  fflush(fp);
  pclose(fp);

  FILE *fp1 = popen("gnuplot", "w");
  if (fp1 == NULL)
  return -1;
  fputs("set datafile separator ','\n", fp1);
  fputs("set terminal pdf\n", fp1);
  fputs("set xrange [0:255]\n", fp1);
  fputs("set yrange [0:2000]\n", fp1);
  fputs("set y2range [0:8]\n", fp1);
  fputs("set terminal png\n", fp1);
  fputs("set ytics nomirror\n", fp1);
  fputs("set y2tics\n", fp1);
  fputs("set grid\n", fp1);
  fputs("set title 'separability of input img'\n", fp1);
  fputs("set ylabel 'histgram'\n", fp1);
  fputs("set key top left\n", fp1);
  //fputs("set style fill solid border lc rgb 'red'\n", fp1);
  fputs("set output 'graphPlot/input_separability.pdf'\n", fp1);
  fputs("plot 'graphPlot/input_v_count_list.csv' with boxes title 'histgram', 'graphPlot/input_separation_list.csv' with lines title 'separability' axes x1y2,  'graphPlot/input_label_list.csv' with lines title 'label list' axes x1y2 ,2/pi with lines title '2 / pi' axes x1y2 \n", fp1);
  fputs("set output 'graphPlot/input_separability.png'\n", fp1);
  fputs("plot 'graphPlot/input_v_count_list.csv' with boxes title 'histgram', 'graphPlot/input_separation_list.csv' with lines title 'separability' axes x1y2,  'graphPlot/input_label_list.csv' with lines title 'label list' axes x1y2 ,2/pi with lines title '2 / pi' axes x1y2 \n", fp1);
  fflush(fp1);
  pclose(fp1);


  FILE *fp2 = popen("gnuplot", "w");
  if (fp2 == NULL)
  return -1;
  fputs("set datafile separator ','\n", fp2);
  fputs("set terminal pdf\n", fp2);
  fputs("set xrange [0:255]\n", fp2);
  fputs("set yrange [0:2000]\n", fp2);
  fputs("set terminal png\n", fp2);
  fputs("set grid\n", fp2);
  fputs("set ylabel 'histgram'\n", fp2);
  fputs("set key top left\n", fp2);
  fputs("set title 'histgram of template, input, correct input'\n", fp2);
  fputs("set style fill solid border lc rgb 'red'\n", fp2);
  fputs("set output 'graphPlot/input_histgrams.pdf'\n", fp2);
  fputs("plot 'graphPlot/template_v_count_list.csv' with lines title 'template histgram', 'graphPlot/input_v_count_list.csv' with lines title 'input histgram', 'graphPlot/correct_input_v_count_list.csv' with lines title 'corrected input histgram' linecolor rgbcolor 'red' \n", fp2);
  fputs("set output 'graphPlot/input_histgrams.png'\n", fp2);
  fputs("plot 'graphPlot/template_v_count_list.csv' with lines title 'template histgram', 'graphPlot/input_v_count_list.csv' with lines title 'input histgram', 'graphPlot/correct_input_v_count_list.csv' with lines title 'corrected input histgram' linecolor rgbcolor 'red' \n", fp2);
  fflush(fp2);
  pclose(fp2);
  return 0;
}

/*
---------------------------------------------------------------------------------------------------
csvを読み込み(一次元のint型配列)
---------------------------------------------------------------------------------------------------
*/
vector<int> TopologyFeature::oned_intCsvReader(string filePath){
  ifstream file(filePath);
  vector<int> readCsv;
  string buffer;
  while(getline(file, buffer)){
    vector<int> record;
    istringstream stream(buffer);
    string token;
    while(getline(stream, token, ',')){
      int tmp = stoi(token);
      readCsv.push_back(tmp);
    }
  }
  return readCsv;
}

/*
---------------------------------------------------------------------------------------------------
csvを読み込み(二次元のint型配列)
---------------------------------------------------------------------------------------------------
*/
vector<vector<int> > TopologyFeature::twod_intCsvReader(string filePath){

  ifstream file(filePath);
  vector<vector<int> > readcsv;
  string buffer;
  while(getline(file, buffer)){
    vector<int> record;
    istringstream stream(buffer);
    string token;
    while(getline(stream, token, ',')){
      int tmp = stoi(token);
      record.push_back(tmp);
    }
    readcsv.push_back(record);
  }
  return readcsv;
}

/*
---------------------------------------------------------------------------------------------------
csvを読み込み(二次元のdouble型配列)
---------------------------------------------------------------------------------------------------
*/
vector<vector<double> > TopologyFeature::twod_doubleCsvReader(string filePath){

  ifstream file(filePath);
  vector<vector<double> > readcsv;
  string buffer;
  while(getline(file, buffer)){
    vector<double> record;
    istringstream stream(buffer);
    string token;
    while(getline(stream, token, ',')){
      double tmp = stoi(token);
      record.push_back(tmp);
    }
    readcsv.push_back(record);
  }
  return readcsv;
}

/*
---------------------------------------------------------------------------------------------------
csvを読み込み(二次元のfloat型配列)
---------------------------------------------------------------------------------------------------
*/
vector<vector<float> > TopologyFeature::twod_floatCsvReader(string filePath){

  ifstream file(filePath);
  vector<vector<float> > readcsv;
  string buffer;
  while(getline(file, buffer)){
    vector<float> record;
    istringstream stream(buffer);
    string token;
    while(getline(stream, token, ',')){
      float tmp = stoi(token);
      record.push_back(tmp);
    }
    readcsv.push_back(record);
  }
  return readcsv;
}


/*
---------------------------------------------------------------------------------------------------
csvを書き込み(一次元のint型配列)
---------------------------------------------------------------------------------------------------
*/
void TopologyFeature::oned_intCsvWriter(vector<int> file, string filePath){
  ofstream ofs(filePath);
  for(int i = 0; i < file.size(); i++){
    if(i == file.size() -1){
      ofs << file[i];
    }else{
      ofs << file[i] << ",";
    }
  }
  ofs << endl;

}

/*
---------------------------------------------------------------------------------------------------
csvを書き込み(一次元のfloat型配列)
---------------------------------------------------------------------------------------------------
*/
void TopologyFeature::oned_floatCsvWriter(vector<float> file, string filePath){

  ofstream ofs(filePath);
  for(int i = 0; i < file.size(); i++){
    if(i == file.size() -1){
      ofs << file[i];
    }else{
      ofs << file[i] << ",";
    }
  }
  ofs << endl;

}
/*
---------------------------------------------------------------------------------------------------
csvを書き込み(一次元のint型配列)
縦書き
---------------------------------------------------------------------------------------------------
*/
void TopologyFeature::oned_vertical_intCsvWriter(vector<int> file, string filePath){
  ofstream ofs(filePath);
  for(int i = 0; i < file.size(); i++){
      ofs << file[i];
      ofs << endl;
  }
}

/*
---------------------------------------------------------------------------------------------------
csvを書き込み(一次元のfloat型配列)
縦書き
---------------------------------------------------------------------------------------------------
*/
void TopologyFeature::oned_vertical_floatCsvWriter(vector<float> file, string filePath){

  ofstream ofs(filePath);
  float larger;
  for(int i = 0; i < file.size(); i++){
      //larger = file[i] *10;
      larger = file[i];
      ofs << larger;
      ofs << endl;
  }

}



/*
---------------------------------------------------------------------------------------------------
csvを書き込み(二次元のint型配列)
---------------------------------------------------------------------------------------------------
*/
void TopologyFeature::twod_intCsvWriter(vector<vector<int> > file, string filePath){

  ofstream ofs(filePath);
  for(int i = 0; i < file.size(); i++){
    for(int j = 0; j < file[0].size(); j ++){
      if(j == file[0].size()-1){
        ofs << file[i][j];
      }else{
        ofs << file[i][j] << ",";
      }
    }
    ofs << endl;
  }
  ofs << endl;
}

/*
---------------------------------------------------------------------------------------------------
csvを書き込み(二次元のdouble型配列)
---------------------------------------------------------------------------------------------------
*/
void TopologyFeature::twod_doubleCsvWriter(vector<vector<double> > file, string filePath){

  ofstream ofs(filePath);
  for(int i = 0; i < file.size(); i++){
    for(int j = 0; j < file[0].size(); j ++){
      if(j == file[0].size()-1){
        ofs << file[i][j];
      }else{
        ofs << file[i][j] << ",";
      }
    }
    ofs << endl;
  }
  ofs << endl;
}

/*
---------------------------------------------------------------------------------------------------
csvを書き込み(二次元のdouble型配列)
---------------------------------------------------------------------------------------------------
*/
void TopologyFeature::twod_floatCsvWriter(vector<vector<float> > file, string filePath){

  ofstream ofs(filePath);
  for(int i = 0; i < file.size(); i++){
    for(int j = 0; j < file[0].size(); j ++){
      if(j == file[0].size()-1){
        ofs << file[i][j];
      }else{
        ofs << file[i][j] << ",";
      }
    }
    ofs << endl;
  }
  ofs << endl;
}
