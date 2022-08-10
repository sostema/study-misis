#include <iostream>
#include <iomanip>
#include <limits>

#include "mosaic/mosaic.h"
#include "superpixel/superpixel.h"

int main() {
  cv::Mat result;
  std::string tile_path;
  std::string image_path;
  int choice;

  std::cout << std::setfill('-') << std::setw(80) << "" << std::endl;
  std::cout << std::setfill(' ') << std::setw(40) << "Mosaic++" << std::setw(40) << std::endl;
  std::cout << std::setfill('-') << std::setw(80) << "" << std::endl;
  do {
    std::cout << "Для использования обычного алгоритма, введите 0\n"
                 "Для использования Superpixel алгоритма, введите 1: ";
    std::cin >> choice;
    if (choice == 0 || choice == 1) {
      break;
    }
  } while (true);
  if (choice == 0) {
    MosaicGenerator mosaic_generator;
    int color_depth;
    std::vector<double> scales{};
    cv::Size shift{};

    std::cout << "Если вы хотите использовать стандартные гиперпараметры, введите 0\n"
                 "Если вы хотите использовать свои параметры, введите любое другое число: ";
    std::cin >> choice;
    if (choice == 0) {
      color_depth = 16;
      scales = {0.25, 0.2, 0.15};
      shift = {};
      tile_path = "../res/heart.png";
      image_path = "../res/kitten_2.png";
    } else {
      std::cout << "Введите глубину цвета плитки: ";
      std::cin >> color_depth;
      std::cout << "Введите коэффициенты масштабирования (0 чтобы закончить ввод):" << std::endl;
      double buffer;
      while ((std::cin >> buffer) && (std::fabs(buffer) > std::numeric_limits<double>::min()))
        scales.push_back(buffer);

      if (scales.empty())
        scales.push_back(1);

      int buffer_1, buffer_2;
      std::cout << "Введите отступ (два целых числа через пробел, или два нуля для полного замощения): ";
      std::cin >> buffer_1 >> buffer_2;
      if ((buffer_1 == 0) && (buffer_2 == 0))
        shift = {buffer_1, buffer_2};
    }

    mosaic_generator = MosaicGenerator(color_depth, scales, shift);

    while (true) {
      if (tile_path.empty()) {
        std::cout << "Введите путь к изображению-шаблону: ";
        std::cin >> tile_path;
      }
      if (image_path.empty()) {
        std::cout << "Введите путь к изображению, которое хотите обработать: ";
        std::cin >> image_path;
      }
      std::cout << "Данные получены, ожидайте..." << std::endl;
      try {
        result = mosaic_generator.GenerateMosaic(tile_path, image_path);
        std::cout << "Изображение готово!" << std::endl;
        cv::imshow("result", result);
        cv::waitKey();

        std::cout << "Сохраняем изображение, ожидайте..." << std::endl;
        cv::imwrite("result.png", result);
        std::cout << "Изображение сохранено, файл result.png" << std::endl;
        break;
      }
      catch (const std::invalid_argument &ia) {
        std::cerr << "Похоже, приложение не может найти ваши фото!" << '\n' << ia.what() << std::endl;
        tile_path.clear();
        image_path.clear();
      }
    }
  } else {
    Superpixel superpixel;
    int algorithm;
    int region_size;
    int ruler;
    int min_element_size;
    int num_iterations;

    std::cout << "Если вы хотите использовать стандартные гиперпараметры, введите 0\n"
                 "Если вы хотите использовать свои параметры, введите любое другое число: ";
    std::cin >> choice;
    if (choice == 0) {
      algorithm = cv::ximgproc::SLICO;
      region_size = 20;
      ruler = 15;
      min_element_size = 25;
      num_iterations = 10;
      tile_path = "../res/heart.png";
      image_path = "../res/kitten_2.png";
    } else {
      std::string buffer;
      do {
        std::cout << "Введите алгоритм (SLIC, SLICO, MSLIC): ";
        std::cin >> buffer;
        if (buffer == "SLIC") {
          algorithm = cv::ximgproc::SLIC;
          break;
        } else if (buffer == "SLICO") {
          algorithm = cv::ximgproc::SLICO;
          break;
        } else if (buffer == "MSLIC") {
          algorithm = cv::ximgproc::MSLIC;
          break;
        }
      } while (true);
      std::cout << "Введите средний размер суперпикселя: ";
      std::cin >> region_size;
      std::cout << "Введите коэффициент плавности: ";
      std::cin >> ruler;
      std::cout << "Введите минимальный размер суперпикселя (или 0 для неплотного замещения): ";
      std::cin >> min_element_size;
      std::cout << "Введите количество итераций: ";
      std::cin >> num_iterations;
    }
    superpixel = Superpixel(algorithm, region_size, ruler, min_element_size, num_iterations);

    while (true) {
        if (tile_path.empty()) {
            std::cout << "Введите путь к изображению-шаблону: ";
            std::cin >> tile_path;
        }
      if (image_path.empty()) {
        std::cout << "Введите путь к изображению, которое хотите обработать: ";
        std::cin >> image_path;
      }
      std::cout << "Данные получены, ожидайте..." << std::endl;
      try {
        result = superpixel.ProcessImage(tile_path, image_path);
        std::cout << "Изображение готово!" << std::endl;
        cv::imshow("result", result);
        cv::waitKey();

        std::cout << "Сохраняем изображение, ожидайте..." << std::endl;
        cv::imwrite("result.png", result);
        std::cout << "Изображение сохранено, файл result.png" << std::endl;
        break;
      }
      catch (const std::invalid_argument &ia) {
        std::cerr << "Похоже, приложение не может найти ваши фото!" << '\n' << ia.what() << std::endl;
        image_path.clear();
      }
    }
  }

  return 0;
}
