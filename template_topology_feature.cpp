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

Mat template_splitRegion(int tmp_range, Mat template_hsv);
Mat re_label(Mat label_template_img);
Mat cleanLabelImage(Mat dst_data, int patch_size);

Mat writeDstData(Mat clean_label_img);
void featureDetection(int patch_size, Mat label_template_img, vector<vector<int> > *sum_one_dimention_scanning, vector<vector<int> > *sum_xy, vector<vector<int> > *sum_boundary, vector<vector<double> > *sum_ave_keypoint);
void saveFeaturePoint (int x, int y, vector<int> one_dimention_scanning, vector<int> word_list, int tmp_boundary, vector<vector<int> > *sum_one_dimention_scanning, vector<vector<int> > *sum_xy, vector<vector<int> > *sum_boundary, vector<vector<double> > *sum_ave_keypoint);
void writeFeaturePoint(Mat template_img, vector<vector<int> > *sum_one_dimention_scanning, vector<vector<int> > *sum_xy, vector<vector<int> > *sum_boundary, vector<vector<double> > *sum_ave_keypoint);


int main(int argc, char** argv){
  Mat template_img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  Mat Gaussian_template;
  Mat template_hsv;
  Mat label_template_img;
  Mat dst_data;
  Mat clean_label_img;
  Mat changed_label_img;

  //コマンドライン引数
  int patch_size;
  if(argv[2] == NULL){
    cout << "please set argv[2] about patch size. defalt is 7" <<endl;
    patch_size = 7;
  }else{
    patch_size = atoi(argv[2]);
    cout << "patch_size = "<<patch_size <<endl;
  }
  int tmp_range;
  if(argv[3] == NULL){
    cout << "please set argv[3] about tmp_range. defalt is 30" << endl;
    tmp_range = 30;
  }else{
    tmp_range = atoi(argv[3]);
    cout << "tmp_range = " << tmp_range << endl;
  }

//featureDetection variables
  vector<vector<int> > sum_one_dimention_scanning;
  vector<vector<int> > sum_xy;
  vector<vector<int> > sum_boundary;
  vector<vector<double> > sum_ave_keypoint;

  //ガウシアンブラー
  GaussianBlur(template_img, Gaussian_template, Size(5,5), 1.5, 1.5);
  cvtColor(Gaussian_template, template_hsv, CV_BGR2HSV);
  //cvtColor(template_img, template_hsv, CV_BGR2HSV);



  label_template_img = template_splitRegion(tmp_range, template_hsv);
  //imwrite("label_template_img.jpg", label_template_img);

  dst_data = re_label(label_template_img);
  //unsigned short画像の領域をunsigned charのランダムな値で埋める
  //changed_label_img = writeDstData(dst_data);

  clean_label_img = cleanLabelImage(dst_data, patch_size);
  cout << "end label_img cleaning" << endl;

  //unsigned short画像の領域をunsigned charのランダムな値で埋める
  //ラベル画像確認用
  changed_label_img = writeDstData(clean_label_img);
  imwrite("changed_label_img.tiff", changed_label_img);

  cout << "feature detection" << endl;
  featureDetection(patch_size, clean_label_img, &sum_one_dimention_scanning, &sum_xy, &sum_boundary, &sum_ave_keypoint);
/*
  cout << "===============================================================" << endl;
  for (int i = 0; i < sum_one_dimention_scanning.size(); i++){
    cout << "sum_one_dimention_sacnning = ";
    for (int j = 0; j < sum_one_dimention_scanning[0].size(); j++){
      cout << sum_one_dimention_scanning[i][j] << ", ";
    }
    cout << endl;
    cout << "sum_xy = ";
    for (int a = 0; a < sum_xy[0].size(); a++){
      cout << sum_xy[i][a] << ", ";
    }
    cout << endl;
    for (int a = 0; a < sum_boundary[0].size(); a++){
      cout << "sum_boundary = " << sum_boundary[i][a]  << ", ";
    }
    cout << endl;
    for (int a = 0; a < sum_ave_keypoint[0].size(); a++){
      cout << "sum_ave_keypoint = " << sum_ave_keypoint[i][a] << ", ";
    }
    cout << endl;
    cout << "============================================" << endl;
  }
  cout << "finish feature detection" << endl;
*/
  writeFeaturePoint(template_img, &sum_one_dimention_scanning, &sum_xy, &sum_boundary, &sum_ave_keypoint);
  return 0;
}

