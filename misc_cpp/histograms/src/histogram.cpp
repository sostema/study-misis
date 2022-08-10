#include "histogram.h"

HistogramGenerator::HistogramGenerator(const int bins_) : bins{bins_} {
  histogram_image = Mat(histogram_height, histogram_width, CV_8UC3, background_color);
}
void HistogramGenerator::ProcessImage(const string &image_path) {
  // Сначала считываем изображение по пути
  ReadImage(image_path);
  // Вычисляем гистограмму по изображению
  CalculateHistogram();
  // Нормализуем гистограммы
  NormalizeHistogram();
  // Рисуем гистограмму на изображении
  DrawHistogram();
  // Показываем гистограмму и изначальное изображение
  ShowHistogram();
}
void HistogramGenerator::CalculateHistogram() {
  // Разделяем изображение на однокональные изображения
  split(image, bgr_planes);
  // Определяем границы значений по цвету (от 0 до 255).
  float range[] = {0, 256};
  const float *histRange = {range};

  // Вычисляем гистограммы по каждому цвету
  calcHist(&bgr_planes[0], 1, nullptr,
           Mat(), blue_histogram, 1, &bins, &histRange, true, false);
  calcHist(&bgr_planes[1], 1, nullptr,
           Mat(), green_histogram, 1, &bins, &histRange, true, false);
  calcHist(&bgr_planes[2], 1, nullptr,
           Mat(), red_histogram, 1, &bins, &histRange, true, false);
}
void HistogramGenerator::DrawHistogram() {
  // Вычисляем ширину столбца
  int bin_width = cvRound(histogram_width / bins);
  for (int i = 1; i < bins; i++) {
    line(histogram_image, Point(bin_width * (i), histogram_height - cvRound(blue_histogram.at<float>(i))),
         Point(bin_width * (i), histogram_height),
         blue_color, 1, 8, 0);
    line(histogram_image, Point(bin_width * (i), histogram_height - cvRound(green_histogram.at<float>(i))),
         Point(bin_width * (i), histogram_height),
         green_color, 1, 8, 0);
    line(histogram_image, Point(bin_width * (i), histogram_height - cvRound(red_histogram.at<float>(i))),
         Point(bin_width * (i), histogram_height),
         red_color, 1, 8, 0);
  }
}
void HistogramGenerator::NormalizeHistogram() {
  // Нормализируем гистограммы по количеству строк в изображении гистограммы (по значению)
  normalize(blue_histogram, blue_histogram, 0, histogram_image.rows, NORM_MINMAX, -1, Mat());
  normalize(green_histogram, green_histogram, 0, histogram_image.rows, NORM_MINMAX, -1, Mat());
  normalize(red_histogram, red_histogram, 0, histogram_image.rows, NORM_MINMAX, -1, Mat());
}
void HistogramGenerator::ShowHistogram() {
  imshow("Original Image", image);
  imshow("Histogram", histogram_image);
  waitKey(0);
}
void HistogramGenerator::ReadImage(const string &image_path) {
  image = imread(image_path, IMREAD_COLOR);
  if (image.empty()) {
    throw std::invalid_argument("Wrong path");
  }
}
