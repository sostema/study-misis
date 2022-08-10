#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"

static const char *keys =
        "{h help      | | help menu}"
        "{i image     | | image file}";

int main(int argc, char **argv) {
    cv::CommandLineParser cmd(argc, argv, keys);
    if (cmd.has("help")) {
        cmd.about("Программа NumNote для поиска серийных номеров на банкнотах.");
        cmd.printMessage();
        return 0;
    }
    cv::String image_file = cmd.get<cv::String>("image");
    cv::Mat input_image = cv::imread(image_file);
    cv::Mat output_image{input_image.clone()};

    cv::Ptr<cv::MSER> ms = cv::MSER::create();
    std::vector<std::vector<cv::Point> > regions;
    std::vector<cv::Rect> mser_bbox;

    cv::Mat gray_input{input_image.clone()};
    cv::cvtColor(gray_input, gray_input, cv::COLOR_BGR2GRAY);
    cv::Mat canny_input{gray_input.clone()};
    cv::Canny(canny_input, canny_input, 150, 250, 3);

    ms->detectRegions(canny_input, regions, mser_bbox);

    for (int i = 0; i < regions.size(); i++)
    {
        rectangle(canny_input, mser_bbox[i], CV_RGB(0, 255, 0));
    }

    imshow("mser", canny_input);
    cv::waitKey(0);
}