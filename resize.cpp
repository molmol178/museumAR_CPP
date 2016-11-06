#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char *argv[]){
  cv::Mat src_img = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
  if(src_img.empty()) return -1;
  cv::Mat dst_img1;
  cv::Mat dst_img2;
  cv::Mat dst_img3;
  cv::Mat dst_img4;

  // INTER_AREA（http://opencv.jp/cookbook/opencv_img.html）でのサイズ変更
  cv::resize(src_img, dst_img1, cv::Size(), 0.75, 0.75,cv::INTER_AREA);
  cv::resize(src_img, dst_img2, cv::Size(), 0.5, 0.5,cv::INTER_AREA);
  cv::resize(src_img, dst_img3, cv::Size(), 0.25, 0.25,cv::INTER_AREA);
  cv::resize(src_img, dst_img4, cv::Size(), 0.1, 0.1,cv::INTER_AREA);

  imwrite("resized/resized_75.png", dst_img1);
  imwrite("resized/resized_50.png", dst_img2);
  imwrite("resized/resized_25.png", dst_img3);
  imwrite("resized/resized_10.png", dst_img4);
}
