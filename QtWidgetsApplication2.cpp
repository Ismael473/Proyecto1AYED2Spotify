#include "QtWidgetsApplication2.h"
#include <QDir>
#include <QMediaPlayer>
#include <QPushButton> 
#include <QKeyEvent>
#include <QProgressBar>

// Inicialización de la variable estática
bool QtWidgetsApplication2::isPaused = false;

QtWidgetsApplication2::QtWidgetsApplication2(QWidget* parent)
    : QMainWindow(parent),
    paginationEnabled(false),
    itemsPerPage(30),
    currentPageIndex(0)
{
    // Configuraciones iniciales
    listView = new QListView(this);
    listView->setGeometry(0, 0, 900, 900);
    this->resize(1300, 1000);

    QDir mainDirectory("canciones");
    QStringList subfolders = mainDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString& subfolder : subfolders) {
        QDir subDir(mainDirectory.absoluteFilePath(subfolder));
        QStringList currentSongs = subDir.entryList(QStringList() << "*.mp3", QDir::Files);
        for (const QString& song : currentSongs) {
            allSongs.append(QFileInfo(song).baseName());
            songPathMapping.insert(QFileInfo(song).baseName(), subDir.absoluteFilePath(song));
        }
    }

    model = new QStringListModel(allSongs, this);
    listView->setModel(model);
    connect(listView, &QListView::clicked, this, &QtWidgetsApplication2::playSelectedSong);

    songProgressBar = new QProgressBar(this);
    songProgressBar->setGeometry(10, 910, 880, 20);
    songProgressBar->setRange(0, 30000);

    player = new QMediaPlayer(this);
    connect(player, &QMediaPlayer::positionChanged, this, &QtWidgetsApplication2::updateProgressBar);

    QPushButton* pauseResumeButton = new QPushButton("Pause", this);
    pauseResumeButton->setGeometry(910, 10, 80, 30);
    connect(pauseResumeButton, &QPushButton::clicked, this, &QtWidgetsApplication2::togglePlayPause);

    QPushButton* forwardButton = new QPushButton(" (3s) >>", this);
    forwardButton->setGeometry(1000, 10, 80, 30);
    connect(forwardButton, &QPushButton::clicked, this, &QtWidgetsApplication2::forwardSong);

    QPushButton* rewindButton = new QPushButton("<< (3s)", this);
    rewindButton->setGeometry(1090, 10, 80, 30);
    connect(rewindButton, &QPushButton::clicked, this, &QtWidgetsApplication2::rewindSong);

    // Botones de paginación
    togglePaginationButton = new QPushButton("Toggle Pagination", this);
    togglePaginationButton->setGeometry(910, 50, 150, 30);
    connect(togglePaginationButton, &QPushButton::clicked, this, &QtWidgetsApplication2::togglePagination);

    nextPageButton = new QPushButton("Next", this);
    nextPageButton->setGeometry(990, 90, 70, 30);
    connect(nextPageButton, &QPushButton::clicked, this, &QtWidgetsApplication2::nextPage);

    previousPageButton = new QPushButton("Previous", this);
    previousPageButton->setGeometry(910, 90, 70, 30);
    connect(previousPageButton, &QPushButton::clicked, this, &QtWidgetsApplication2::previousPage);
}

QtWidgetsApplication2::~QtWidgetsApplication2()
{
    delete player;
    delete model;
}

void QtWidgetsApplication2::playSelectedSong(const QModelIndex& index)
{
    QString songName = index.data().toString();
    QString selectedSongPath = songPathMapping.value(songName);
    player->setMedia(QUrl::fromLocalFile(selectedSongPath));
    player->play();
    isPaused = false;
}

void QtWidgetsApplication2::togglePlayPause()
{
    if (isPaused) {
        player->play();
        isPaused = false;
    }
    else {
        player->pause();
        isPaused = true;
    }
}

void QtWidgetsApplication2::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Space) {
        togglePlayPause();
    }
    QMainWindow::keyPressEvent(event);
}

void QtWidgetsApplication2::updateProgressBar(qint64 position)
{
    songProgressBar->setValue(position);
}

void QtWidgetsApplication2::forwardSong()
{
    qint64 position = player->position();
    player->setPosition(position + 3000);
}

void QtWidgetsApplication2::rewindSong()
{
    qint64 position = player->position();
    player->setPosition(position - 3000);
}

void QtWidgetsApplication2::togglePagination()
{
    paginationEnabled = !paginationEnabled;
    updateSongView();
}

void QtWidgetsApplication2::nextPage()
{
    if (currentPageIndex < (allSongs.count() - 1) / itemsPerPage) {
        currentPageIndex++;
        updateSongView();
    }
}

void QtWidgetsApplication2::previousPage()
{
    if (currentPageIndex > 0) {
        currentPageIndex--;
        updateSongView();
    }
}

void QtWidgetsApplication2::updateSongView()
{
    QStringList songsToDisplay;

    if (paginationEnabled) {
        for (int i = currentPageIndex * itemsPerPage; i < (currentPageIndex + 1) * itemsPerPage && i < allSongs.count(); ++i) {
            songsToDisplay.append(allSongs[i]);
        }
    }
    else {
        songsToDisplay = allSongs;
    }

    delete model;
    model = new QStringListModel(songsToDisplay, this);
    listView->setModel(model);
}
