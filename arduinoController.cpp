#include "arduinoController.h"

ArduinoController::ArduinoController(QObject *parent)
    : QObject(parent), lastSentSignal('0')
{
}

ArduinoController::~ArduinoController()
{
    if (serial.isOpen()) {
        serial.close();
    }
}

bool ArduinoController::connectToArduino(const QString &portName)
{ 
    /**
     * Función encargada de conectar el software de la computadora con el del arduino
     * @return true para establecer que se llegó a establecer una conexión exitosa
    */
    serial.setPortName(portName);
    serial.setBaudRate(QSerialPort::Baud9600);

    if (!serial.open(QIODevice::ReadWrite)) {
        return false;  // Falló la conexión.
    }

    connect(&serial, &QSerialPort::readyRead, this, &ArduinoController::onDataReceived);

    return true;
}
void ArduinoController::sendProgressValue(int value)
{
    /**
     * Función encargada de enviar un valor de barra de progreso al arduino
     * @return Una barra de progeso en hardware creada con leds
    */
    char signalToSend = '0';  // Definimos '0' como señal por defecto (no hará nada en el Arduino).

    if (value < 2300 || value > 29000) {
        signalToSend = 'X';  // Apagar todas las filas
    } else if (value > 28000) {
        signalToSend = 'H';
    } else if (value > 24100) {
        signalToSend = 'G';
    } else if (value > 19800) {
        signalToSend = 'F';
    } else if (value > 16500) {
        signalToSend = 'E';
    } else if (value > 13200) {
        signalToSend = 'D';
    } else if (value > 9900) {
        signalToSend = 'C';
    } else if (value > 6600) {
        signalToSend = 'B';
    } else if (value > 3300) {
        signalToSend = 'A';
    }

    // Solo enviamos la señal si ha cambiado desde la última vez
    if (signalToSend != lastSentSignal) {
        serial.write(&signalToSend, 1);  // Enviamos la señal al Arduino.
        lastSentSignal = signalToSend;  // Actualizamos la última señal enviada.
    }
}

void ArduinoController::resetSignalSent()
{
    /**
     * Función encargada de reestablecer los valores de las señales utilizadas
     * @return establece las señales a 0
    */
    lastSentSignal = '0';  // Restablecemos la última señal enviada a '0'.
}
void ArduinoController::onDataReceived()
{
    /**
     * Función encargada de obtener la información recibida por el arduino
     * @return La información recibida por el arduino
    */
    QByteArray data = serial.readAll();
    if (data.contains('P')) {
        emit buttonPressed();
    }
}

