#ifndef QWORKER_QWORKER_H_
#define QWORKER_QWORKER_H_

#include <QObject>
#include <QThread>
#include <utility>
#include <opencv/object_detection.h>

//!  Промежуточный класс для работы с предсказанием.
class QWorker : public QObject {
 Q_OBJECT
 public:
  //! Конструктор.
  QWorker(std::string &modelPath, std::string &modelConfigPath, std::map<int, std::string>& classes, std::map<std::string, std::string> config) {
    std::vector<std::string> retval;
    retval.reserve(classes.size());
    for (auto const& element : classes) {
      retval.push_back(element.second);
    }
    if(!config.empty()){
      model_ = Model(modelPath, modelConfigPath, retval, config.at("framework"), std::stof(config.at("confThreshold")), std::stof(config.at("nmsThreshold")), std::stof(config.at("scale")), std::stoi(config.at("inpWidth")), std::stoi(config.at("inpHeight")));
    }
    else{
      model_ = Model(modelPath, modelConfigPath, retval);
    }
 };

 public slots:
  //! Событие для предсказания изображения.
  void processFrame(const cv::Mat &frame);
  //! Событие для "пропуска" кадра.
  void throughFrame(const cv::Mat &frame);

 signals:
  //! Сигнал, передающий готовые предсказания.
  void predictionsReady(std::vector<Model::Prediction> predictions);
  //! Сигнал, передающий предсказанное изображение, обработанное моделью.
  void frameReady(const cv::Mat &frame);

 private:
  Model model_{}; //!< Модель, которая используется для предсказаний.
};

#endif
