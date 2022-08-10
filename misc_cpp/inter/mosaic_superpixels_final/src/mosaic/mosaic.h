#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <utility>
#include <set>
#include <limits>

#pragma once

#include "tile_generator/tilegen.h"

//! Метод для поиска наиболее частого цвета в изображении
std::pair<cv::Scalar, double> FindMostFrequentColor(const cv::Mat &image);

struct Box {
    Box(std::pair<cv::Scalar, double> color_, cv::Point position_, cv::Size size_, Tile tile_ = Tile())
            : color(std::move(color_)), position(std::move(position_)), size(std::move(size_)),
              tile(std::move(tile_)) {};
    std::pair<cv::Scalar, double> color;
    cv::Point position;
    cv::Size size;
    Tile tile{};
    double distance{0};

    bool operator<(const Box &rhs) const {
        return (distance < rhs.distance);
    }
};

//! Класс для создания мозаики
class MosaicGenerator {
public:
    //! Стандартный конструктор (для создания пустого объекта генератора).
    MosaicGenerator() = default;

    //! Деструктор.
    ~MosaicGenerator() = default;

    //! Конструктор копирования.
    MosaicGenerator(const MosaicGenerator &rhs) = default;

    //! Конструктор
    MosaicGenerator(int color_depth_, std::vector<double> scales_, cv::Size shift_);

    //! Метод, создающий мозаику по данной плитке
    cv::Mat GenerateMosaic(const std::string &tile_path, const std::string &image_path);

private:
    //! Метод для чтения изображения.
    static cv::Mat ReadImage(const std::string &image_path);

    //! Метод для уменьшения количества цветов в изображении
    void ColorQuantization();

    //! Метод для разбиения изображения на участки
    std::vector<Box> ImageSplitBox(const cv::Mat &image, const cv::Size &resolution);

    //! Метод поиска самой подходящей плитки к участку
    static std::pair<Tile, double> MostSimilarTile(const Box &box, const std::vector<Tile> &tiles_);

    //! Метод постройки участков и нахождения наилучшей плитки для каждого
    void ProcessImageBoxes();

    //! Наложение плитки на изображение
    void TilePlacement(const Box &box);

    //! Создание мозаики
    void CreateTiledMosaic();

    TileGenerator tile_generator{};
    std::map<cv::Size, std::vector<Tile>, SizeCompare> tilemap{};
    std::vector<Box> boxes{};

    cv::Mat input_image{};
    cv::Mat output_image{};

    int color_depth{};
    std::vector<double> scales;
    cv::Size shift;
};

