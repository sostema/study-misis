#include <algorithm>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

static const char *window_name = "BackFore";

//void getGradient(cv::Mat &in, cv::Mat &out, double constant = 0.15f) {
//    int ddepth = CV_32F;
//    cv::Mat in_gray, grad_x, grad_y;
//
//    if (in.empty()) {
//        return;
//    }
//
//    cv::cvtColor(in, in_gray, cv::COLOR_BGR2GRAY);
//    in_gray.convertTo(in_gray, CV_16SC1, 1.0 / 255.0);
//    cv::Scharr(in_gray, grad_x, ddepth, 1, 0);
//    cv::Scharr(in_gray, grad_y, ddepth, 0, 1);
//
//    cv::magnitude(grad_x, grad_y, out);
//    out += constant;
//}

void performMorphology(const cv::Mat &in, cv::Mat &out, cv::MorphTypes operation, int iterations = 1) {
    int element_size = static_cast<int>(std::max(3.0, std::min(cv::sqrt(in.rows * in.cols) / 300, 20.0)));
    cv::Mat structuring_element;
    cv::getStructuringElement(cv::MorphShapes::MORPH_ELLIPSE, cv::Size(element_size, element_size));

    cv::morphologyEx(in, out, operation, structuring_element, cv::Point(-1, -1), iterations);
}

void addAlphaChannel(const cv::Mat &in, cv::Mat &out, cv::Mat &alpha) {
    cv::cvtColor(in, out, cv::COLOR_BGR2BGRA);
    std::vector<cv::Mat> bgra;
    cv::split(out, bgra);
    bgra[3] = alpha;
    cv::merge(bgra, out);
}

//void removeBackgroundGradient(cv::Mat &in, cv::Mat &out) {
//    cv::Mat alpha_mask;
//    getGradient(in, alpha_mask);
//    cv::imwrite("gradient.png", alpha_mask);
//
//    cv::Point seed_point = {static_cast<int>(0.01f * in.cols), static_cast<int>(0.01f * in.rows)};
//
//    performMorphology(alpha_mask, alpha_mask, cv::MorphTypes::MORPH_DILATE, 3);
//
//    cv::floodFill(alpha_mask, seed_point, cv::Scalar(0), nullptr, cv::Scalar(0.01f), cv::Scalar(0.01f),
//                  cv::FloodFillFlags::FLOODFILL_FIXED_RANGE);
//    cv::imwrite("alpha_maks_flooded.png", alpha_mask);
//
//    performMorphology(alpha_mask, alpha_mask, cv::MorphTypes::MORPH_ERODE, 1);
//    cv::imwrite("alpha_mask_eroded.png", alpha_mask);
//    performMorphology(alpha_mask, alpha_mask, cv::MorphTypes::MORPH_OPEN, 3);
//    cv::imwrite("alpha_maks_opened.png", alpha_mask);
//    performMorphology(alpha_mask, alpha_mask, cv::MorphTypes::MORPH_ERODE, 1);
//    cv::imwrite("alpha_maks_eroded2.png", alpha_mask);
//    cv::threshold(alpha_mask, alpha_mask, 0, 255, cv::ThresholdTypes::THRESH_BINARY);
//    cv::imwrite("thresholded.png", alpha_mask);
//
//    alpha_mask.convertTo(alpha_mask, CV_8UC1, 255);
//    cv::GaussianBlur(alpha_mask, alpha_mask, cv::Size(5, 5), 5);
//
//    cv::imwrite("alpha_mask.png", alpha_mask);
//
//    addAlphaChannel(in, out, alpha_mask);
//}

