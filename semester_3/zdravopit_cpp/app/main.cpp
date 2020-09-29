#include <mainwindow.h>
#include <QApplication>

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  MainWindow mainWindow;
  mainWindow.setWindowTitle("Zdravopit");
  mainWindow.show();
  return QApplication::exec();
}