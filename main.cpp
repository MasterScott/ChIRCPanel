#include "mainwindow.hpp"
#include <QApplication>
#include <memory>

std::unique_ptr<MainWindow> w;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    w = std::make_unique<MainWindow>();
    w->show();
    return a.exec();
}