void removeBackgroundCanny(const cv::Mat &in, cv::Mat &out) {
    cv::Mat in_gray, edges;
    cv::cvtColor(in, in_gray, cv::COLOR_BGR2GRAY);
    cv::imwrite("gray.png", in_gray);
    cv::Canny(in_gray, edges, 10, 100);
    cv::imwrite("canny.png", edges);
    performMorphology(edges, edges, cv::MorphTypes::MORPH_DILATE);
//    cv::dilate(edges, edges, cv::noArray());
    cv::imwrite("edges_after_dilation.png", edges);
    performMorphology(edges, edges, cv::MorphTypes::MORPH_ERODE);
//    cv::erode(edges, edges, cv::noArray());
    cv::imwrite("edges_after_erosion.png", edges);

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(edges, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

    cv::Mat alpha_mask(in.size(), CV_8UC1, cv::Scalar(0));
    for (auto &contour : contours) {
        cv::fillConvexPoly(alpha_mask, contour, cv::Scalar(255));
    }
    cv::imwrite("filled_countours.png", alpha_mask);


//    cv::dilate(alpha_mask, alpha_mask, cv::noArray());
    performMorphology(alpha_mask, alpha_mask, cv::MorphTypes::MORPH_DILATE);
    cv::imwrite("alpha_mask_after_dilation.png", alpha_mask);
//    cv::erode(alpha_mask, alpha_mask, cv::noArray());
    performMorphology(alpha_mask, alpha_mask, cv::MorphTypes::MORPH_ERODE);
    cv::imwrite("alpha_mask_after_erosion.png", alpha_mask);
    cv::GaussianBlur(alpha_mask, alpha_mask, cv::Size(5, 5), 5);
    cv::imwrite("mask.png", alpha_mask);
    addAlphaChannel(in_gray, out, alpha_mask);
}

void insertImage(cv::Mat &background, cv::Mat &foreground, cv::Mat &out) {
    int scale_width = background.size().width / foreground.size().width;
    int scale_height = background.size().height / foreground.size().height;
    int img_scale = std::min(scale_width, scale_height);

    cv::Mat resized_foreground;
    cv::resize(foreground, resized_foreground,
               cv::Size(foreground.size().width * img_scale, foreground.size().height * img_scale));

    int crop_X = resized_foreground.cols, crop_Y = resized_foreground.rows;
    int offset_W = (background.cols - crop_X) / 2, offset_H = (background.rows - crop_Y) / 2;

    cv::Point location(offset_W, offset_H);

    for (int y = std::max(location.y, 0); y < background.rows; ++y) {
        int fY = y - location.y;

        if (fY >= resized_foreground.rows)
            break;

        for (int x = std::max(location.x, 0); x < background.cols; ++x) {
            int fX = x - location.x;

            if (fX >= resized_foreground.cols)
                break;

            double opacity = ((double) resized_foreground.data[fY * resized_foreground.step +
                                                               fX * resized_foreground.channels() + 3]) / 255;

            for (int c = 0; opacity > 0 && c < background.channels(); ++c) {
                unsigned char overlayPx = resized_foreground.data[fY * resized_foreground.step +
                                                                  fX * resized_foreground.channels() + c];
                unsigned char srcPx = background.data[y * background.step + x * background.channels() + c];
                background.data[y * background.step + background.channels() * x + c] =
                        srcPx * (1. - opacity) + overlayPx * opacity;
            }
        }
    }
    background.copyTo(out);
}

int main(int argc, char **argv) {

    cv::String background_path = "../res/lawn.jpg";
    cv::String foreground_path = "../res/bucket.jpeg";

    cv::Mat background = cv::imread(background_path, cv::IMREAD_UNCHANGED);
    cv::Mat foreground = cv::imread(foreground_path, cv::IMREAD_UNCHANGED);

    if (foreground.empty()) {
        return -1;
    }
    if (background.empty()) {
        return -1;
    }

    if (foreground.channels() != 4) {
        removeBackgroundCanny(foreground, foreground);
        cv::imwrite("processed_foreground.png", foreground);
    }

    cv::Mat result;
    insertImage(background, foreground, result);
    cv::imwrite("result.png", result);

    cv::imshow(window_name, result);
    cv::waitKey();
    cv::destroyAllWindows();

    return 0;
}