#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

struct result_t
{
    int TP;
    int FP;
    int FN;
    int TN;

    double accuracy() {
        return static_cast<double>(TP + TN) / static_cast<double>(TP + TN + FP + FN);
    };
};

result_t conf_mat_2b(cv::Mat1b reference, cv::Mat1b pred)
{
    CV_Assert(reference.size == pred.size);

    result_t result = { 0 };

    cv::Mat temp;
    cv::bitwise_and(pred, reference, temp);
    result.TP = cv::countNonZero(temp);
    cv::bitwise_and(pred, ~reference, temp);
    result.FP = cv::countNonZero(temp);
    cv::bitwise_and(~pred, reference, temp);
    result.FN = cv::countNonZero(temp);
    cv::bitwise_and(~pred, ~reference, temp);
    result.TN = cv::countNonZero(temp);

    return result;
}

int main(int argc, char **argv){

    cv::String reference_path = "../doc/bucket/mask_reference.png";
    cv::String mask_path = "../doc/bucket/mask.png";

    cv::Mat reference = cv::imread(reference_path, CV_8UC1);
    cv::Mat mask = cv::imread(mask_path, CV_8UC1);

    if (reference.empty()) {
        return -1;
    }
    if (mask.empty()) {
        return -1;
    }

    result_t result = conf_mat_2b(reference, mask);

    double accuracy = result.accuracy();

    std::cout << "Accuracy = " << accuracy << std::endl;

    return 0;
}