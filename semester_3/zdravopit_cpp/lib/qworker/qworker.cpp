#include "qworker.h"
void QWorker::processFrame(const cv::Mat &frame) {
  model_.predict(const_cast<cv::Mat &>(frame));
  cv::Mat processedFrame = model_.currentFrame_;
  std::vector<Model::Prediction> predictions = model_.currentPredictions;
  emit predictionsReady(model_.currentPredictions);
  emit frameReady(processedFrame);
}
void QWorker::throughFrame(const cv::Mat &frame) {
  emit frameReady(frame);
}
