#include <limits>
#include <vector>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

static const char *window_name = "seamless";

static const char *keys =
        "{@image      | | image file}"
        "{w width     |0| width}"
        "{h height    |0| height}";

void getEnergyMap(const cv::Mat &in, cv::Mat &out) {
    cv::Mat gray;
    if (in.channels() == 3) {
        cv::cvtColor(in, gray, cv::COLOR_BGR2GRAY);
    } else {
        cv::cvtColor(in, gray, cv::COLOR_BGRA2GRAY);
    }
    cv::Mat dx, dy;
    cv::Sobel(gray, dx, CV_64F, 1, 0);
    cv::Sobel(gray, dy, CV_64F, 0, 1);
    cv::magnitude(dx, dy, out);

    double min_value, max_value, Z;
    minMaxLoc(out, &min_value, &max_value);
    Z = 1 / max_value * 255;
    out = out * Z;
    out.convertTo(out, CV_8U);
}

std::vector<int> findBestVerticalSeam(const cv::Mat &energy_map) {
    int height = energy_map.rows;
    int width = energy_map.cols;

    int dp[height][width];

    for (int col = 0; col < width; col++) {
        dp[0][col] = (int) (energy_map.at<uchar>(0, col));
    }

    int min_energy = std::numeric_limits<int>::max();
    int min_index = -1;

    for (int row = 1; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (col == 0)
                dp[row][col] = cv::min(dp[row - 1][col + 1], dp[row - 1][col]);
            else if (col == width - 1)
                dp[row][col] = cv::min(dp[row - 1][col - 1], dp[row - 1][col]);
            else
                dp[row][col] = cv::min({dp[row - 1][col - 1], dp[row - 1][col], dp[row - 1][col + 1]});
            dp[row][col] += (int) energy_map.at<uchar>(row, col);
        }
    }

    for (int col = 0; col < width; col++) {
        if (dp[height - 1][col] < min_energy) {
            min_energy = dp[height - 1][col];
            min_index = col;
        }
    }

    std::vector<int> seam(height);
    cv::Point current_position(height - 1, min_index);
    seam[current_position.x] = current_position.y;

    while (current_position.x != 0) {
        int value = dp[current_position.x][current_position.y] -
                    (int) energy_map.at<uchar>(current_position.x, current_position.y);
        int row = current_position.x;
        int col = current_position.y;
        if (col == 0) {
            if (value == dp[row - 1][col + 1])
                current_position = cv::Point(row - 1, col + 1);
            else
                current_position = cv::Point(row - 1, col);
        } else if (col == width - 1) {
            if (value == dp[row - 1][col - 1])
                current_position = cv::Point(row - 1, col - 1);
            else
                current_position = cv::Point(row - 1, col);
        } else {
            if (value == dp[row - 1][col - 1])
                current_position = cv::Point(row - 1, col - 1);
            else if (value == dp[row - 1][col + 1])
                current_position = cv::Point(row - 1, col + 1);
            else
                current_position = cv::Point(row - 1, col);
        }
        seam[current_position.x] = current_position.y;
    }

    return seam;
}

std::vector<int> findBestHorizontalSeam(const cv::Mat &energy_map) {
    int height = energy_map.rows;
    int width = energy_map.cols;

    int dp[height][width];

    for (int row = 0; row < height; row++) {
        dp[row][0] = (int) (energy_map.at<uchar>(row, 0));
    }

    int min_energy = std::numeric_limits<int>::max();
    int min_index = -1;

    for (int col = 1; col < width; col++) {
        for (int row = 0; row < height; row++) {
            if (row == 0)
                dp[row][col] = cv::min(dp[row + 1][col - 1], dp[row][col - 1]);
            else if (row == height - 1)
                dp[row][col] = cv::min(dp[row - 1][col - 1], dp[row][col - 1]);
            else
                dp[row][col] = cv::min({dp[row + 1][col - 1], dp[row][col - 1], dp[row - 1][col - 1]});
            dp[row][col] += (int) energy_map.at<uchar>(row, col);
        }
    }

    for (int row = 0; row < height; row++) {
        if (dp[row][width - 1] < min_energy) {
            min_energy = dp[row][width - 1];
            min_index = row;
        }
    }

    std::vector<int> seam(width);
    cv::Point current_position(min_index, width - 1);
    seam[current_position.y] = current_position.x;

    while (current_position.y != 0) {
        int value = dp[current_position.x][current_position.y] -
                    (int) energy_map.at<uchar>(current_position.x, current_position.y);
        int row = current_position.x;
        int col = current_position.y;
        if (row == 0) {
            if (value == dp[row + 1][col - 1])
                current_position = cv::Point(row + 1, col - 1);
            else
                current_position = cv::Point(row, col - 1);
        } else if (row == height - 1) {
            if (value == dp[row - 1][col - 1])
                current_position = cv::Point(row - 1, col - 1);
            else
                current_position = cv::Point(row, col - 1);
        } else {
            if (value == dp[row - 1][col - 1])
                current_position = cv::Point(row - 1, col - 1);
            else if (value == dp[row + 1][col - 1])
                current_position = cv::Point(row + 1, col - 1);
            else
                current_position = cv::Point(row, col - 1);
        }
        seam[current_position.y] = current_position.x;
    }

    return seam;
}

