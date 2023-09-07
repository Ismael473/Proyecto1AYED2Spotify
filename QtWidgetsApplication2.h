#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication2.h"
#include <QMediaPlayer>
#include <QProgressBar>
#include <QHash>
#include <QListView>
#include <QPushButton>
#include <QStringListModel>
#include <QLabel>
#include <QMap>
#include <QModelIndex>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTableWidget>

class QtWidgetsApplication2 : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApplication2(QWidget* parent = nullptr);
    ~QtWidgetsApplication2();
    void togglePlayPause();
    void forwardSong();
    void rewindSong();
    void loadSongs();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void playSelectedSong(QTableWidgetItem* item);
    void updateProgressBar(qint64 position);
    void togglePagination();
    void nextPage();
    void previousPage();
    void handleScroll(int position);
    void updateMemoryUsage();

private:
    QMediaPlayer* player;
    QProgressBar* songProgressBar;
    static bool isPaused;
    QTableWidget* tableWidget;         // Para actualizarlo posteriormente
    QStringListModel* model;       

    // Botones de paginación
    QPushButton* togglePaginationButton;
    QPushButton* nextPageButton;
    QPushButton* previousPageButton;

    // Variables para la paginación
    bool paginationEnabled;
    int itemsPerPage;
    int currentPageIndex;
    QStringList allSongs;
    QLabel* pageInfoLabel;

    // Funciones
    void updateSongView();         // Actualiza las canciones mostradas en el listView
    void updatePageInfoLabel();
    Ui::QtWidgetsApplication2Class ui;
    QLabel* memoryUsageLabel;
    QLabel* currentMemoryUsage;
    QTimer* memoryUpdateTimer;
    QHash<QString, QString> songPathMapping;
};
