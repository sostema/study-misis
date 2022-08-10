#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

static const char *keys =
        "{h help      | | help menu}"
        "{i image     | | image file}";

// Создание цветной маски
void ColoredMask(cv::Mat &src, cv::Mat &dst, double threshold = -1) {
    cv::Mat gray{src.clone()}, color{src.clone()}, mask{src.clone()}, denoised{src.clone()};

    // Размытие для удаления мелких шумов.
    cv::medianBlur(src, denoised, 3);

    // Сохранение в ЧБ для получения маски.
    cv::cvtColor(denoised, gray, cv::COLOR_BGR2GRAY);

    // Получение цветной части изображения.
    double adaptiveThreshold = threshold < 0 ? cv::mean(src)[0] : threshold;
    cv::cvtColor(denoised, color, cv::COLOR_BGR2HLS);
    cv::inRange(color, cv::Scalar{0, adaptiveThreshold / 6, 60},
                cv::Scalar{180, adaptiveThreshold, 255}, mask);

    // Создание маски цветной части изображения.
    cv::bitwise_and(gray, gray, dst, mask);
}

// Проврека, равны ли матрицы (с погрешностью)
inline bool MatEquals(cv::Mat &lhs, cv::Mat &rhs, double epsilon = 0.0) {
    return cv::countNonZero(lhs != rhs) <= epsilon * lhs.size().area();
}

// Проверка кругов на заполненность
void ValidateCircles(cv::Mat &src, std::vector<cv::Vec3f> &input_circles, std::vector<cv::Vec3f> &dst_circles) {
    for (auto circle: input_circles) {
        cv::Point center{static_cast<int>(circle[0]), static_cast<int>(circle[1])};
        int radius{static_cast<int>(circle[2])};

        cv::Mat empty = cv::Mat::zeros(src.size(), CV_8UC1);
        cv::Mat mask = cv::Mat::zeros(src.size(), CV_8UC1);
        cv::Mat crop = cv::Mat::zeros(src.size(), CV_8UC1);

        cv::circle(mask, center, radius, cv::Scalar{255, 255, 255}, -1);
        src.copyTo(crop, mask);

        if (!MatEquals(crop, empty)) {
            dst_circles.push_back(circle);
        }
    }
}

// Поиск кругов по вписанным в контуры эллипсам
void FindNestedCircles(cv::Mat &src, std::vector<cv::Vec3f> &resulted_circles) {
    double minorSize = std::min(src.size[0], src.size[1]) * 0.075;
    double majorSize = std::max(src.size[0], src.size[1]) * 0.1;
    double roundness{0.7};

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(src, contours, hierarchy,
                     cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    for (const auto &contour: contours) {
        if (contour.size() < 5) {
            continue;
        }

        cv::RotatedRect ellipse = cv::fitEllipse(contour);
        double minor, major;
        minor = std::min(ellipse.size.width, ellipse.size.height);
        major = std::max(ellipse.size.width, ellipse.size.height);

        if (((minor / 2) > minorSize) && ((major / 2) < majorSize)) {
            double r1 = std::fabs(1 - std::fabs(major - minor) / std::max(minor, major));
            if (r1 > roundness) {
                cv::Vec3f nested_circle{ellipse.center.x, ellipse.center.y, static_cast<float>(major / 2)};
                resulted_circles.push_back(nested_circle);
            }
        }
    }
}

// Вспомогательная функция поиска расстояния между кругами
double DistanceBetweenCircles(cv::Vec3f &lhs, cv::Vec3f &rhs) {
    cv::Point lhs_center{static_cast<int>(lhs[0]), static_cast<int>(lhs[1])};
    cv::Point rhs_center{static_cast<int>(lhs[0]), static_cast<int>(lhs[1])};
    return cv::norm(lhs_center - rhs_center);
}

// Вспомогательная функция, которая определяет, находится ли круг внутри другого круга с погрешностью
bool IsCircleNested(cv::Vec3f &inner, cv::Vec3f &outer, double epsilon = 1.1) {
    double distance = DistanceBetweenCircles(inner, outer);
    double secondRadius = outer[2] * epsilon;
    return distance < secondRadius && inner[2] < (secondRadius - distance);
}

// Вспомогательная функция, проверяет, равны ли два вектора (в частности, на проверку кругов)
bool Vec3fEquals(cv::Vec3f &lhs, cv::Vec3f &rhs) {
    return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2];
}

