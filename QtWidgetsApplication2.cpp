#include "QtWidgetsApplication2.h"
#include <QListView>
#include <QDir>
#include <QStringListModel>
#include <QMediaPlayer>
#include <QPushButton> 
#include <QKeyEvent>
#include <QProgressBar>

//aaaaa
bool QtWidgetsApplication2::isPaused = false;

QtWidgetsApplication2::QtWidgetsApplication2(QWidget* parent)
    : QMainWindow(parent)
{
    // Crea el QListView
    QListView* listView = new QListView(this);
    listView->setGeometry(0, 0, 900, 900);
    this->resize(1300, 1000);

    // Lista para almacenar todas las canciones
    QStringList songs;
    QDir mainDirectory("canciones");
    QStringList subfolders = mainDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    // Recorre cada subcarpeta
    for (const QString& subfolder : subfolders) {
        QDir subDir(mainDirectory.absoluteFilePath(subfolder));
        QStringList currentSongs = subDir.entryList(QStringList() << "*.mp3", QDir::Files);

        // Agrega las canciones (solo el nombre) a la lista principal y mapea el nombre al path completo
        for (const QString& song : currentSongs) {
            songs.append(QFileInfo(song).baseName());
            songPathMapping.insert(QFileInfo(song).baseName(), subDir.absoluteFilePath(song));
        }
    }

    // Configura la QStringListModel con las canciones y la asigna al QListView
    QStringListModel* model = new QStringListModel(songs, this);
    listView->setModel(model);

    // Botón para adelantar la canción
    QPushButton* forwardButton = new QPushButton(" (3s) >>", this);
    forwardButton->setGeometry(1000, 10, 80, 30);
    connect(forwardButton, &QPushButton::clicked, this, &QtWidgetsApplication2::forwardSong);

    // Botón para retrasar la canción
    QPushButton* rewindButton = new QPushButton("<< (3s)", this);
    rewindButton->setGeometry(1090, 10, 80, 30);
    connect(rewindButton, &QPushButton::clicked, this, &QtWidgetsApplication2::rewindSong);

    // Crea el botón de pausa/reanudación
    QPushButton* pauseResumeButton = new QPushButton("Pause", this);
    pauseResumeButton->setGeometry(910, 10, 80, 30);
    connect(pauseResumeButton, &QPushButton::clicked, this, &QtWidgetsApplication2::togglePlayPause);

    // Crea la barra de progreso
    songProgressBar = new QProgressBar(this);
    songProgressBar->setGeometry(10, 910, 880, 20);
    songProgressBar->setRange(0, 30000); // Duración máxima 30s en milisegundos

    // Configura el reproductor
    player = new QMediaPlayer(this);
    connect(player, &QMediaPlayer::positionChanged, this, &QtWidgetsApplication2::updateProgressBar); // Conexión para actualizar la barra
    connect(listView, &QListView::clicked, this, &QtWidgetsApplication2::playSelectedSong);
}

QtWidgetsApplication2::~QtWidgetsApplication2()
{
    delete player;
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
    QMainWindow::keyPressEvent(event); // Llama al método base para manejar otros eventos de teclas
}

void QtWidgetsApplication2::updateProgressBar(qint64 position)
{
    songProgressBar->setValue(position);
}

void QtWidgetsApplication2::forwardSong() {
    qint64 position = player->position();
    player->setPosition(position + 3000); // Adelanta 3 segundos (3000 milisegundos)
}

void QtWidgetsApplication2::rewindSong() {
    qint64 position = player->position();
    player->setPosition(position - 3000); // Retrasa 3 segundos (3000 milisegundos)
}
