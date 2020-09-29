#include <QMainWindow>
#include <QMessageBox>
#include <QThread>
#include <QDebug>
#include <QFile>
#include <QGroupBox>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QCloseEvent>
#include <QFileDialog>
#include <QStandardPaths>
#include "ui_mainwindow.h"

#include <opencv2/opencv.hpp>

#include <qitembox/qitembox.h>
#include <qworker/qworker.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//!  Основной класс для графического интерфейса.
class MainWindow : public QMainWindow {
 Q_OBJECT

 public:
  //! Конструктор Qt.
  explicit MainWindow(QWidget *parent = 0);

  //! Деструктор Qt.
  ~MainWindow() override;

  //! Событие закрытия.
  /*!
    При закрытии приложения проверяет, открыт ли видеопоток, и если да, то закрывает его.
  */
  void closeEvent(QCloseEvent *event) override;

 private slots:
  //! Событие при нажатии кнопки выбора файла.
  void on_chooseFileButton_clicked();
  //! Событие при нажатии кнопки обработки файла.
  void on_processFileButton_clicked();
  //! Событие при нажатии кнопки начала видеопотока.
  void on_beginCameraButton_clicked();
  //! Событие при нажатии кнопки снимка текущего кадра видеопотока.
  void on_makePhotoButton_clicked();
  //! Событие изменения размера окна.
  void resizeEvent(QResizeEvent *event) override;

  //! Событие при изменении текущего состояния.
  /*!
    Изменяет интерфейс при смене текущего состояния (фото/видео).
    \param v текущий индекс
  */
  void on_modeSelect_currentIndexChanged(int v);
  //! Явно изменяет текущую картинку.
  /*!
    Изменяет текущую картинку на QLabel на новую картинку.
    \param frame картинка, которая будет показываться.
  */
  void update_main_graphics(const cv::Mat &frame);
  //! Добавляет продукты в список справа.
  /*!
    \param predictions вектор предсказаний.
  */
  void add_products(const std::vector<Model::Prediction> &predictions);

 private:
  Ui::MainWindow *ui_{}; //!< Интерфейс объекта класса MainWindow.
  QPixmap pixmap_{}; //!< QPixmap, который используется для преобразования cv::Mat для использования в QLabel.

  bool process_{false}; //!< Текущее состояние видеопотока.
  cv::VideoCapture video_capture_; //!< Видеопоток.

  std::string modelConfigPath_{":/model.pbtxt"}; //!< Путь к описанию модели.
  std::string modelPath_{":/model.pb"}; //!< Путь к модели.
  std::string labelsPath_{":/labels.txt"}; //!< Путь к файлу с текстовыми метками.
  std::string urlsPath_{":/urls.txt"}; //!< Путь к файлу с ссылками на товары.

  std::string configPath_{}; //!< Путь к файлу с конфигурацией.

  std::map<int, std::string> labels_; //!< Словарь пар номер-название для классов.
  std::map<int, std::string> urls_; //!< Словарь пар номер-ссылка для классов.
  std::map<std::string, std::string> config_; //!< Словарь конфигурационного файла.

  void startVideoCapture(); //!< Метод для явного начала видеопотока.
  void endVideoCapture(); //!< Метод для явного окончания видеопотока.
  void applyPixmap(); //!< Метод для явного изменения QLabel.

  //! Метод для вызова обработки кадра.
  /*!
    \param frame кадр для обработки.
  */
  void processFrame(cv::Mat frame);
};