#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <map>
#include <utility>

#pragma once

struct SizeCompare {
    bool operator()(const cv::Size_<int> &lhs, const cv::Size_<int> &rhs) const {
        return lhs.width < rhs.width || lhs.height < rhs.height;
    }
};

//! Структура для представления плитки
struct Tile {
    //! Изображение плитки
    cv::Mat image{};
    //! Цвет плитки
    cv::Scalar color{};

    Tile(cv::Mat image_, cv::Scalar color_) : image(std::move(image_)), color(std::move(color_)) {};

    Tile() = default;
};

//! Класс для создания плиток из изображения
class TileGenerator {
public:
    //! Стандартный конструктор (для создания пустого объекта генератора).
    TileGenerator() = default;

    //! Деструктор.
    ~TileGenerator() = default;

    //! Конструктор копирования.
    TileGenerator(const TileGenerator &rhs) = default;

    //! Конструктор.
    explicit TileGenerator(int depth_) : depth(depth_) { CalculateColorVector(); };

    //! Метод для генерации плиток.
    std::map<cv::Size, std::vector<Tile>, SizeCompare> GenerateTiles(const cv::Mat &image,
                                                                     const std::vector<double> &scales);

private:
    //! Метод для создания вектора цвета.
    void CalculateColorVector();

    //! Глубина изображения плитки.
    int depth{};

    //! Цвета (с учётом глубины)
    std::vector<cv::Scalar> colors;
};


