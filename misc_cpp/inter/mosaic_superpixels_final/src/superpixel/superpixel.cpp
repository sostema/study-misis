#include "superpixel.h"

cv::Mat Superpixel::ColorQuantization(const cv::Mat &image) const {
    cv::Mat data;
    image.convertTo(data, CV_32F);
    data = data.reshape(1, data.total());

    cv::Mat labels, centers;
    cv::kmeans(data, n_colors, labels, cv::TermCriteria(cv::TermCriteria::MAX_ITER, 10, 1.0), 3,
               cv::KMEANS_PP_CENTERS, centers);

    centers = centers.reshape(image.channels(), centers.rows);

    auto *p = data.ptr<cv::Vec3f>();
    for (int i = 0; i < data.rows; i++) {
        int center_id = labels.at<int>(i);
        p[i] = centers.at<cv::Vec3f>(center_id);
    }

    data = data.reshape(image.channels(), image.rows);
    data.convertTo(data, image.type());

    return data;
}

cv::Mat Superpixel::ProcessImage(const std::string &tile_path, const std::string &image_path) const {
    cv::Mat cell_image = cv::imread(tile_path, CV_8UC4);
    cv::Mat input_image = ReadImage(image_path);
    cv::Mat result_image, converted_image, quantized_image, mask, labels;
    int n_superpixels;

    result_image = cv::Mat(input_image.rows, input_image.cols, input_image.type(), cv::Scalar(0, 0, 0));
    quantized_image = ColorQuantization(input_image);

    cv::GaussianBlur(input_image, converted_image, cv::Size(5, 5), 0);
    cv::cvtColor(converted_image, converted_image, cv::COLOR_BGR2HSV);

    cv::Ptr<cv::ximgproc::SuperpixelSLIC>
            slic = cv::ximgproc::createSuperpixelSLIC(converted_image, algorithm, region_size, float(ruler));
    slic->iterate(num_iterations);
    if (min_element_size > 0)
        slic->enforceLabelConnectivity(min_element_size);

    n_superpixels = slic->getNumberOfSuperpixels();
    std::cout << "Количество суперпикселей: " << n_superpixels << std::endl;

    slic->getLabels(labels);
    for (int i = 0; i <= n_superpixels; i++) {
        mask = labels == i;
        int mask_size = cv::countNonZero(mask);
        int cell_size = cv::saturate_cast<int>(cv::sqrt(mask_size));

        cv::Mat cell, cell_mask;
        cell_image.copyTo(cell);
        try {
            cv::resize(cell, cell, cv::Size(cell_size, cell_size));
            std::vector<cv::Mat> planes;
            cv::split(cell, planes);
            cv::Mat alpha_plane = planes[3];
            cv::threshold(alpha_plane, cell_mask, 0, 255, 0);

            cv::Mat cropped_superpixel;
            quantized_image.copyTo(cropped_superpixel, mask);

            std::vector<std::vector<cv::Point>> contours;
            std::vector<cv::Vec4i> hierarchy;
            cv::findContours(mask, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
            cv::Moments M = cv::moments(contours[0]);
            cv::Point superpixel_center = {static_cast<int>((M.m10 / M.m00)), static_cast<int>((M.m01 / M.m00))};
            cv::Mat inset_image(result_image, cv::Rect(superpixel_center, cv::Size(cell_size, cell_size)));

            cv::Scalar dominant_color = FindDominantColor(cropped_superpixel);
            cell.setTo(dominant_color, cell_mask);
            cv::cvtColor(cell, cell, cv::COLOR_BGRA2BGR);
//        result_image.setTo(dominant_color, cropped_superpixel);
            cell.copyTo(result_image(cv::Rect(superpixel_center, cv::Size(cell_size, cell_size))), cell_mask);
        }
        catch (cv::Exception &exception) {
            continue;
        }
//        cv::imshow("resimgae", result_image);
    }

//    slic->getLabelContourMask(mask, true);
//    result_image.setTo(cv::Scalar(0, 0, 0), mask);

    return result_image;
}

cv::Mat Superpixel::ReadImage(const std::string &image_path) {
    cv::Mat image = cv::imread(image_path, cv::IMREAD_UNCHANGED);
    if (image.empty()) {
        std::ostringstream error_oss;
        error_oss << "Wrong path: " << image_path;
        throw std::invalid_argument(error_oss.str());
    }
    if (image.channels() == 4) {
        cv::cvtColor(image, image, cv::COLOR_BGRA2BGR);
    }
    return image;
}

cv::Scalar Superpixel::FindDominantColor(const cv::Mat &image) {
    cv::Mat data;
    image.convertTo(data, CV_32F);

    data = data.reshape(1, data.total());
    cv::Mat labels, centers;
    cv::kmeans(data, 2, labels, cv::TermCriteria(cv::TermCriteria::MAX_ITER, 10, 1.0), 3,
               cv::KMEANS_PP_CENTERS, centers);
    centers.convertTo(centers, image.type());
    centers = centers.reshape(image.channels(), centers.rows);
    auto color = centers.at<cv::Vec3b>(0, 0);
    if (color == cv::Vec3b(0, 0, 0)) {
        color = centers.at<cv::Vec3b>(0, 1);
    }
    return color;
}

