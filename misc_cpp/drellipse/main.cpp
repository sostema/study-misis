#include <iostream>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;

cv::Mat hough_ellipse_detection(const cv::Mat &img, float threshold = 200, int neighbour_size = 10, int rad_step = 4,
                                int center_step = 4);

long int approximate(float x, long int step) {
    long int temp = lround(x / static_cast<float>(step));
    return temp * step;
}


int main() {
    cv::Mat input, result, test;
    input = cv::imread("input.jpg");
    if (input.empty()){
        return -1;
    }
    result = hough_ellipse_detection(input, 200, 10, 4 , 4);
    cv::imshow("result", result);
    cv::waitKey(0);
    cv::imwrite("result.png", result);
    return 0;
}

cv::Mat hough_ellipse_detection(const cv::Mat &img, float threshold, int neighbour_size, int rad_step, int center_step) {
    cv::Mat img_edge, img_ellipse;
    Canny(img, img_edge, 75, 100, 3, true);
    //imshow("img_edge", img_edge);
    //return img_edge;
    int max_r = min(img_edge.rows, img_edge.cols) / 2;
    img_ellipse = cv::Mat::zeros(img.rows, img.cols, CV_8U);

    vector<vector<cv::Mat> > accumelator;
    for (int r1 = rad_step; r1 < max_r; r1 += rad_step) {
        vector<cv::Mat> temp;
        for (int r2 = rad_step; r2 < max_r; r2 += rad_step) {
            cv::Mat ellipse_centers = cv::Mat::zeros(img.rows, img.cols, CV_32F);
            temp.push_back(ellipse_centers);
        }
        //cout<<temp.size()<<" "<<max_r / rad_step<<endl;
        accumelator.push_back(temp);
    }

    for (int r1 = rad_step; r1 < max_r; r1 += rad_step) {
        int i = r1 / rad_step;
        for (int r2 = rad_step; r2 < max_r; r2 += rad_step) {
            int j = r2 / rad_step;
            for (int y = 0; y < img_edge.rows; y++)
                for (int x = 0; x < img_edge.cols; x++) {
                    if (img_edge.at<uint8_t>(y, x) < 255)
                        continue;
                    for (int a = rad_step; a < img_edge.rows - rad_step; a += center_step) {
                        float d = r2 * r2 * (1 - (y - a) * (y - a) * 1.0 / (r1 * r1));
                        if (d < 0.0)
                            continue;
                        long int b1 = approximate(x - sqrt(d), center_step);
                        long int b2 = approximate(x + sqrt(d), center_step);
                        if (b1 >= rad_step && b1 < img_edge.cols - rad_step)
                            accumelator[i - 1][j - 1].at<float>(a, b1) += 1.0;
                        if (b2 >= rad_step && b2 < img_edge.cols - rad_step)
                            accumelator[i - 1][j - 1].at<float>(a, b2) += 1.0;
                    }
                }
        }
    }

    for (int r1 = rad_step; r1 < max_r; r1 += rad_step) {
        int i = r1 / rad_step;
        for (int r2 = rad_step; r2 < max_r; r2 += rad_step) {
            int j = r2 / rad_step;
            for (int a = rad_step; a < img_edge.rows - rad_step; a += center_step)
                for (int b = rad_step; b < img_edge.cols - rad_step; b += center_step) {

                    if (accumelator[i - 1][j - 1].at<float>(a, b) <= threshold)
                        continue;
                    bool local_max = true;
                    for (int d1 = i - neighbour_size;
                         local_max && d1 <= i + neighbour_size && d1 < max_r / rad_step; d1++) {
                        if (d1 < 1)
                            continue;
                        for (int d2 = j - neighbour_size;
                             local_max && d2 <= j + neighbour_size && d2 < max_r / rad_step; d2++) {
                            if (d2 < 1)
                                continue;
                            for (int d3 = a - neighbour_size * center_step;
                                 local_max && d3 <= a + neighbour_size * center_step &&
                                 d3 < img_edge.rows - rad_step; d3 += center_step) {
                                if (d3 < rad_step)
                                    continue;
                                for (int d4 = b - neighbour_size * center_step;
                                     local_max && d4 <= b + neighbour_size * center_step &&
                                     d4 < img_edge.rows - rad_step; d4 += center_step) {
                                    if (d4 < rad_step)
                                        continue;
                                    if (accumelator[i - 1][j - 1].at<float>(a, b) <
                                        accumelator[d1 - 1][d2 - 1].at<float>(d3, d4))
                                        local_max = false;
                                }
                            }
                        }
                    }

                    if (local_max) {
                        ellipse(img_ellipse, cv::Point(b, a), cv::Size(r2, r1), 0.0, 0.0, 360, cv::Scalar(255, 255, 255));
                    }

                }
        }
    }
    return img_ellipse;
}