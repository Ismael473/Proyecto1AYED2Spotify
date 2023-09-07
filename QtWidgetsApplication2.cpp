#include "QtWidgetsApplication2.h"
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




// Inicialización de la variable estática
bool QtWidgetsApplication2::isPaused = false;

QtWidgetsApplication2::QtWidgetsApplication2(QWidget* parent)
    : QMainWindow(parent),
    paginationEnabled(false),
    itemsPerPage(200),
    currentPageIndex(0)
{
    // Configuraciones iniciales
    tableWidget = new QTableWidget(this);
    tableWidget->setGeometry(0, 0, 900, 900);
    tableWidget->setColumnCount(1); // Establecer una sola columna
    tableWidget->setHorizontalHeaderLabels({ "Songs" });
    this->resize(1300, 1000);

    // Botón para cargar las canciones
    QPushButton* loadSongsButton = new QPushButton("Cargar librerias de canciones", this);
    loadSongsButton->setGeometry(910, 130, 250, 30);
    connect(loadSongsButton, &QPushButton::clicked, this, &QtWidgetsApplication2::loadSongs);

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


    memoryUsageLabel = new QLabel("Memoria en uso", this);
    memoryUsageLabel->setGeometry(910, 170, 150, 20);  //Ubicacion

    currentMemoryUsage = new QLabel("0000 MB / 2000 MB", this);
    currentMemoryUsage->setGeometry(910, 190, 150, 20);  // Ubicacion

    memoryUpdateTimer = new QTimer(this);
    connect(memoryUpdateTimer, &QTimer::timeout, this, &QtWidgetsApplication2::updateMemoryUsage);
    memoryUpdateTimer->start(1000);  // Cada 1 segundos



    // Etiqueta de información de página
    pageInfoLabel = new QLabel(this);
    pageInfoLabel->setGeometry(910, 210, 160, 20);



    // Conectar el slot de playSelectedSong
    connect(tableWidget, &QTableWidget::itemClicked, this, &QtWidgetsApplication2::playSelectedSong);

    // Nueva conexión para manejar el desplazamiento
    connect(tableWidget->verticalScrollBar(), &QScrollBar::valueChanged, this, &QtWidgetsApplication2::handleScroll);
}

QtWidgetsApplication2::~QtWidgetsApplication2()
{
    delete player;
}

void QtWidgetsApplication2::loadSongs() {
    QDir mainDirectory("canciones");
    QStringList subfolders = mainDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString& subfolder : subfolders) {
        QDir subDir(mainDirectory.absoluteFilePath(subfolder));
        QStringList currentSongs = subDir.entryList(QStringList() << "*.mp3", QDir::Files);
        for (const QString& songFile : currentSongs) {
            QString fullPath = subDir.absoluteFilePath(songFile);

            TagLib::FileRef file(fullPath.toStdString().c_str());
            if (!file.isNull() && file.tag()) {
                TagLib::Tag* tag = file.tag();
                QString title = QString::fromStdString(tag->title().toCString(true));  // Convert title from TagLib::String to QString

                if (!title.isEmpty()) {
                    allSongs.append(title);
                }
                else {
                    allSongs.append(QFileInfo(songFile).baseName()); // Default to filename if title is empty
                }
            }
            else {
                allSongs.append(QFileInfo(songFile).baseName()); // Default to filename if file is invalid or has no tags
            }
            songPathMapping.insert(allSongs.last(), fullPath);
        }
    }

    updateSongView();
}

void QtWidgetsApplication2::playSelectedSong(QTableWidgetItem* item)
{
    QString songName = item->text();
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
    updatePageInfoLabel();
}

void QtWidgetsApplication2::handleScroll(int value) {
    QScrollBar* verticalScrollBar = tableWidget->verticalScrollBar();

    disconnect(verticalScrollBar, &QScrollBar::valueChanged, this, &QtWidgetsApplication2::handleScroll);  // Desconectar la señal

    // Si estamos al final de la barra de desplazamiento y hay una siguiente página
    if (value == verticalScrollBar->maximum() && currentPageIndex < (allSongs.count() - 1) / itemsPerPage) {
        nextPage();
    }
    // Si estamos al inicio de la barra de desplazamiento y no estamos en la primera página
    else if (value == verticalScrollBar->minimum() && currentPageIndex > 0) {
        previousPage();
    }

    connect(verticalScrollBar, &QScrollBar::valueChanged, this, &QtWidgetsApplication2::handleScroll);  // Reconectar la señal
}

void QtWidgetsApplication2::nextPage() {
    if (currentPageIndex < (allSongs.count() - 1) / itemsPerPage) {
        currentPageIndex++;
        updateSongView();
        tableWidget->verticalScrollBar()->setValue(tableWidget->verticalScrollBar()->minimum());
        updatePageInfoLabel();
    }
}

void QtWidgetsApplication2::previousPage() {
    if (currentPageIndex > 0) {
        currentPageIndex--;
        updateSongView();
        tableWidget->verticalScrollBar()->setValue(tableWidget->verticalScrollBar()->maximum());
        updatePageInfoLabel();
    }
}
void QtWidgetsApplication2::updateSongView() {
    tableWidget->setRowCount(0); // Resetear filas
    QStringList songsToDisplay;

    if (paginationEnabled) {
        for (int i = currentPageIndex * itemsPerPage; i < (currentPageIndex + 1) * itemsPerPage && i < allSongs.count(); ++i) {
            songsToDisplay.append(allSongs[i]);
        }
    }
    else {
        songsToDisplay = allSongs;
    }

    for (const QString& song : songsToDisplay) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(song));
    }

    // Restablecer la posición de desplazamiento para evitar problemas
    tableWidget->verticalScrollBar()->setValue(0);
    updatePageInfoLabel();
}

void QtWidgetsApplication2::updateMemoryUsage() {
    int currentMemory;
    int minRange, maxRange, change;

    if (paginationEnabled) {
        minRange = 400;
        maxRange = 700;
    }
    else {
        minRange = 1200;
        maxRange = 1800;
    }

    // Obtener el valor actual
    QStringList parts = currentMemoryUsage->text().split(" ");
    currentMemory = parts[0].toInt();

    change = qrand() % 151;  // Valor entre 0 y 150
    if (qrand() % 2 == 0) {
        change = -change;  // La mitad del tiempo, hacemos el cambio negativo
    }

    // Aplicar el cambio y mantenerlo dentro del rango
    currentMemory += change;
    currentMemory = qBound(minRange, currentMemory, maxRange);

    // Actualizar la etiqueta con el nuevo valor
    currentMemoryUsage->setText(QString::number(currentMemory) + " MB / 2000 MB");
}

void QtWidgetsApplication2::updatePageInfoLabel() {
    if (paginationEnabled) {
        int totalPages = (allSongs.count() + itemsPerPage - 1) / itemsPerPage;
        pageInfoLabel->setText(QString("Page %1 of %2").arg(currentPageIndex + 1).arg(totalPages));
    }
    else {
        pageInfoLabel->setText("");  // Vaciamos el texto si la paginación no está habilitada
    }
}