// Удаление вложенных окружностей (чтобы удалить концентрические круги)
void FindOuterCircles(std::vector<cv::Vec3f> &inner_circles, std::vector<cv::Vec3f> &outer_circles,
                      std::vector<cv::Vec3f> &result_circles) {
    std::vector<cv::Vec3f> nested_circles;
    for (auto &inner: inner_circles) {
        for (auto &outer: outer_circles) {
            if (IsCircleNested(inner, outer, 1.3)) {
                if ((DistanceBetweenCircles(inner, outer) < 100) && ((inner[2] / outer[2]) > 0.5)) {
                    nested_circles.push_back(inner);
                } else {
                    nested_circles.push_back(outer);
                }
            }
        }
    }

    // Здесь мы создаём объединение внутренних и внешних кругов и после удаляем те, которые являются вложенными
    std::vector<cv::Vec3f> inner_and_outer_circles;
    inner_and_outer_circles.insert(inner_and_outer_circles.end(), inner_circles.begin(), inner_circles.end());
    inner_and_outer_circles.insert(inner_and_outer_circles.end(), outer_circles.begin(), outer_circles.end());

    for (auto &circle: inner_and_outer_circles) {
        bool isEligible = true;
        for (auto &nested_circle: nested_circles) {
            if (Vec3fEquals(circle, nested_circle)) {
                isEligible = false;
                break;
            }
        }
        if (isEligible) {
            result_circles.push_back(circle);
        }
    }
}


int main(int argc, char **argv) {
    cv::CommandLineParser cmd(argc, argv, keys);
    if (cmd.has("help")) {
        cmd.about("Программа Marking для поиска печатей на фотографии.");
        cmd.printMessage();
        return 0;
    }
    cv::String image_file = cmd.get<cv::String>("image");
    cv::Mat input_document = cv::imread(image_file);
    cv::Mat output_image{input_document.clone()};

    cv::Mat colored_mask{};
    ColoredMask(input_document, colored_mask);

    cv::imshow("colored_mask", colored_mask);
//    cv::waitKey();
    cv::imwrite("result/colored_mask.png", colored_mask);

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(colored_mask, circles, cv::HOUGH_GRADIENT, 1,
                     static_cast<int>(std::min(colored_mask.size().height, colored_mask.size().width) / 10),
                     100, 100, 0, 0);

    std::vector<cv::Vec3f> valid_circles;
    ValidateCircles(colored_mask, circles, valid_circles);

    output_image = input_document.clone();
    for (auto circle: valid_circles) {
        cv::Point center{static_cast<int>(circle[0]), static_cast<int>(circle[1])};
        int radius{static_cast<int>(circle[2])};
        cv::circle(output_image, center, radius, cv::Scalar{128, 0, 255}, 2);
    }
    cv::imshow("valid_circles", output_image);
//    cv::waitKey();
    cv::imwrite("result/valid_circles.png", output_image);

    std::vector<cv::Vec3f> nested_circles;
    FindNestedCircles(colored_mask, nested_circles);

    output_image = input_document.clone();
    for (auto circle: nested_circles) {
        cv::Point center{static_cast<int>(circle[0]), static_cast<int>(circle[1])};
        int radius{static_cast<int>(circle[2])};
        cv::circle(output_image, center, radius, cv::Scalar{128, 128, 255}, 2);
    }
    cv::imshow("nested_circles", output_image);
//    cv::waitKey();
    cv::imwrite("result/nested_circles.png", output_image);

    std::vector<cv::Vec3f> outer_circles;
    FindOuterCircles(nested_circles, valid_circles, outer_circles);

    output_image = input_document.clone();
    for (auto circle: outer_circles) {
        cv::Point center{static_cast<int>(circle[0]), static_cast<int>(circle[1])};
        int radius{static_cast<int>(circle[2])};
        cv::circle(output_image, center, radius, cv::Scalar{0, 255, 0}, 2);
    }
    cv::imshow("result_circles", output_image);
//    cv::waitKey();
    cv::imwrite("result/result_circles.png", output_image);

    output_image = cv::Mat::zeros(input_document.size(), input_document.type());
    cv::Mat mask = cv::Mat::zeros(input_document.size(), CV_8UC1);
    for (auto circle: outer_circles) {
        cv::Point center{static_cast<int>(circle[0]), static_cast<int>(circle[1])};
        int radius{static_cast<int>(circle[2])};
        cv::circle(mask, center, radius, cv::Scalar{255, 255, 255}, -1);
    }
    input_document.copyTo(output_image, mask);
    cv::imshow("result", output_image);
    cv::waitKey();
    cv::imwrite("result/result.png", output_image);

    return 0;
}