void deleteColumn(cv::Mat &image) {
    cv::Mat output_image(image.rows, image.cols - 1, image.type());
    cv::Mat energy_map;
    getEnergyMap(image, energy_map);
    std::vector<int> seam = findBestVerticalSeam(energy_map);

    for (int row = 0; row < output_image.rows; row++) {
        for (int col = 0; col < output_image.cols; col++) {
            if (col >= seam[row]) {
                output_image.at<cv::Vec3b>(row, col) = image.at<cv::Vec3b>(row, col + 1);
            } else {
                output_image.at<cv::Vec3b>(row, col) = image.at<cv::Vec3b>(row, col);
            }
        }
    }
    output_image.copyTo(image);
}

void deleteRow(cv::Mat &image) {
    cv::Mat output_image(image.rows - 1, image.cols, image.type());
    cv::Mat energy_map;
    getEnergyMap(image, energy_map);
    std::vector<int> seam = findBestHorizontalSeam(energy_map);

    for (int col = 0; col < image.cols; col++) {
        for (int row = 0; row < image.rows; row++) {
            if (row >= seam[col]) {
                output_image.at<cv::Vec3b>(row, col) = image.at<cv::Vec3b>(row + 1, col);
            } else {
                output_image.at<cv::Vec3b>(row, col) = image.at<cv::Vec3b>(row, col);
            }
        }
    }
    output_image.copyTo(image);
}

//Deprecated functionality
//void addColumn(cv::Mat &image) {
//    cv::Mat output_image(image.rows, image.cols + 1, image.type());
//    cv::Mat energy_map;
//    getEnergyMap(image, energy_map);
//    std::vector<int> seam = findBestVerticalSeam(energy_map);
//
//    for (int row = 0; row < image.rows; row++) {
//        int out_col = 0;
//        for (int col = 0; col < image.cols; col++) {
//            if (col == seam[row]) {
//                output_image.at<cv::Vec3b>(row, out_col) = image.at<cv::Vec3b>(row, col);
//                output_image.at<cv::Vec3b>(row, out_col + 1) = image.at<cv::Vec3b>(row, col);
//                out_col += 2;
//            } else {
//                output_image.at<cv::Vec3b>(row, out_col) = image.at<cv::Vec3b>(row, col);
//                out_col++;
//            }
//        }
//    }
//    output_image.copyTo(image);
//}
//
//void addRow(cv::Mat &image) {
//    cv::Mat output_image(image.rows + 1, image.cols, image.type());
//    cv::Mat energy_map;
//    getEnergyMap(image, energy_map);
//    std::vector<int> seam = findBestHorizontalSeam(energy_map);
//
//    for (int col = 0; col < image.cols; col++) {
//        int out_row = 0;
//        for (int row = 0; row < image.rows; row++) {
//            if (row == seam[col]) {
//                output_image.at<cv::Vec3b>(out_row, col) = image.at<cv::Vec3b>(row, col);
//
//                output_image.at<cv::Vec3b>(out_row + 1, col) = image.at<cv::Vec3b>(row, col);
//                out_row += 2;
//            } else {
//                output_image.at<cv::Vec3b>(out_row, col) = image.at<cv::Vec3b>(row, col);
//                out_row++;
//            }
//        }
//    }
//    output_image.copyTo(image);
//}

int main(int argc, char **argv) {
    cv::CommandLineParser cmd(argc, argv, keys);
    cv::namedWindow(window_name, 0);

    cv::String image_file = cmd.get<cv::String>("@image");
    const cv::Mat input_image = cv::imread(image_file);

    int new_width = cmd.get<int>("width");
    int new_height = cmd.get<int>("height");

    if (input_image.empty()) {
        throw std::invalid_argument("No file was found!");
    }
    if (input_image.channels() > 3) {
        throw std::invalid_argument("Image has more than 3 channels!");
    }

    if (new_width <= 0) {
        new_width = input_image.cols;
    }
    if (new_height <= 0) {
        new_height = input_image.rows;
    }

    cv::Mat output_image;
    input_image.copyTo(output_image);

//    cv::Mat energy_map;
//    getEnergyMap(input_image, energy_map);
//    cv::imwrite("unnorm_energy_map.png", energy_map);

    while (output_image.cols != new_width || output_image.rows != new_height) {
        if (output_image.cols < new_width) {
//            addColumn(output_image);
        } else if (output_image.cols > new_width) {
            deleteColumn(output_image);
        }
        if (output_image.rows < new_height) {
//            addRow(output_image);
        } else if (output_image.rows > new_height) {
            deleteRow(output_image);
        }
    }

    cv::imwrite("result.jpg", output_image);

    cv::Mat resized_input;
    cv::resize(input_image, resized_input, output_image.size());
    cv::imwrite("resized_result.jpg", resized_input);

    return 0;
}
