#define OBJECT_DETECTION_OBJECT_DETECTION_H
#ifdef OBJECT_DETECTION_OBJECT_DETECTION_H

#include <fstream>
#include <sstream>

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <utility>

//!  Класс для обработки кадра.
class Model {
 public:
  //! Стандартный конструктор (для создания пустого объекта модели).
  Model() = default;
  //! Конструктор.
  Model(std::string &modelPath,
        std::string &configPath,
        std::vector<std::string> classes,
        std::string &framework = (std::string &) "",
        float confThreshold = 0.5f,
        float nmsThreshold = 0.5f,
        float scale = 1,
        int inpWidth = 640,
        int inpHeight = 640,
        cv::Scalar mean = cv::Scalar(0, 0, 0),
        bool swapRB = true);
  //! Деструктор.
  ~Model() = default;
  //! Конструктор копирования.
  Model(const Model &) = default;

  //! Метод для обработки изображения.
  void predict(cv::Mat &frame);
  //! Метод для предобработки изображения.
  inline void preprocess(cv::Mat &frame);
  //! Метод для постобработки изображения.
  void postprocess(cv::Mat &frame, const std::vector<cv::Mat> &outs);
  //! Метод для изображения границ найденных объектов.
  void drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat &frame);

  //! Структура предсказания.
  struct Prediction;
  //! Вектор текущих предсказаний.
  std::vector<Prediction> currentPredictions{};
  //! Текущее изображение для обработки.
  cv::Mat currentFrame_{};

 private:
  //! Интерфейс работы с Tensorflow моделью в OpenCV DNN.
  cv::dnn::Net net_;
  float confThreshold_{0}, nmsThreshold_{0}, scale_{0};
  int inpWidth_{0}, inpHeight_{0};
  cv::Scalar mean_;
  bool swapRB_{true};
  //! Вектор названий классов.
  std::vector<std::string> classes_;
  //! Вектор последнего слоя.
  std::vector<std::string> outNames_;
};

//! Структура предсказания.
struct Model::Prediction {
  //! Конструктор.
  Prediction(cv::Mat frame, std::string label) : frame_(std::move(frame)), label_(std::move(label)) {}
  //! Изображение предсказанного продукта.
  cv::Mat frame_;
  //! Название предсказанного продукта.
  std::string label_;
};

#endif