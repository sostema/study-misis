#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <utility>
#include <opencv2/core/utility.hpp>

#include <opencv2/ximgproc.hpp>

#include <iostream>

class Superpixel {
public:
    //! Стандартный конструктор (для создания пустого объекта генератора).
    Superpixel() = default;

    //! Деструктор.
    ~Superpixel() = default;

    //! Конструктор копирования.
    Superpixel(const Superpixel &rhs) = default;

    //! Конструктор.
    Superpixel(int algorithm_, int region_size_, int ruler_, int min_element_size_,
               int num_iterations_)
            : algorithm(algorithm_),
              region_size(region_size_),
              ruler(ruler_),
              min_element_size(min_element_size_),
              num_iterations(num_iterations_) {};

    //! Применение алгоритма
    cv::Mat ProcessImage(const std::string &tile_path, const std::string &image_path) const;

    //! Метод для чтения изображения.
    static cv::Mat ReadImage(const std::string &image_path);

private:
    //! Метод для уменьшения количества цветов в изображении
    cv::Mat ColorQuantization(const cv::Mat &image) const;

    //! Метод для поиска самого часто встреющегося цвета в изображении
    static cv::Scalar FindDominantColor(const cv::Mat &image);

    //! Выбранный алгоритм
    int algorithm{cv::ximgproc::SLIC};
    //! Средний размер суперпикселя
    int region_size{};
    //! Коэффициент усиления плавности суперпикселя
    int ruler{};
    //! Минимальный размер суперпикселя
    int min_element_size{};
    //! Количество итераций алгоритма
    int num_iterations{};

    //! Количество цветов в изображении
    int n_colors{16};
};
