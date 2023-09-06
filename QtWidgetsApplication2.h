#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication2.h"
#include <QMediaPlayer>
#include <QProgressBar>
#include <QHash>

class QtWidgetsApplication2 : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApplication2(QWidget* parent = nullptr);
    ~QtWidgetsApplication2();
    void togglePlayPause();
    void forwardSong();
    void rewindSong();


protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void playSelectedSong(const QModelIndex& index);
    void updateProgressBar(qint64 position);  


private:
    QMediaPlayer* player;
    static bool isPaused;
    QProgressBar* songProgressBar; 
    Ui::QtWidgetsApplication2Class ui;
    QHash<QString, QString> songPathMapping;
};
