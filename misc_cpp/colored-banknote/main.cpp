#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

static const char *keys = "{h help      | | help menu}"
                          "{i image     | | image file}";

static const std::vector<int> r50{25, 30, 60, 255, 0, 255};
static const std::vector<int> r100{20, 25, 60, 255, 0, 255};
static const std::vector<int> r1000{20, 180, 60, 255, 0, 255};

// void findBanknote(const cv::Mat &input, cv::Mat &out) {
//     cv::Mat blurred = input.clone();
//     cv::medianBlur(blurred, blurred, 9);
//
//     cv::Mat gray(blurred), gray_;
//     cvtColor(gray, gray, cv::COLOR_BGR2GRAY);
//     std::vector<std::vector<cv::Point>> contours;
//     std::vector<cv::Point> biggest_area;
//
//     const int threshold_level = 2;
//     for (int l = 0; l < threshold_level; l++) {
//         if (l == 0) {
//             cv::Canny(gray, gray_, 10, 20, 3);
//             cv::dilate(gray_, gray_, cv::Mat(), cv::Point(-1, -1));
//         } else {
//             gray_ = gray >= int((l + 1) * 255 / threshold_level);
//         }
//
//         cv::findContours(gray_, contours, cv::RETR_LIST,
//         cv::CHAIN_APPROX_SIMPLE); std::vector<cv::Point> approx; for (auto
//         &contour: contours) {
//             cv::approxPolyDP(cv::Mat(contour), approx,
//                              arcLength(cv::Mat(contour), true) * 0.02, true);
//
//             if (approx.size() == 4 && cv::isContourConvex(cv::Mat(approx))) {
//                 if (biggest_area.empty()) {
//                     biggest_area = approx;
//                 }
//                 if (fabs(cv::contourArea(approx)) >
//                 fabs(cv::contourArea(biggest_area))) {
//                     biggest_area = approx;
//                 }
//             }
//         }
//     }
//     if (biggest_area.empty()) {
//         std::cout << "Купюра не обнаружена!" << std::endl;
//     } else {
//         out = input(cv::boundingRect(biggest_area));
//     }
// }

void colorTransformation(const cv::Mat &input, cv::Mat &r50mat,
                         cv::Mat &r100mat, cv::Mat &r1000mat) {
  cv::Mat result{input.clone()};
  cv::cvtColor(input, result, cv::COLOR_BGR2HSV_FULL);
  //    cv::imshow("hsv", result);
  //    cv::waitKey();

  cv::inRange(result, std::vector<int>{r50[0], r50[2], r50[4]},
              std::vector<int>{r50[1], r50[3], r50[5]}, r50mat);

  cv::inRange(result, std::vector<int>{r100[0], r100[2], r100[4]},
              std::vector<int>{r100[1], r100[3], r100[5]}, r100mat);

  cv::inRange(result, std::vector<int>{r1000[0], r1000[2], r1000[4]},
              std::vector<int>{r1000[1], r1000[3], r1000[5]}, r1000mat);
}

void findClosestValue(const cv::Mat &r50mat, const cv::Mat &r100mat,
                      const cv::Mat &r1000mat) {
  int r50count, r100count, r1000count;
  r50count = cv::countNonZero(r50mat);
  r100count = cv::countNonZero(r100mat);
  r1000count = cv::countNonZero(r1000mat);

  std::cout << r50count << " " << r100count << " " << r1000count << std::endl;

  int maxcount = std::max({r50count, r100count, r1000count});
  if (maxcount == 0) {
    std::cout << "Купюра не обнаружена!" << std::endl;
  } else if (r50count == maxcount) {
    std::cout << "Обнаружена купюра номиналом 50 рублей!" << std::endl;
  } else if (r100count == maxcount) {
    std::cout << "Обнаружена купюра номиналом 100 рублей!" << std::endl;
  } else if (r1000count == maxcount) {
    std::cout << "Обнаружена купюра номиналом 1000 рублей!" << std::endl;
  }
}

int main(int argc, char **argv) {
  cv::CommandLineParser cmd(argc, argv, keys);
  if (cmd.has("help")) {
    cmd.about(
        "Программа ColoredBanknote для определения номинала купюры по цвету.");
    cmd.printMessage();
    return 0;
  }

  cv::String image_file = cmd.get<cv::String>("image");
  cv::Mat input_banknote = cv::imread(image_file);
  //Шаг №1: находим контуры
  cv::Mat result;
  input_banknote.copyTo(result);
  //    cv::imshow("res", result);
  cv::imwrite("banknote.jpg", result);
  //    cv::waitKey();

  //Шаг №2: Приводим найденные купюры в HSV и приводим в цветовой диапазон
  cv::Mat r50mat, r100mat, r1000mat;
  result.copyTo(r50mat);
  result.copyTo(r100mat);
  result.copyTo(r1000mat);
  colorTransformation(result, r50mat, r100mat, r1000mat);
  cv::imwrite("r50.jpg", r50mat);
  cv::imwrite("r100.jpg", r100mat);
  cv::imwrite("r1000.jpg", r1000mat);

  //Шаг №3: Находим наибольшее совпадение
  findClosestValue(r50mat, r100mat, r1000mat);

  return 0;
}