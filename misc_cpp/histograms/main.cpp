#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

#include "src/histogram.h"

int main() {
  std::cout << "Write amount of bins: ";
  int bins;
  std::cin >> bins;

  HistogramGenerator histgen = HistogramGenerator(bins);

  std::cout << "Write the full path to the image: ";
  std::string choice;
  std::cin >> choice;

  histgen.ProcessImage(choice);
}