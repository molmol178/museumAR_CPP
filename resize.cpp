#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char *argv[]){
  cv::Mat src_img = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
  cv::Mat src_img1 = cv::imread(argv[2], CV_LOAD_IMAGE_COLOR);
  cv::Mat src_img2 = cv::imread(argv[3], CV_LOAD_IMAGE_COLOR);
  cv::Mat src_img3 = cv::imread(argv[4], CV_LOAD_IMAGE_COLOR);
  cv::Mat src_img4 = cv::imread(argv[5], CV_LOAD_IMAGE_COLOR);
  cv::Mat src_img5 = cv::imread(argv[6], CV_LOAD_IMAGE_COLOR);
 
  if(src_img.empty()) return -1;
  cv::Mat dst_img2;
  cv::Mat dst_img3;
  cv::Mat dst_img4;

  cv::Mat dst_img12;
  cv::Mat dst_img13;
  cv::Mat dst_img14;

  cv::Mat dst_img22;
  cv::Mat dst_img23;
  cv::Mat dst_img24;

  cv::Mat dst_img32;
  cv::Mat dst_img33;
  cv::Mat dst_img34;

  cv::Mat dst_img42;
  cv::Mat dst_img43;
  cv::Mat dst_img44;

  cv::Mat dst_img52;
  cv::Mat dst_img53;
  cv::Mat dst_img54;

  cv::Mat dst_img62;
  cv::Mat dst_img63;
  cv::Mat dst_img64;


  // INTER_AREA（http://opencv.jp/cookbook/opencv_img.html）でのサイズ変更
  cv::resize(src_img, dst_img2, cv::Size(), 0.5, 0.5,cv::INTER_AREA);
  cv::resize(src_img, dst_img3, cv::Size(), 0.25, 0.25,cv::INTER_AREA);
  cv::resize(src_img, dst_img4, cv::Size(), 0.125, 0.125,cv::INTER_AREA);

  imwrite("resized/resized_50.png", dst_img2);
  imwrite("resized/resized_25.png", dst_img3);
  imwrite("resized/resized_12-5.png", dst_img4);

  cv::resize(src_img1, dst_img12, cv::Size(), 0.5, 0.5,cv::INTER_AREA);
  cv::resize(src_img1, dst_img13, cv::Size(), 0.25, 0.25,cv::INTER_AREA);
  cv::resize(src_img1, dst_img14, cv::Size(), 0.125, 0.125,cv::INTER_AREA);

  imwrite("resized/resized1_50.png", dst_img12);
  imwrite("resized/resized1_25.png", dst_img13);
  imwrite("resized/resized1_12-5.png", dst_img14);

  cv::resize(src_img2, dst_img22, cv::Size(), 0.5, 0.5,cv::INTER_AREA);
  cv::resize(src_img2, dst_img23, cv::Size(), 0.25, 0.25,cv::INTER_AREA);
  cv::resize(src_img2, dst_img24, cv::Size(), 0.125, 0.125,cv::INTER_AREA);

  imwrite("resized/resized2_50.png", dst_img22);
  imwrite("resized/resized2_25.png", dst_img23);
  imwrite("resized/resized2_12-5.png", dst_img24);

  cv::resize(src_img3, dst_img32, cv::Size(), 0.5, 0.5,cv::INTER_AREA);
  cv::resize(src_img3, dst_img33, cv::Size(), 0.25, 0.25,cv::INTER_AREA);
  cv::resize(src_img3, dst_img34, cv::Size(), 0.125, 0.125,cv::INTER_AREA);

  imwrite("resized/resized3_50.png", dst_img32);
  imwrite("resized/resized3_25.png", dst_img33);
  imwrite("resized/resized3_12-5.png", dst_img34);

  cv::resize(src_img4, dst_img42, cv::Size(), 0.5, 0.5,cv::INTER_AREA);
  cv::resize(src_img4, dst_img43, cv::Size(), 0.25, 0.25,cv::INTER_AREA);
  cv::resize(src_img4, dst_img44, cv::Size(), 0.125, 0.125,cv::INTER_AREA);

  imwrite("resized/resized4_50.png", dst_img42);
  imwrite("resized/resized4_25.png", dst_img43);
  imwrite("resized/resized4_12-5.png", dst_img44);

  cv::resize(src_img5, dst_img52, cv::Size(), 0.5, 0.5,cv::INTER_AREA);
  cv::resize(src_img5, dst_img53, cv::Size(), 0.25, 0.25,cv::INTER_AREA);
  cv::resize(src_img5, dst_img54, cv::Size(), 0.125, 0.125,cv::INTER_AREA);

  imwrite("resized/resized5_50.png", dst_img52);
  imwrite("resized/resized5_25.png", dst_img53);
  imwrite("resized/resized5_12-5.png", dst_img54);

}
