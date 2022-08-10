#include "mosaic.h"

#include <utility>

MosaicGenerator::MosaicGenerator(int color_depth_, std::vector<double> scales_, cv::Size shift_)
        : color_depth(color_depth_), scales(std::move(scales_)), shift(std::move(shift_)) {
    tile_generator = TileGenerator(color_depth);
}

void MosaicGenerator::ColorQuantization() {
    std::vector<cv::Mat> planes;
    split(input_image, planes);
    cv::Mat mask;
    planes[3].copyTo(mask);
    cv::threshold(mask, mask, 0, 255, cv::THRESH_BINARY);

    cv::Mat test_image;
    merge(planes, test_image);

    cv::Mat data;
    test_image.convertTo(data, CV_32F);
    data = data.reshape(1, data.total());

    cv::Mat labels, centers;
    cv::kmeans(data, color_depth, labels, cv::TermCriteria(cv::TermCriteria::MAX_ITER, 10, 1.0), 3,
               cv::KMEANS_PP_CENTERS, centers);

    centers = centers.reshape(4, centers.rows);
    data = data.reshape(4, data.rows);

    auto *p = data.ptr<cv::Vec4f>();
    for (size_t i = 0; i < data.rows; i++) {
        int center_id = labels.at<int>(i);
        p[i] = centers.at<cv::Vec4f>(center_id);
    }
    test_image = data.reshape(4, test_image.rows);
    test_image.convertTo(test_image, CV_8UC4);
    test_image.copyTo(input_image, mask);

    cv::Mat_<cv::Vec4b>::iterator iterator_start;
    iterator_start = input_image.begin<cv::Vec4b>();
    cv::Mat_<cv::Vec4b>::iterator iterator_end;
    iterator_end = input_image.end<cv::Vec4b>();
    for (; iterator_start != iterator_end; iterator_start++) {
        if (std::fabs((*iterator_start)[3]) <= std::numeric_limits<uchar>::min()) {
            (*iterator_start) = {0, 0, 0, 0};
        } else {
            (*iterator_start)[3] = 255;
        }
    }
}

cv::Mat MosaicGenerator::GenerateMosaic(const std::string &tile_path, const std::string &image_path) {
    cv::Mat tile = ReadImage(tile_path);
    tilemap = tile_generator.GenerateTiles(tile, scales);
    std::cout << "Количество плиток: " << tilemap.size() * tilemap.begin()->second.size() << std::endl;
    input_image = ReadImage(image_path);
    cv::resize(input_image, input_image, cv::Size(input_image.size().width, input_image.size().height));
    ColorQuantization();
//  imwrite("image.png", input_image);
    ProcessImageBoxes();
    std::cout << "Количество квадратиков: " << boxes.size() << std::endl;
    CreateTiledMosaic();
    return output_image;
}

cv::Mat MosaicGenerator::ReadImage(const std::string &image_path) {
    cv::Mat image = cv::imread(image_path, CV_8UC4);
    if (image.empty()) {
        std::ostringstream error_oss;
        error_oss << "Wrong path: " << image_path;
        throw std::invalid_argument(error_oss.str());
    }
    return image;
}

// Находим самый частовстречаемый цвет в картинке
std::pair<cv::Scalar, double> FindMostFrequentColor(const cv::Mat &image) {
    cv::Mat data;
    image.convertTo(data, CV_32F);
    cv::Mat unshaped_data;
    data.copyTo(unshaped_data);

    data = data.reshape(1, data.total());
    cv::Mat labels, centers;
    cv::kmeans(data, 1, labels, cv::TermCriteria(cv::TermCriteria::MAX_ITER, 10, 1.0), 3,
               cv::KMEANS_PP_CENTERS, centers);
    centers.convertTo(centers, CV_8UC4);
    centers = centers.reshape(4, centers.rows);
    auto color = centers.at<cv::Vec4b>(0, 0);
    if (std::fabs(color[3]) <= std::numeric_limits<uchar>::min()) {
        return std::make_pair(color, 0);
    }

    int counter{1}, total{image.size().height * image.size().width};
    double frac{0};

    for (int y = 0; y < image.size[1]; y++) {
        for (int x = 0; x < image.size[0]; x++) {
            const auto &pixel_color = image.at<cv::Vec4b>(y, x);
            if (pixel_color == color) {
                counter++;
            }
        }
    }
    frac = (double) counter / (double) total;
    return std::make_pair(color, frac);
}

