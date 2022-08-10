#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

using namespace cv;
using namespace cv::xfeatures2d;

static const char *window_name = "Fingernote";

static const char *keys =
        "{h help      | | help menu}"
        "{c camera    |0| camera id}"
        "{v video     | | video file}"
        "{i image     | | image file}";

int main(int argc, char **argv) {
    CommandLineParser cmd(argc, argv, keys);
    if (cmd.has("help")) {
        cmd.about("Программа Fingernote для определения пальцев на банкнотах.\n"
                  "Если не предоставлен видеофайл, программа запросит открыть вебкамеру.\n"
                  "Use ['q' or 'Q' or 'esc'] to exit.\n");
        cmd.printMessage();
        return 0;
    }
    int capture = cmd.get<int>("camera");
    String video_file = cmd.get<String>("video");
    String image_file = cmd.get<String>("image");
    bool use_camera_capture = video_file.empty();
    cv::namedWindow(window_name, 0);
    int current_frame_idx = -1;

    VideoCapture cap;

    if (use_camera_capture) {
        if (!cap.open(capture)) {
            std::cout << "Could not initialize capturing..." << capture << "\n";
            return -1;
        }
    } else {
        if (!cap.open(video_file)) {
            std::cout << "Could not open video..." << video_file << "\n";
            return -1;
        }
    }

    const cv::Mat input_banknote = cv::imread(image_file, 0);

    //-- Шаг 1: Найти детекторы и ключевые точки у примера банкноты для определения.
    int minHessian = 400;
    Ptr<SURF> detector = SURF::create(minHessian);
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    std::vector<KeyPoint> keypoints_banknote;
    Mat descriptors_banknote;
    detector->detectAndCompute(input_banknote, noArray(), keypoints_banknote, descriptors_banknote);

    for (;;) {
        Mat frame;
        cap >> frame;
        current_frame_idx++;
        if (frame.empty()) break;

        cv::Mat mask(frame.rows, frame.cols, CV_8UC1, cv::Scalar(0));

        //-- Шаг 2: Находим на изображении пальцы (пиксели с характерным для кожи цветом)
        cv::Mat bgr2hsvImg, hsv2skinImg;
        cv::cvtColor(frame, bgr2hsvImg, cv::COLOR_BGR2HSV);
        cv::inRange(bgr2hsvImg, cv::Scalar(0,80,80), cv::Scalar(20,255,255),
                    hsv2skinImg);
        cv::blur(hsv2skinImg, hsv2skinImg, cv::Size(3,3));

//        cv::imshow("Finger detection", hsv2skinImg);

        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        //-- Шаг 3: Находим дескрипторы и ключевые точки в изображении
        std::vector<KeyPoint> keypoints_frame;
        Mat descriptors_frame;
        detector->detectAndCompute(gray, noArray(), keypoints_frame, descriptors_frame);

        //-- Шаг 3: Соотносим дескрипторы банкноты с изображением
        std::vector<std::vector<DMatch> > knn_matches;
        matcher->knnMatch(descriptors_banknote, descriptors_frame, knn_matches, 2);

        //-- Шаг 4: Отбрасываем плохие совпадения
        const float RATIO_THRESH = 0.725f;
        const int MIN_MATCH_COUNT = 150;
        std::vector<DMatch> good_matches;
        for (auto &knnMatch : knn_matches) {
            if (knnMatch[0].distance < RATIO_THRESH * knnMatch[1].distance) {
                good_matches.push_back(knnMatch[0]);
            }
        }
        //-- Если хороших совпадений больше минимального необходимо количества, то на изображении присутствует банкнота
        if (good_matches.size() >= MIN_MATCH_COUNT) {

            //-- Шаг 5 (Опционально): Отрисовываем все хорошие совпадения для проверки гипотезы
            Mat img_matches;
            drawMatches(input_banknote, keypoints_banknote, frame, keypoints_frame, good_matches,
                        img_matches,
                        Scalar::all(-1),
                        Scalar::all(-1), std::vector<char>(),
                                DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

            //-- Шаг 6: Строим гомографию по ключевым точкам среди хороших совпадений
            std::vector<Point2f> banknote;
            std::vector<Point2f> frame_scene;
            for (auto &goodMatch : good_matches) {
                //-- Get the keypoints from the good matches
                banknote.push_back(keypoints_banknote[goodMatch.queryIdx].pt);
                frame_scene.push_back(keypoints_frame[goodMatch.trainIdx].pt);
            }
            Mat H = findHomography(banknote, frame_scene, RANSAC);

            //-- Шаг 7: Находим границы банкноты на изображении посредством преобразования перспективы
            std::vector<Point2f> obj_corners(4);
            obj_corners[0] = Point2f(0, 0);
            obj_corners[1] = Point2f((float) input_banknote.cols, 0);
            obj_corners[2] = Point2f((float) input_banknote.cols, (float) input_banknote.rows);
            obj_corners[3] = Point2f(0, (float) input_banknote.rows);
            std::vector<std::vector<Point>> scene_corners(1);
            perspectiveTransform(obj_corners, obj_corners, H);

            scene_corners[0].emplace_back(Point((int)obj_corners[0].x, (int)obj_corners[0].y));
            scene_corners[0].emplace_back(Point((int)obj_corners[1].x, (int)obj_corners[1].y));
            scene_corners[0].emplace_back(Point((int)obj_corners[2].x, (int)obj_corners[2].y));
            scene_corners[0].emplace_back(Point((int)obj_corners[3].x, (int)obj_corners[3].y));

            //-- Шаг 8: Заполняем найденную область банкноты
            cv::drawContours(mask,scene_corners,-1, Scalar(255),cv::FILLED,
                             8 );
//            imshow("Banknote Mask", mask);
//            imshow("Good Matches & Object detection", img_matches);

            //-- Шаг 9: Находим пересечение масок пальцев и банкноты на изображении
            cv::Mat finger_mask_overlap = mask & hsv2skinImg;
//            imshow("Finger Mask Overlap", finger_mask_overlap);
            int number_of_pixels_in_mask = cv::countNonZero(mask);
            int number_of_pixels_in_finger_mask = cv::countNonZero(finger_mask_overlap);
            float pixel_ratio = (float)number_of_pixels_in_finger_mask / (float)number_of_pixels_in_mask;

            const float MIN_RATIO = 0.05;

            //-- Если пальцы составляют больше определённого процента на маске банкноты,
            //-- то значит они лежат на банкноте
            if (pixel_ratio > MIN_RATIO) {
                std::cout << "FOUND FINGERS ON FRAME №" << current_frame_idx << std::endl;
                cv::Mat detected_fingernote;
                cv::Mat fingers_highlight;
                cv::cvtColor(finger_mask_overlap, fingers_highlight, cv::COLOR_GRAY2BGR);
                fingers_highlight = (fingers_highlight - cv::Scalar(0,0,255)) / 2;
                fingers_highlight = frame - fingers_highlight;
                cv::imshow(window_name, fingers_highlight);

            }
            else {
                std::cout << "No fingers were found on frame №" << current_frame_idx << std::endl;
                imshow(window_name, frame);
            }
        }
        else {
            std::cout << "No banknote was detected on frame №" << current_frame_idx << std::endl;
            imshow(window_name, frame);
        }

        std::cout << "---" << std::endl;



        int c = waitKey(1) & 0xff;
        if (c == 'q' || c == 'Q' || c == 27)
            break;
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}