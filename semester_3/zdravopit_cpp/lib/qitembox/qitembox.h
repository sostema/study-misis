#ifndef QITEMBOX_QITEMBOX_QITEMBOX_H_
#define QITEMBOX_QITEMBOX_QITEMBOX_H_

#include <QGroupBox>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDesktopServices>
#include <QUrl>
#include <QCoreApplication>
#include "ui_qitembox.h"

QT_BEGIN_NAMESPACE
namespace Ui { class QItemBox; }
QT_END_NAMESPACE


//! Класс представления продукта.
class QItemBox : public QGroupBox {
 Q_OBJECT

 public:
  //! Конструктор Qt.
  /*!
    \param parent родительский QWidget.
    \param q_image фото продукта из картинки.
    \param item_url ссылка на продукт.
    \param label название класса продукта.
  */
  explicit QItemBox(QWidget *parent = 0,
                    const QImage q_image = QImage(),
                    std::string item_url = "",
                    std::string label = "");
  //! Деструктор Qt.
  ~QItemBox() override;

 private slots:
  //! Событие при нажатии кнопки удаления.
  void on_deleteButton_pressed();
  //! Событие при нажатии кнопки покупки.
  void on_buyButton_pressed();

 signals:
  //! Сигнал при удалении.
  void delete_from_layout();

 private:
  Ui::QItemBox *ui_{}; //!< Интерфейс объекта класса QItemBox.

  QGraphicsScene scene_{}; //!< QGraphicsScene для QLabel.
  QGraphicsPixmapItem pixmap_{}; //!< QGraphicsPixmapItem для QGraphicsScene.

  std::string item_url_{}; //!< Строка, содержащая ссылку на продукт.
  std::string label_{}; //!< Строка, содержащая название класса продукта.

};

#endif