// Разбиваем картинку на квадратики определённого размера
std::vector<Box> MosaicGenerator::ImageSplitBox(const cv::Mat &image, const cv::Size &resolution) {
    cv::Size box_shift; // Сдвиг разбиения
    if (shift.empty()) {
        box_shift = resolution;
    } else {
        box_shift = shift;
    }
    std::vector<Box> boxes_; // Квадратики
    for (int y = 0; y < image.size[1]; y += box_shift.height) {
        for (int x = 0; x < image.size[0]; x += box_shift.width) {
            // Берём ROI (кропаем изображение) квадратика
            cv::Rect roi = cv::Rect(cv::Point(x, y), cv::Point(x + resolution.width, y + resolution.height));
            // Удостоверяемся, что квадратик не вышел за границы картинки
            cv::Rect clipped_roi = roi & cv::Rect(0, 0, image.size().width, image.size().height);
            if (roi == clipped_roi) {
                cv::Mat boxed_image;
                image(roi).copyTo(boxed_image);
//        imwrite("boxed_image.png", boxed_image);
                auto mfc = FindMostFrequentColor(boxed_image);
                if (std::fabs(mfc.second) > std::numeric_limits<double>::min()) {
                    boxes_.emplace_back(Box(mfc, cv::Point(x, y), roi.size()));
                }
            }
        }
    }
    return boxes_;
}

std::pair<Tile, double> MosaicGenerator::MostSimilarTile(const Box &box, const std::vector<Tile> &tiles_) {
    Tile result = tiles_[0];
    double result_distance = -1;
    double minimal_distance = std::numeric_limits<double>::max();
    for (const auto &tile : tiles_) {
        double distance = (1 + cv::norm(box.color.first, tile.color, cv::NORM_L2)) / box.color.second;
        if (distance < minimal_distance) {
            result = tile;
            result_distance = distance;
            minimal_distance = distance;
        }
    }
    return std::make_pair(result, result_distance);
}

void MosaicGenerator::ProcessImageBoxes() {
    for (const auto &sized_tiles : tilemap) {
        auto size = sized_tiles.first;
        auto tiles = sized_tiles.second;
        std::vector<Box> boxes_ = ImageSplitBox(input_image, size);
        for (auto &box : boxes_) {
            auto most_similar_pair = MostSimilarTile(box, tiles);
            box.tile = most_similar_pair.first;
            box.distance = most_similar_pair.second;
            boxes.emplace_back(box);
        }
    }
}

void MosaicGenerator::TilePlacement(const Box &box) {
    cv::Point p1 = box.position;
    cv::Point p2 = p1 + cv::Point(box.size.width, box.size.height);
    cv::Mat image_box;
    output_image(cv::Rect(p1, p2)).copyTo(image_box);
    cv::Mat image_mask, mask;
    std::vector<cv::Mat> planes;

    cv::split(box.tile.image, planes);
    planes[3].copyTo(mask);
    cv::split(image_box, planes);
    planes[3].copyTo(image_mask);

    cv::Mat mask_diff;
    cv::bitwise_and(image_mask, mask, mask_diff);

    if (std::fabs(cv::sum(mask_diff)[0]) <= std::numeric_limits<double>::min()) {
        box.tile.image(cv::Rect(0, 0,
                                image_box.size().width, image_box.size().height)).copyTo(
                output_image(cv::Rect(p1, p2)));
    }
}

void MosaicGenerator::CreateTiledMosaic() {
    output_image = cv::Mat(input_image.size(), CV_8UC4, {0, 0, 0, 0});
    std::sort(boxes.begin(), boxes.end());
    for (const auto &box : boxes) {
        TilePlacement(box);
    }
}

