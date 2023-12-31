#ifndef APLICACION_H
#define APLICACION_H

#include <QMainWindow>
#include <QDir>
#include <QMediaPlayer>
#include <QPushButton>
#include <QKeyEvent>
#include <QProgressBar>
#include <QScrollBar>
#include <QLabel>
#include <QTimer>
#include <QTableWidgetItem>
#include <QMediaMetaData>
#include <QEventLoop>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <QSpinBox>
#include <QQueue>
#include "arduinoController.h"


class Aplicacion : public QMainWindow
{
    Q_OBJECT

public:
    explicit Aplicacion(QWidget* parent = nullptr);
    ~Aplicacion();

private slots:
    void loadSongs();
    void playSelectedSong(QTableWidgetItem* item);
    void togglePlayPause();
    void updateProgressBar(qint64 position);
    void forwardSong();
    void rewindSong();
    void togglePagination();
    void handleScroll(int value);
    void nextPage();
    void previousPage();
    void updateSongView();
    void updateMemoryUsage();
    void updatePageInfoLabel();
    void sortSongsByArtist();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    // Atributos
    QTableWidget* tableWidget;
    QProgressBar* songProgressBar;
    QMediaPlayer* player;
    QPushButton* togglePaginationButton;
    QPushButton* nextPageButton;
    QPushButton* previousPageButton;
    QLabel* memoryUsageLabel;
    QLabel* currentMemoryUsage;
    QTimer* memoryUpdateTimer;
    QLabel* pageInfoLabel;
    QLabel *songTitleLabel;
    QLabel *songAlbumLabel;
    QLabel *songArtistLabel;
    QLabel *songYearLabel;
    QSpinBox* itemsPerPageSpinBox;
    ArduinoController *arduinoController;


    // Otros atributos de tu clase
    bool paginationEnabled;
    int itemsPerPage;
    int currentPageIndex;
    static bool isPaused;
    QStringList allSongs;
    QMap<QString, QString> songPathMapping;
    QMap<QString, QString> songArtistMapping; // Almacena el artista relacionado con el nombre de la canción
    QQueue<QStringList> songPages;
    bool firstRowLit = false;



};

#endif // APLICACION_H
