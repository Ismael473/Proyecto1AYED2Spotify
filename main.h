#ifndef MAIN_H
#define MAIN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Main; }
QT_END_NAMESPACE

class Main : public QMainWindow
{
    /**
     * Función encargada de correr el programa
    */
    Q_OBJECT

public:
    Main(QWidget *parent = nullptr);
    ~Main() override{};

private:
    Ui::Main *ui;
};
#endif // MAIN_H
