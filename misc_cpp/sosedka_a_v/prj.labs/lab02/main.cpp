/**
    Lab02 :
    @file main.cpp
    @author Artyom Sosedka
    @version 1.0
    @date 20/02/21
*/

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>
#include <numeric>
#include <iostream>

cv::Mat CompressImage(const cv::Mat& image, int compression) {
  std::vector<uchar> buffer;
  std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, compression};
  cv::imencode(".jpg", image, buffer, params);
  cv::Mat compressedImage = cv::imdecode(buffer, 1);
  return compressedImage;
}

cv::Mat GenerateHistogram(const cv::Mat& image, const int* channels) {
  int histSize = 256;
  float range[] = { 0, 256 };
  const float* histRange = { range };
  cv::Mat histogram;
  cv::calcHist(&image, 1, channels, cv::Mat(), histogram, 1, &histSize, &histRange, true, false);
  int hist_w = image.cols;
  int hist_h = image.rows;
  int bin_w = cvCeil( (double) hist_w/histSize );

  cv::normalize(histogram, histogram, 0, histogram.rows, cv::NORM_MINMAX, -1, cv::Mat() );

  cv::Mat histogramImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0x00,0x00,0x00) );

  for( int i = 0; i < histSize; i++ )
  {
    cv::line( histogramImage, cv::Point( bin_w*(i), 0),
          cv::Point( bin_w*(i), hist_h - cvRound(histogram.at<float>(i)) ),
          cv::Scalar( 0xff, 0xff, 0xff), 2, 8, 0);
  }

  return histogramImage;
}

std::vector<cv::Mat> GenerateCollages(const cv::Mat& image) {
  cv::Mat firstCollage, secondCollage;

  std::vector<cv::Mat> bgr_planes;
  cv::split(image, bgr_planes);

  cv::Mat emptyMat = cv::Mat::zeros(cv::Size(image.cols, image.rows), CV_8UC1);
  std::vector<cv::Mat> channels;

  channels.push_back(bgr_planes[0]);
  channels.push_back(emptyMat);
  channels.push_back(emptyMat);
  cv::merge(channels, bgr_planes[0]);

  channels.clear();
  channels.push_back(emptyMat);
  channels.push_back(bgr_planes[1]);
  channels.push_back(emptyMat);
  cv::merge(channels, bgr_planes[1]);

  channels.clear();
  channels.push_back(emptyMat);
  channels.push_back(emptyMat);
  channels.push_back(bgr_planes[2]);
  cv::merge(channels, bgr_planes[2]);

  std::vector<cv::Mat> buffers;
  buffers.resize(2);
  cv::vconcat(image, bgr_planes[1], buffers[0]);
  cv::vconcat(bgr_planes[0], bgr_planes[2], buffers[1]);
  cv::hconcat(buffers[0], buffers[1], firstCollage);

  buffers.clear();
  buffers.resize(2);
  std::vector<cv::Mat> histograms{GenerateHistogram(image, new int[] {0, 1, 2}),
                                  GenerateHistogram(bgr_planes[1], new int[] {1}),
                                  GenerateHistogram(bgr_planes[0], new int[] {0}),
                                  GenerateHistogram(bgr_planes[2], new int[] {2})};
  cv::vconcat(histograms[0], histograms[1], buffers[0]);
  cv::vconcat(histograms[2], histograms[3], buffers[1]);
  cv::hconcat(buffers[0], buffers[1], secondCollage);

  std::vector<cv::Mat> result{firstCollage, secondCollage};
  return result;
}

int main() {
  std::string filePath;
  std::cin >> filePath;
  cv::Mat image = cv::imread(filePath);
  cv::resize(image, image, cv::Size(), 0.1, 0.1);
  cv::Mat compressedImage = CompressImage(image, 25);
  std::vector<cv::Mat> imageCollages = GenerateCollages(image);
  std::vector<cv::Mat> compressedImageCollages = GenerateCollages(compressedImage);
  cv::imwrite("result_1.jpg", imageCollages[0]);
  cv::imwrite("result_2.jpg", imageCollages[1]);
  cv::imwrite("compressed_result_1.jpg", compressedImageCollages[0]);
  cv::imwrite("compressed_result_2.jpg", compressedImageCollages[1]);
}
