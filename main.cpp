#include "Aplicacion.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Aplicacion w;
    w.show();
    return a.exec();
}
