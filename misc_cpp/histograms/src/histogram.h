#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#pragma once

using namespace cv;
using namespace std;

//!  Класс для создания гистограмм
class HistogramGenerator {
 public:
  //! Стандартный конструктор (для создания пустого объекта генератора).
  HistogramGenerator() = default;
  //! Деструктор.
  ~HistogramGenerator() = default;
  //! Конструктор копирования.
  HistogramGenerator(const HistogramGenerator &rhs) = default;
  //! Конструктор.
  explicit HistogramGenerator(int bins_);
  //! Метод для создания гистограммы (обработка изображения и создание гистограммы).
  void ProcessImage(const string &image_path);
 private:
  //! Метод для нормализации гистограммы.
  void NormalizeHistogram();
  //! Метод для чтения изображения по пути.
  void ReadImage(const string &image_path);
  //! Метод для вычисления гистограммы.
  void CalculateHistogram();
  //! Метод для рисования гистограммы на рисунок.
  void DrawHistogram();
  //! Метод для изображения гистограммы.
  void ShowHistogram();

  //! Количество интервалов (столбцов).
  int bins{};

  //! Изображение, для которого строят гистограмму.
  Mat image;
  //! Изображение, на которое строят гистограмму.
  Mat histogram_image;

  //! Вектор однокональных изображений.
  vector<Mat> bgr_planes;
  Mat blue_histogram, green_histogram, red_histogram;

  //! Ширина изображения гистограммы.
  int histogram_width{512};
  //! Высота изображения гистограммы.
  int histogram_height{400};

  //! Цвет заднего фона гистограммы.
  const Scalar background_color = Scalar(0, 0, 0);
  //! Красный цвет.
  const Scalar red_color = Scalar(255, 0, 0);
  //! Зелёный цвет.
  const Scalar green_color = Scalar(0, 255, 0);
  //! Синий цвет.
  const Scalar blue_color = Scalar(0, 0, 255);
};