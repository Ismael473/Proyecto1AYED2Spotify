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
    void togglePagination();
    void nextPage();
    void previousPage();

private:
    QMediaPlayer* player;
    QProgressBar* songProgressBar;
    static bool isPaused;
    QListView* listView;            // Para actualizarlo posteriormente
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
    QHash<QString, QString> songPathMapping;
};
