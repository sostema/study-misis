#include "object_detection.h"

Model::Model(std::string &modelPath,
             std::string &configPath,
             std::vector<std::string> classes,
             std::string &framework,
             float confThreshold,
             float nmsThreshold,
             float scale,
             int inpWidth,
             int inpHeight,
             cv::Scalar mean,
             bool swapRB) : net_(cv::dnn::readNetFromTensorflow(modelPath, configPath)),
                                                 confThreshold_(confThreshold),
                                                 nmsThreshold_(nmsThreshold),
                                                 scale_(scale),
                                                 inpWidth_(inpWidth),
                                                 inpHeight_(inpHeight),
                                                 mean_(std::move(mean)),
                                                 swapRB_(swapRB),
                                                 classes_(std::move(classes)) {
  outNames_ = net_.getUnconnectedOutLayersNames();
}

void Model::preprocess(cv::Mat &frame) {
  static cv::Mat blob;
  // Create a 4D blob from a frame.
  cv::Size inpSize(inpWidth_, inpHeight_);
  if (inpSize.width <= 0) inpSize.width = frame.cols;
  if (inpSize.height <= 0) inpSize.height = frame.rows;
  cv::dnn::blobFromImage(frame, blob, 1.0, inpSize, cv::Scalar(), swapRB_, false, CV_8U);

  // Run a model.
  net_.setInput(blob, "", scale_, mean_);
  if (net_.getLayer(0)->outputNameToIndex("im_info") != -1)  // Faster-RCNN or R-FCN
  {
    resize(frame, frame, inpSize);
    cv::Mat imInfo = (cv::Mat_<float>(1, 3) << inpSize.height, inpSize.width, 1.6f);
    net_.setInput(imInfo, "im_info");
  }
}

void Model::drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat &frame) {
  rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 255, 0));

  std::string label = cv::format("%.2f", conf);
  if (!classes_.empty()) {
    CV_Assert(classId < (int) classes_.size());
    label = classes_[classId] + ": " + label;
  }

  int baseLine;
  cv::Size labelSize = getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 1, 1, &baseLine);

  top = cv::max(top, labelSize.height);
  rectangle(frame, cv::Point(left, top - labelSize.height),
            cv::Point(left + labelSize.width, top + baseLine), cv::Scalar::all(255), cv::FILLED);
  putText(frame, label, cv::Point(left, top), cv::FONT_HERSHEY_TRIPLEX, 1, cv::Scalar());
}

void Model::postprocess(cv::Mat &frame, const std::vector<cv::Mat> &outs) {
  static std::vector<int> outLayers = net_.getUnconnectedOutLayers();
  static std::string outLayerType = net_.getLayer(outLayers[0])->type;

  std::vector<int> classIds;
  std::vector<float> confidences;
  std::vector<cv::Rect> boxes;
  if (outLayerType == "DetectionOutput") {
    // Network produces output blob with a shape 1x1xNx7 where N is a number of
    // detections and an every detection is a vector of values
    // [batchId, classId, confidence, left, top, right, bottom]
    CV_Assert(!outs.empty());
    for (const auto &out : outs) {
      auto *data = (float *) out.data;
      for (size_t i = 0; i < out.total(); i += 7) {
        float confidence = data[i + 2];
        if (confidence > confThreshold_) {
          int left = (int) data[i + 3];
          int top = (int) data[i + 4];
          int right = (int) data[i + 5];
          int bottom = (int) data[i + 6];
          int width = right - left + 1;
          int height = bottom - top + 1;
          if (width <= 2 || height <= 2) {
            left = (int) (data[i + 3] * frame.cols);
            top = (int) (data[i + 4] * frame.rows);
            right = (int) (data[i + 5] * frame.cols);
            bottom = (int) (data[i + 6] * frame.rows);
            width = right - left + 1;
            height = bottom - top + 1;
          }
          classIds.push_back((int) (data[i + 1]) - 1);  // Skip 0th background class id.
          boxes.emplace_back(left, top, width, height);
          confidences.push_back(confidence);
        }
      }
    }
  } else if (outLayerType == "Region") {
    for (const auto &out : outs) {
      // Network produces output blob with a shape NxC where N is a number of
      // detected objects and C is a number of classes + 4 where the first 4
      // numbers are [center_x, center_y, width, height]
      auto *data = (float *) out.data;
      for (int j = 0; j < out.rows; ++j, data += out.cols) {
        cv::Mat scores = out.row(j).colRange(5, out.cols);
        cv::Point classIdPoint;
        double confidence;
        minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
        if (confidence > confThreshold_) {
          int centerX = (int) (data[0] * frame.cols);
          int centerY = (int) (data[1] * frame.rows);
          int width = (int) (data[2] * frame.cols);
          int height = (int) (data[3] * frame.rows);
          int left = centerX - width / 2;
          int top = centerY - height / 2;

          classIds.push_back(classIdPoint.x);
          confidences.push_back((float) confidence);
          boxes.emplace_back(left, top, width, height);
        }
      }
    }
  } else
    CV_Error(cv::Error::StsNotImplemented, "Unknown output layer type: " + outLayerType);

  currentPredictions = std::vector<Prediction>();
  std::vector<int> indices;
  cv::dnn::NMSBoxes(boxes, confidences, confThreshold_, nmsThreshold_, indices);
  for (int idx : indices) {
    if (classes_[classIds[idx]] == "?") { //Verify correct models
      continue;
    }
    cv::Rect box = boxes[idx];
    currentPredictions.emplace_back(frame(box).clone(), classes_[classIds[idx]]);
  }
  for (int idx : indices) {
    if (classes_[classIds[idx]] == "?"){ //Verify correct models
      continue;
    }
    cv::Rect box = boxes[idx];
    drawPred(classIds[idx], confidences[idx], box.x, box.y,
             box.x + box.width, box.y + box.height, frame);
  }
}

void Model::predict(cv::Mat &frame) {

  cv::Mat blob;

  preprocess(frame);

  std::vector<cv::Mat> outs;
  net_.forward(outs, outNames_);

  postprocess(frame, outs);

  currentFrame_ = frame;
}
