#include "MainWindow.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GomokuWindow w;
    w.show();

    return a.exec();
}
