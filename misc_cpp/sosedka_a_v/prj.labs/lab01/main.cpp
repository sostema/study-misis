/**
    Lab01 : Gamma correction research in OpenCV
    @file main.cpp
    @author Artyom Sosedka
    @version 1.0
    @date 20/02/21
*/

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <ctime>
#include <fstream>
#include <string>

/**
 * Applies a gamma correction with the specified coefficient to the image using the pow function.
 * @param image - image for gamma correction.
 * @param gammaCoefficient - gamma correction coefficient.
 * @return image with gamma correction
 */
cv::Mat GammaCorrectionPow(const cv::Mat &image, double gammaCoefficient);

/**
 * Applies a gamma correction with the specified coefficient to the image using direct access to pixels.
 * @param image - image for gamma correction.
 * @param gammaCoefficient - gamma correction coefficient.
 * @return image with gamma correction
 */
cv::Mat GammaCorrection(const cv::Mat &image, double gammaCoefficient);

/**
 * Applies a gamma correction with the specified coefficient to the image using direct access to pixels with TUP.
 * @param image - image for gamma correction.
 * @param gammaCoefficient - gamma correction coefficient.
 * @return image with gamma correction
 */
cv::Mat GammaCorrectionTUP(const cv::Mat &image, double gammaCoefficient);

int main() {
  //Generating testing image
  cv::Mat lines(60, 768, CV_8U, cv::Scalar(0));
  for (int r = 0; r < lines.cols; r++) {
    lines.col(r).setTo((int) (r / 3));
  }

  u_int startTimestamp, endTimestamp, timeDifference;
  std::ofstream fout("result.out");
  std::stringstream experimentResult;
  std::string experimentResultString;

  float gammaCoefficient{2.3};

  startTimestamp = std::clock();
  cv::Mat image_gcp = GammaCorrectionPow(lines, gammaCoefficient);
  endTimestamp = std::clock();
  timeDifference = endTimestamp - startTimestamp;
  experimentResult << "Elapsed time for gamma correction (using pow function): " << timeDifference << " ms." << std::endl;

  startTimestamp = std::clock();
  cv::Mat image_gc = GammaCorrection(lines, gammaCoefficient);
  endTimestamp = std::clock();
  timeDifference = endTimestamp - startTimestamp;
  experimentResult << "Elapsed time for gamma correction (direct access): " << timeDifference << " ms." << std::endl;

  startTimestamp = std::clock();
  cv::Mat image_tup = GammaCorrectionTUP(lines, gammaCoefficient);
  endTimestamp = std::clock();
  timeDifference = endTimestamp - startTimestamp;
  experimentResult << "Elapsed time for gamma correction (direct access with TUP): " << timeDifference << " ms." << std::endl;

  experimentResultString = experimentResult.str();
  std::cout << experimentResultString;
  fout << experimentResultString;

  fout.close();

  // Concatenate images
  cv::Mat result;
  vconcat(lines, image_gcp, result);
  vconcat(result, image_gc, result);

  // Save result
  imwrite("result.png", result);
}

cv::Mat GammaCorrectionPow(const cv::Mat &image, double gammaCoefficient) {
  cv::Mat image_float;
  image.convertTo(image_float, CV_64F);
  image_float /= 255.0;
  cv::pow(image_float, gammaCoefficient, image_float);
  image_float *= 255.0;
  image_float.convertTo(image_float, CV_8U);

  return image_float;
}

cv::Mat GammaCorrection(const cv::Mat &image, double gammaCoefficient) {
  cv::Mat image_float;
  image.convertTo(image_float, CV_64F);
  for (int i = 0; i < image_float.rows; i++)
    for (int j = 0; j < image_float.cols; j++)
      image_float.at<double>(i, j) = (pow(image_float.at<double>(i, j) / 255.0, gammaCoefficient) * 255.0);
  image_float.convertTo(image_float, CV_8U);

  return image_float;
}

cv::Mat GammaCorrectionTUP(const cv::Mat &image, double gammaCoefficient) {
  cv::Mat lookUpTable(1, 256, CV_8U);
  uchar *p = lookUpTable.ptr();
  for (int i = 0; i < 256; ++i)
    p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gammaCoefficient) * 255.0);
  cv::Mat res = image.clone();
  cv::LUT(image, lookUpTable, res);

  return res;
}