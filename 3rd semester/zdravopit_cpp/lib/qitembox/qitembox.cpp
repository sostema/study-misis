#include "qitembox.h"

#include <utility>

QItemBox::QItemBox(QWidget *parent, const QImage q_image, std::string item_url, std::string label)
    : QGroupBox(parent), ui_(new Ui::QItemBox), item_url_(std::move(item_url)), label_(std::move(label)) {
  ui_->setupUi(this);
  ui_->label->setPixmap(QPixmap::fromImage(q_image.rgbSwapped()).scaled(100, 100));

  ui_->itemLine->setText(QString::fromStdString(label_));
}

QItemBox::~QItemBox() {
  delete ui_;
}

void QItemBox::on_deleteButton_pressed() {
  emit delete_from_layout();
}

void QItemBox::on_buyButton_pressed() {
  QString link = QString::fromStdString(item_url_);
  QDesktopServices::openUrl(QUrl(link));
}
