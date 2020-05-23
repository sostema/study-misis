#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow) {

  ui_->setupUi(this);
  process_ = false;

  ui_->modeSelect->setCurrentIndex(0);
  ui_->processFileButton->setEnabled(false);
  ui_->makePhotoButton->setEnabled(false);

  QString temporaryDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
  QFile::copy(QString::fromStdString(urlsPath_), temporaryDir + "/urls.txt");
  QFile::copy(QString::fromStdString(labelsPath_), temporaryDir + "/labels.txt");
  QFile::copy(QString::fromStdString(modelPath_), temporaryDir + "/model.pb");
  QFile::copy(QString::fromStdString(modelConfigPath_), temporaryDir + "/model.pbtxt");

  labelsPath_ = temporaryDir.toStdString() + "/labels.txt";
  modelPath_ = temporaryDir.toStdString() + "/model.pb";
  modelConfigPath_ = temporaryDir.toStdString() + "/model.pbtxt";
  urlsPath_ = temporaryDir.toStdString() + "/urls.txt";

  configPath_ = QString(qApp->applicationDirPath() + "/config.txt").toStdString();

  std::ifstream ifs_labels(labelsPath_);
  std::ifstream ifs_urls(urlsPath_);
  std::ifstream ifs_model(modelPath_);
  std::ifstream ifs_modelConfig(modelConfigPath_);
  std::ifstream ifs_config(configPath_);

  if (!ifs_labels.is_open())
    CV_Error(cv::Error::StsError, "File " + labelsPath_ + " not found");
  if (!ifs_urls.is_open())
    CV_Error(cv::Error::StsError, "File " + urlsPath_ + " not found");
  if (!ifs_model.is_open())
    CV_Error(cv::Error::StsError, "File " + modelPath_ + " not found");
  if (!ifs_modelConfig.is_open())
    CV_Error(cv::Error::StsError, "File " + modelConfigPath_ + " not found");

  std::string line;
  size_t i = 0;
  while (std::getline(ifs_labels, line)){
    std::string label_line{line};
    std::getline(ifs_urls, line);
    std::string url_line{line};
    labels_.insert(std::make_pair(i, label_line));
    urls_.insert(std::make_pair(i, url_line));
    i++;
  }

  if (ifs_config.is_open())
    while (std::getline(ifs_config, line)){

      auto index = line.find('=');
      std::pair<std::string,std::string> keyVal;
      if (index != std::string::npos) {
        keyVal = std::make_pair(
            line.substr(0,index),
            line.substr(index+1)
        );
        while (!keyVal.second.empty() && keyVal.second.front() == ' ') {
          keyVal.second.erase(0,1);
        }
      }
      config_.insert(keyVal);
    }
}

MainWindow::~MainWindow() {
  delete ui_;
}

void MainWindow::startVideoCapture() {
  video_capture_.open(0);
  if(video_capture_.isOpened()){
    process_ = true;
    ui_->beginCameraButton->setText("Закончить");
    ui_->makePhotoButton->setEnabled(true);
  }
  qApp->processEvents();
}

void MainWindow::endVideoCapture() {
  if(video_capture_.isOpened()){
    process_ = false;
    video_capture_.release();
    ui_->beginCameraButton->setText("Начать");
    ui_->makePhotoButton->setEnabled(false);
  }
  qApp->processEvents();
}

void MainWindow::on_beginCameraButton_clicked() {
  if (process_) {
    endVideoCapture();
  } else {
    startVideoCapture();
    auto *frameWorker = new QWorker(modelPath_, modelConfigPath_, labels_, config_);

    QObject::connect(frameWorker, &QWorker::frameReady, this, &MainWindow::update_main_graphics);

    while (process_) {
      cv::Mat frame;
      video_capture_ >> frame;
      frameWorker->throughFrame(frame);
    }
  }
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (process_) {
    endVideoCapture();
  }
  event->accept();
}

void MainWindow::on_chooseFileButton_clicked() {
  QString fname = QFileDialog::getOpenFileName(this, "Выберите фото", QString("~/"), "Photos (*.jpg *.jpeg *.png)");
  if (QFile::exists(fname))
    ui_->filePath->setText(fname);
  pixmap_.load(ui_->filePath->text());
  applyPixmap();
  ui_->processFileButton->setEnabled(true);
  qApp->processEvents();
}

void MainWindow::applyPixmap() {
  if (pixmap_.isNull()) return;
  ui_->videoView->setPixmap(pixmap_.scaled(ui_->videoView->size(), Qt::KeepAspectRatio));
  qApp->processEvents();
}

void MainWindow::update_main_graphics(const cv::Mat &frame) {
  pixmap_ = QPixmap::fromImage(QImage(frame.data,
                                      frame.cols,
                                      frame.rows,
                                      frame.step,
                                      QImage::Format_RGB888).rgbSwapped());
  applyPixmap();
}

void MainWindow::processFrame(cv::Mat frame) {
  auto *frameWorker = new QWorker(modelPath_, modelConfigPath_, labels_, config_);
  QObject::connect(frameWorker, &QWorker::frameReady, this, &MainWindow::update_main_graphics);
  QObject::connect(frameWorker, &QWorker::predictionsReady, this, &MainWindow::add_products);
  frameWorker->processFrame(frame);
}

void MainWindow::on_makePhotoButton_clicked() {
  cv::Mat frame;
  video_capture_ >> frame;

  processFrame(frame);

  endVideoCapture();

  qApp->processEvents();
}

void MainWindow::on_processFileButton_clicked() {
  QString filePath = ui_->filePath->text();
  cv::Mat frame = cv::imread(filePath.toStdString());

  processFrame(frame);

  qApp->processEvents();
}

void MainWindow::on_modeSelect_currentIndexChanged(int v) {
  if (v != 0) {
    ui_->processFileButton->setEnabled(false);
  }
  ui_->filePath->clear();
  pixmap_ = QPixmap();
  applyPixmap();
  qApp->processEvents();
}

void MainWindow::add_products(const std::vector<Model::Prediction> &predictions) {
  QWidget *scrollArea = ui_->scrollAreaWidgetContents;
  while (QWidget *w = scrollArea->findChild<QWidget *>())
    delete w;
  for (const Model::Prediction &prediction : predictions) {
    QImage curqimage = QImage(prediction.frame_.data,
                              prediction.frame_.cols,
                              prediction.frame_.rows,
                              prediction.frame_.step,
                              QImage::Format_RGB888);
    for (auto const& element : urls_) {
      std::string str = element.second;
      if (str == "?") continue;
      std::size_t pos = str.find(prediction.label_);
      if (pos == std::string::npos) continue;
      auto *predictionItem = new QItemBox(0,
                                          curqimage.copy(),
                                          str.substr(pos + prediction.label_.length() + 1),
                                          prediction.label_);
      scrollArea->layout()->addWidget(predictionItem);
      connect(predictionItem, &QItemBox::delete_from_layout, [scrollArea, predictionItem]() {
        scrollArea->layout()->removeWidget(predictionItem);
        predictionItem->disconnect();
        delete predictionItem;
      });
      break;
    }
  }
}
void MainWindow::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  applyPixmap();
}

