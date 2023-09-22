#ifndef ARDUINOCONTROLLER_H
#define ARDUINOCONTROLLER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

class ArduinoController : public QObject
{
    Q_OBJECT

public:
    explicit ArduinoController(QObject *parent = nullptr);
    ~ArduinoController();

    bool connectToArduino(const QString &portName);
    void sendProgressValue(int value);
    void resetSignalSent();

signals:
    void buttonPressed();

private slots:
    void onDataReceived();

private:
    QSerialPort serial;
    bool sentSignal;  // Declaración de sentSignal
    char lastSentSignal;
};

#endif // ARDUINOCONTROLLER_H