Mat template_splitRegion(int separate_range, Mat template_hsv){
  int template_x = template_hsv.cols;
  int template_y = template_hsv.rows;
  vector<Mat> planes;
  Mat v_value = Mat(template_y, template_x, CV_8UC1);
  vector<int> v_count_list(256, 0);

  split(template_hsv, planes);
  v_value = planes[2];

  for(int y = 0; y < template_y; y++ ){
    for (int x = 0; x < template_x; x++){
      int v = v_value.at<unsigned char>(y,x);
      v_count_list[v] += 1;
    }
  }
  ofstream of_v_count("v_count_list.csv");
  for(int i = 0; i < v_count_list.size(); i++){
    of_v_count << v_count_list[i] << ",";
  }
  of_v_count << endl;
  cout << "output v_count_list" <<endl;

  vector<float> separation_list(256,0);
  int tmp_range = separate_range;
  for(int i = 0; i < tmp_range; i++){
    separation_list[i] = 0;
  }
  for(int i = 256 - tmp_range; i < separation_list.size(); i++){
    separation_list[i] = 0;
  }

  for(int i = 0 + tmp_range; i < 256 - tmp_range; i++){
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
  cout << "separation_list" <<endl;
  for(int i = 0; i < separation_list.size(); i++){
    cout << separation_list[i] << ", ";
  }
  cout << endl;
  cout << endl;
  cout << endl;

  ofstream of_sep("separation_list.csv");
  for(int i = 0; i < separation_list.size(); i++){
    of_sep << separation_list[i] << ",";
  }
  of_sep << endl;
  cout << "output separation_list" <<endl;


  vector<int> label_list(256,0);
  vector<float> tmp_sep_list;
  vector<int> sep_index;
  vector<int> sep_max_index;
  int b = 1;
  for(int a = 0 + 1; a < 256 - 1; a++){
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
        b ++;
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

  ofstream of_label("label_list.csv");
  for(int i = 0; i < label_list.size(); i++){
    of_label << label_list[i] << ",";
  }
  of_label << endl;
  cout << "output label_list" <<endl;

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
  //ofstream ofs("dst_data.csv");
  //ofs << dst_data << endl;

  return dst_data;
}

Mat cleanLabelImage(Mat dst_data, int patch_size){

  int x_size = dst_data.cols;
  int y_size = dst_data.rows;
  //距離画像
  Mat tmp_dst_data = Mat::zeros(y_size, x_size, CV_8UC1);

  //dst_dataの最大値を求める(maxVal)
  double maxVal;
  minMaxLoc(dst_data, NULL, &maxVal);

  //maxValの大きさの配列を作る
  vector<unsigned short> label_list(maxVal, 0);
  unsigned short label;

  for(int y = 0; y < y_size; y++){
    for(int x = 0; x < x_size; x++){
      label = dst_data.at<unsigned short>(y, x);
      label_list[label] += 1;
    }
  }

  //パッチサイズよりも小さい領域を0にする
  for(int y = 0; y < y_size; y++){
    for(int x = 0; x < x_size; x++){
      unsigned short dst_xy = dst_data.at<unsigned short>(y, x);
      if (label_list[dst_xy] < patch_size/2 * patch_size/2 * M_PI){
        dst_data.at<unsigned short>(y,x) = 0;
      }
    }
  }
  Mat cleaned_dst_data;
  cleaned_dst_data = writeDstData(dst_data);
  imwrite("cleaned_dst_data.tiff" , cleaned_dst_data );
  //ofstream of_zero("dst_data_in_cleanLanelImage0.csv");
  //of_zero << dst_data << endl;



  //１回目のスキャン．左上から右下へ
  for(int y = 1; y < y_size; y++){
    for(int x = 1; x < x_size; x++){

      if(dst_data.at<unsigned short>(y, x) == 0){
        unsigned short dst_xy = dst_data.at<unsigned short>(y, x);
        int tmp_dst_xy = tmp_dst_data.at<int>(y, x);
        unsigned short dst_up = dst_data.at<unsigned short>(y-1, x);
        unsigned short dst_left = dst_data.at<unsigned short>(y, x-1);
        int tmp_dst_up = tmp_dst_data.at<int>(y-1, x);
        int tmp_dst_left = tmp_dst_data.at<int>(y, x-1);

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
  //ofstream of_one("dst_data_in_cleanLanelImage1.csv");
  //of_one << dst_data << endl;
  //cout << dst_data << endl;

  //２回目のスキャン．右下から左上へ
  for(int y = y_size - 2; y > 1; y--){
    for(int x = x_size - 2; x > 1; x--){

      if(tmp_dst_data.at<int>(y, x) > 0){
        unsigned short dst_xy = dst_data.at<unsigned short>(y, x);
        int tmp_dst_xy = tmp_dst_data.at<int>(y, x);
        unsigned short dst_down = dst_data.at<unsigned short>(y+1, x);
        unsigned short dst_right = dst_data.at<unsigned short>(y, x+1);
        int tmp_dst_down = tmp_dst_data.at<int>(y+1, x);
        int tmp_dst_right = tmp_dst_data.at<int>(y, x+1);

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
  //ofstream of_two("dst_data_in_cleanLanelImage2.csv");
  //of_two << dst_data << endl;

  cout << "end clean_label_img out" << endl;


  return dst_data;
}

Mat writeDstData(Mat clean_label_img){
  int y_size = clean_label_img.rows;
  int x_size = clean_label_img.cols;
  Mat changed_label_img = Mat(y_size, x_size, CV_8UC1);

  double maxVal;
  minMaxLoc(clean_label_img, NULL, &maxVal);

  vector<unsigned short> label_list(maxVal, 0);
  unsigned short label;


  for(int y = 0; y < y_size; y++){
    for(int x = 0; x < x_size; x++){
      label = clean_label_img.at<unsigned short>(y, x);
      label_list[label] += 1;
    }
  }

  for(int i = 0; i < label_list.size(); i++){
    int lucky = 1 + rand() % (256 - 1);
    for(int y = 0; y < y_size; y++){
      for(int x = 0; x < x_size; x++){
        if(label_list[i] == 0){
          break;
          //cout << "zero exists!!!!!!!!!!!!!!!!" << endl;
        }else{
          if(clean_label_img.at<unsigned short>(y, x) == i){
            changed_label_img.at<unsigned char>(y, x) = lucky;
          }
        }
      }
    }
  }
  return changed_label_img;
}
void featureDetection(int patch_size, Mat label_template_img, vector<vector<int> > *sum_one_dimention_scanning, vector<vector<int> > *sum_xy, vector<vector<int> > *sum_boundary, vector<vector<double> > *sum_ave_keypoint){

  vector<vector<int> > this_sum_one_dimention_scanning = *sum_one_dimention_scanning;
  vector<vector<int> > this_sum_xy = *sum_xy;
  vector<vector<int> > this_sum_boundary = *sum_boundary;
  vector<vector<double> > this_sum_ave_keypoint = *sum_ave_keypoint;

  int x_size = label_template_img.cols;
  int y_size = label_template_img.rows;
  int n = patch_size;
  int one_n = 16;


  Mat scanning_filter(n, n, CV_16U);
  int scan_x = scanning_filter.cols;
  int scan_y = scanning_filter.rows;


  for(int y = 0; y < y_size - scan_y; y += patch_size/2){
    for(int x = 0; x < x_size - scan_x; x += patch_size/2){
      for(int s_y = 0; s_y < scan_y; s_y++){
        for(int s_x = 0; s_x < scan_x; s_x++){
          scanning_filter.at<unsigned short>(s_y, s_x) = label_template_img.at<unsigned short>(s_y + y, s_x + x);
        }
      }
          int one_d_data[] = {scanning_filter.at<unsigned short>(0,3),scanning_filter.at<unsigned short>(0,4),scanning_filter.at<unsigned short>(1,5),scanning_filter.at<unsigned short>(2,6),scanning_filter.at<unsigned short>(3,6),scanning_filter.at<unsigned short>(4,6),scanning_filter.at<unsigned short>(5,5),scanning_filter.at<unsigned short>(6,4),scanning_filter.at<unsigned short>(6,3),scanning_filter.at<unsigned short>(6,2),scanning_filter.at<unsigned short>(5,1),scanning_filter.at<unsigned short>(4,0),scanning_filter.at<unsigned short>(3,0),scanning_filter.at<unsigned short>(2,0),scanning_filter.at<unsigned short>(1,1),scanning_filter.at<unsigned short>(0,2)};

          vector<int> one_dimention_scanning(one_d_data,end(one_d_data));
          int scanning_center = {scanning_filter.at<unsigned short>(3,3)};
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

            /*
            cout << "-------------------one_d_lists----------------------" << endl;
            cout << "scanning_center = " << scanning_center << endl;
            cout << "word_list.size() = " << word_list.size() << endl;
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
            cout << "min word_list = " << word_list[cnt_minIndex] << endl;
            cout << "one_dimention_scanning[0] = " << one_dimention_scanning[0] << endl;
            cout << "one_dimention_scanning[15] = " << one_dimention_scanning[15] << endl;
            */

            //要素数が2,短い方のラベルが中心画素と同じ
            if(one_d_count == 1){
              if(word_list.size() == 2 && scanning_center == word_list[cnt_minIndex]){
                //最も小さいラベルが120度以下
                if(min_cnt <= one_n / 3){
                  //word_listの合計を求める
                  int tmp_boundary = 2;
                  saveFeaturePoint(x, y, one_dimention_scanning, word_list, tmp_boundary, &this_sum_one_dimention_scanning, &this_sum_xy, &this_sum_boundary, &this_sum_ave_keypoint);
                }
              }
            }
            else if(one_d_count == 2){
              if(word_list.size() == 3){
                int tmp_boundary = 3;
                saveFeaturePoint(x, y, one_dimention_scanning,word_list, tmp_boundary, &this_sum_one_dimention_scanning, &this_sum_xy, &this_sum_boundary, &this_sum_ave_keypoint);
                }
              if (one_dimention_scanning[0] == one_dimention_scanning[15] && scanning_center == word_list[cnt_minIndex]){
                //最も小さいラベルが120度以下
                if(min_cnt <= one_n / 3){
                int tmp_boundary = 2;
                saveFeaturePoint(x, y, one_dimention_scanning,word_list, tmp_boundary, &this_sum_one_dimention_scanning, &this_sum_xy, &this_sum_boundary, &this_sum_ave_keypoint);
                }
              }
          }
          else if(one_d_count == 3){
              if (one_dimention_scanning[0] == one_dimention_scanning[15]){
                //最も小さいラベルが120度以下
                int tmp_boundary = 3;
                saveFeaturePoint(x, y, one_dimention_scanning,word_list, tmp_boundary, &this_sum_one_dimention_scanning, &this_sum_xy, &this_sum_boundary, &this_sum_ave_keypoint);
              }
           }
          }
    }
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

  tmp_sum_xy.push_back(y+3);
  tmp_sum_xy.push_back(x+3);

  this_sum_xy.push_back(tmp_sum_xy);

  tmp_sum_boundary.push_back(tmp_boundary);
  this_sum_boundary.push_back(tmp_sum_boundary);

  //return
  *sum_one_dimention_scanning = this_sum_one_dimention_scanning;
  *sum_xy = this_sum_xy;
  *sum_boundary = this_sum_boundary;
  *sum_ave_keypoint = this_sum_ave_keypoint;
}

//カラー画像に特徴点をマッピング
void writeFeaturePoint(Mat template_img, vector<vector<int> > *sum_one_dimention_scanning, vector<vector<int> > *sum_xy, vector<vector<int> > *sum_boundary, vector<vector<double> > *sum_ave_keypoint){

   vector<vector<int> > this_sum_one_dimention_scanning = *sum_one_dimention_scanning;
  vector<vector<int> > this_sum_xy = *sum_xy;
  vector<vector<int> > this_sum_boundary = *sum_boundary;
  vector<vector<double> > this_sum_ave_keypoint = *sum_ave_keypoint;


  for(int i = 0; i < this_sum_xy.size(); i++){
    vector<int> center = this_sum_xy[i];
    vector<int> top_left(2,0);
    vector<int> bottom_right(2,0);

    for(int i = 0; i < center.size(); i++){
      top_left[i] = center[i] - 3;
      bottom_right[i] = center[i] + 3;
    }

    if(this_sum_boundary[i][0] == 2){
      template_img.at<Vec3b>(center[0],center[1])[0] = 255;
      template_img.at<Vec3b>(center[0],center[1])[1] = 0;
      template_img.at<Vec3b>(center[0],center[1])[2] = 0;
      rectangle(template_img, Point(top_left[1],top_left[0]), Point(bottom_right[1],bottom_right[0]), Scalar(255,0,0), 1);
    }
    else if(this_sum_boundary[i][0] == 3){
      template_img.at<Vec3b>(center[0],center[1])[0] = 0;
      template_img.at<Vec3b>(center[0],center[1])[1] = 255;
      template_img.at<Vec3b>(center[0],center[1])[2] = 0;
      rectangle(template_img, Point(top_left[1],top_left[0]), Point(bottom_right[1],bottom_right[0]), Scalar(0,255,0), 1);
    }
  }
  imwrite("detect_feature_point.jpg",template_img);
}
