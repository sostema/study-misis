#include <iostream>
#include <opencv2/quality/qualitymse.hpp>
#include <opencv2/quality/qualityssim.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

int main() {
    cv::Mat etalonMask, mask;
    etalonMask = cv::imread("etalonMask.png", cv::IMREAD_GRAYSCALE);
    mask = cv::imread("mask.png", cv::IMREAD_GRAYSCALE);
    cv::resize(mask, mask, etalonMask.size());
    std::cout << "MSE: " << cv::quality::QualityMSE::compute(mask, etalonMask, cv::noArray()) << std::endl;
    std::cout << "SSIM: " << cv::quality::QualitySSIM::compute(mask, etalonMask, cv::noArray()) << std::endl;

    return 0;
}
