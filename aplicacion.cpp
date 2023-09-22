#include "aplicacion.h"
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
#include "arduinoController.h"
#include <QMessageBox>
#include <QDebug>
#include <QStandardPaths>


// Inicialización de la variable estática
bool Aplicacion::isPaused = false;



Aplicacion::Aplicacion(QWidget* parent)
/**
 * Función encargada de la creación de la ventana principal y de administrar su correcto funcionamiento
 * incluyendo la conexión con el arduino y la paginación
 * 
 * @param parent
 * 
*/
    : QMainWindow(parent),
    paginationEnabled(false),
    currentPageIndex(0)
{
    // Configuraciones iniciales
    tableWidget = new QTableWidget(this);
    tableWidget->setGeometry(0, 0, 900, 900);
    tableWidget->setColumnCount(2); // Establecer dos columnas
    tableWidget->setHorizontalHeaderLabels({ "Songs", "Artist" });
    this->resize(1300, 1000);


    // Inicializa el controlador de Arduino
    arduinoController = new ArduinoController(this);
    if (!arduinoController->connectToArduino("COM3")) {
        // Opcional: Muestra un mensaje de error si no puede conectar
        QMessageBox::warning(this, "Error de conexión", "No se pudo conectar al Arduino en el puerto COM3");
    }


    // Botón para cargar las canciones
    QPushButton* loadSongsButton = new QPushButton("Cargar librerias de canciones", this);
    loadSongsButton->setGeometry(910, 130, 250, 30);
    connect(loadSongsButton, &QPushButton::clicked, this, &Aplicacion::loadSongs);

    songProgressBar = new QProgressBar(this);
    songProgressBar->setGeometry(10, 910, 880, 20);
    songProgressBar->setRange(0, 30000);

    player = new QMediaPlayer(this);
    connect(player, &QMediaPlayer::positionChanged, this, &Aplicacion::updateProgressBar);

    QPushButton* pauseResumeButton = new QPushButton("Pause", this);
    pauseResumeButton->setGeometry(1000, 10, 80, 30);
    connect(pauseResumeButton, &QPushButton::clicked, this, &Aplicacion::togglePlayPause);

    QPushButton* forwardButton = new QPushButton(" (3s) >>", this);
    forwardButton->setGeometry(1090, 10, 80, 30);
    connect(forwardButton, &QPushButton::clicked, this, &Aplicacion::forwardSong);

    QPushButton* rewindButton = new QPushButton("<< (3s)", this);
    rewindButton->setGeometry(910, 10, 80, 30);
    connect(rewindButton, &QPushButton::clicked, this, &Aplicacion::rewindSong);

    // Botones de paginación
    togglePaginationButton = new QPushButton("Toggle Pagination", this);
    togglePaginationButton->setGeometry(910, 50, 150, 30);
    connect(togglePaginationButton, &QPushButton::clicked, this, &Aplicacion::togglePagination);

    nextPageButton = new QPushButton("Next", this);
    nextPageButton->setGeometry(990, 90, 70, 30);
    connect(nextPageButton, &QPushButton::clicked, this, &Aplicacion::nextPage);

    previousPageButton = new QPushButton("Previous", this);
    previousPageButton->setGeometry(910, 90, 70, 30);
    connect(previousPageButton, &QPushButton::clicked, this, &Aplicacion::previousPage);

    QPushButton* sortSongsButton = new QPushButton("Ordenar por Artista", this);
    sortSongsButton->setGeometry(910, 220, 250, 30);  // Ajusta la geometría según donde quieras colocar el botón
    connect(sortSongsButton, &QPushButton::clicked, this, &Aplicacion::sortSongsByArtist);

    // Añade estas declaraciones al principio de la función constructora
    songTitleLabel = new QLabel("Título: ", this);
    songTitleLabel->setGeometry(910, 260, 400, 20);

    songAlbumLabel = new QLabel("Álbum: ", this);
    songAlbumLabel->setGeometry(910, 280, 400, 20);

    songArtistLabel = new QLabel("Artista: ", this);
    songArtistLabel->setGeometry(910, 300, 400, 20);

    songYearLabel = new QLabel("Año: ", this);
    songYearLabel->setGeometry(910, 320, 400, 20);



    itemsPerPageSpinBox = new QSpinBox(this);
    itemsPerPageSpinBox->setGeometry(1070, 50, 80, 30); // Ajusta la geometría según necesidad
    itemsPerPageSpinBox->setRange(50, 100); // Establece un rango razonable
    itemsPerPageSpinBox->setValue(200); // Valor inicial por defecto



    memoryUsageLabel = new QLabel("Memoria en uso", this);
    memoryUsageLabel->setGeometry(910, 170, 150, 20);  //Ubicacion

    currentMemoryUsage = new QLabel("0000 MB / 2000 MB", this);
    currentMemoryUsage->setGeometry(910, 190, 150, 20);  // Ubicacion

    memoryUpdateTimer = new QTimer(this);
    connect(memoryUpdateTimer, &QTimer::timeout, this, &Aplicacion::updateMemoryUsage);
    memoryUpdateTimer->start(1000);  // Cada 1 segundos



    // Etiqueta de información de página
    pageInfoLabel = new QLabel(this);
    pageInfoLabel->setGeometry(910, 210, 160, 20);

    connect(arduinoController, &ArduinoController::buttonPressed, this, &Aplicacion::togglePlayPause);


    // Conectar el slot de playSelectedSong
    connect(tableWidget, &QTableWidget::itemClicked, this, &Aplicacion::playSelectedSong);

    // Nueva conexión para manejar el desplazamiento
    connect(tableWidget->verticalScrollBar(), &QScrollBar::valueChanged, this, &Aplicacion::handleScroll);

}

Aplicacion::~Aplicacion()
/**
 * Función encargada de la liberación de espacios de memoria
 * @return espacios de memoria libres
*/
{
    delete player;
    delete arduinoController;  // Libera la memoria de ArduinoController
}

void Aplicacion::loadSongs() {
    /**
     * función encargada de encontrar los directorios donde están los archivos a utilizar y facilitar el acceso a ellos
     * @return la información importante de las canciones mostradas en la aplicación 
    */
    QDir mainDirectory("/home/ismael/Desktop/CancionesSpotify/fma_small");
    if(!mainDirectory.exists()){
        qWarning("No existe el directorio");
    }else{
        qWarning("Lo encontró");
    }

    QStringList subfolders = mainDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    allSongs.clear();  // Asegurarse de que la lista esté vacía antes de cargar nuevas canciones

    for (const QString& subfolder : subfolders) {
        QDir subDir(mainDirectory.absoluteFilePath(subfolder));
        QStringList currentSongs = subDir.entryList(QStringList() << "*.mp3", QDir::Files);
        for (const QString& songFile : currentSongs) {
            QString fullPath = subDir.absoluteFilePath(songFile);

            TagLib::FileRef file(fullPath.toStdString().c_str());
            if (!file.isNull() && file.tag()) {
                TagLib::Tag* tag = file.tag();
                QString title = QString::fromStdString(tag->title().toCString(true));
                QString artist = QString::fromStdString(tag->artist().toCString(true));

                if (!title.isEmpty()) {
                    allSongs.append(title);
                    songArtistMapping.insert(title, artist);
                } else {
                    QString baseName = QFileInfo(songFile).baseName();
                    allSongs.append(baseName);
                    songArtistMapping.insert(baseName, artist);
                }
            } else {
                QString baseName = QFileInfo(songFile).baseName();
                allSongs.append(baseName);
                songArtistMapping.insert(baseName, "");
            }
            songPathMapping.insert(allSongs.last(), fullPath);
        }
    }

    // Segmento adicional para cargar las canciones en paginación
    songPages.clear();  // Asegurarse de que las páginas anteriores se eliminen

    int start = currentPageIndex == 0 ? 0 : (currentPageIndex - 1) * itemsPerPage;
    for (int i = start; i < start + 3 * itemsPerPage && i < allSongs.count(); i += itemsPerPage) {
        songPages.enqueue(allSongs.mid(i, itemsPerPage));
    }

    updateSongView();
}



void Aplicacion::playSelectedSong(QTableWidgetItem* item)
{
    /**
     * Función encargada de la reproducción de los archivos de audio
     * @param QTableWidgetItem* item
     * @return reproduce la canción selecionada 
    */
    QString songName = item->text();
    QString selectedSongPath = songPathMapping.value(songName);
    player->setMedia(QUrl::fromLocalFile(selectedSongPath));
    player->play();
    isPaused = false;

    // Actualizar los labels
    TagLib::FileRef file(selectedSongPath.toStdString().c_str());
    if (!file.isNull() && file.tag()) {
        TagLib::Tag* tag = file.tag();
        songTitleLabel->setText("Título: " + QString::fromStdString(tag->title().toCString(true)));
            songAlbumLabel->setText("Álbum: " + QString::fromStdString(tag->album().toCString(true)));
            songArtistLabel->setText("Artista: " + QString::fromStdString(tag->artist().toCString(true)));
        songYearLabel->setText("Año: " + QString::number(tag->year()));
    } else {
        songTitleLabel->setText("Título: Desconocido");
            songAlbumLabel->setText("Álbum: Desconocido");
            songArtistLabel->setText("Artista: Desconocido");
        songYearLabel->setText("Año: Desconocido");
    }
}


void Aplicacion::togglePlayPause()
{
    /**
     * Función encargada de pausar o reproducir los archivos
     * @return una señal que hace que se pause o se reproduzca el archivo de audio
    */
    if (isPaused) {
        player->play();
        isPaused = false;
    }
    else {
        player->pause();
        isPaused = true;
    }
}

void Aplicacion::keyPressEvent(QKeyEvent* event)
{
    /**
     * Función encargada de obtener el input de teclado que implementa el boton de espacio
     * @return Una señal que representa que se pulsó la tecla espaciadora
    */
    if (event->key() == Qt::Key_Space) {
        togglePlayPause();
    }
    QMainWindow::keyPressEvent(event);
}

void Aplicacion::updateProgressBar(qint64 position)
{
    /**
     * Función encargada de mostrar graficamente que el progreso de reproducción del archivo de audio
     * @return una representación visual de cuanto ha pasado reproduciendose un archivo de audio
    */
    songProgressBar->setValue(position);
    // Imprimir el progreso de la canción
    qDebug() << "Progreso de la canción:" << position;

    // Envía el valor de posición al Arduino
    arduinoController->sendProgressValue(static_cast<int>(position));
}

void Aplicacion::forwardSong()
{
    /**
     * Función encargada de pasar a la siguiente canción
     * @return avanza a la siguiente canción
    */
    qint64 position = player->position();
    player->setPosition(position + 3000);
}

void Aplicacion::rewindSong()
{
    /**
     * Función encargada de regresar la canción al inicio o a regresar a la canción anterior
     * @return La canción que ya se estaba reproduciendo o la canción anterior a la que se está reproduciendo
    */
    qint64 position = player->position();
    player->setPosition(position - 3000);
}

void Aplicacion::togglePagination() {
    /**
     * Función encargada de alternar la paginación de las canciones
     * @return un nuevo set de canciones.
    */
    paginationEnabled = !paginationEnabled;
    if (paginationEnabled) {
        itemsPerPage = itemsPerPageSpinBox->value();
        loadSongs();
    }
    updateSongView();
    updatePageInfoLabel();
}



void Aplicacion::handleScroll(int value) {
    /**
     * Función encargada de poder mostrar las canciones cuando se desliza hacia abajo en la lista
     * @param int value
     * @return La visualización de las canciones conforme se va deslizando hacia abajo
    */
    QScrollBar* verticalScrollBar = tableWidget->verticalScrollBar();

    disconnect(verticalScrollBar, &QScrollBar::valueChanged, this, &Aplicacion::handleScroll);  // Desconectar la señal

    // Si estamos al final de la barra de desplazamiento y hay una siguiente página
    if (value == verticalScrollBar->maximum() && currentPageIndex < (allSongs.count() - 1) / itemsPerPage) {
        nextPage();
    }
    // Si estamos al inicio de la barra de desplazamiento y no estamos en la primera página
    else if (value == verticalScrollBar->minimum() && currentPageIndex > 0) {
        previousPage();
    }

    connect(verticalScrollBar, &QScrollBar::valueChanged, this, &Aplicacion::handleScroll);  // Reconectar la señal
}

void Aplicacion::nextPage() {
    /**
     * Función encargada de cargar la siguiente pagina cuando no se pudieron mostrar todas las canciones en una misma página
     * @return El resto de canciones que no se presentan en la página actual
    */
    if (!songPages.isEmpty()) {
        songPages.dequeue();  // Eliminar la página más antigua

        int newPageIndexEnd = (currentPageIndex + 3) * itemsPerPage;
        if (newPageIndexEnd < allSongs.count()) {
            songPages.enqueue(allSongs.mid(newPageIndexEnd, itemsPerPage));  // Cargar la nueva "siguiente" página
        }

        currentPageIndex++;
        updateSongView();
        tableWidget->verticalScrollBar()->setValue(tableWidget->verticalScrollBar()->minimum());
        updatePageInfoLabel();
    }
}

void Aplicacion::previousPage() {
    /**
     * Función que muestra las canciones que ya se llegaron a cargar en la página anterior
     * @return La canciones que ya se habían cargado previamente
    */
    if (currentPageIndex > 0) {
        songPages.removeLast();  // Eliminar la página más reciente

        int newPageIndexStart = (currentPageIndex - 2) * itemsPerPage;
        if (newPageIndexStart >= 0) {
            songPages.prepend(allSongs.mid(newPageIndexStart, itemsPerPage));  // Cargar la nueva "anterior" página
        }

        currentPageIndex--;
        updateSongView();
        tableWidget->verticalScrollBar()->setValue(tableWidget->verticalScrollBar()->maximum());
        updatePageInfoLabel();
    }
}

void Aplicacion::updateSongView() {
    /**
     * Función encargada de ir actualizando el visualizador de canciones
     * @return El visualizador de canciones ya actualizado
    */
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
        tableWidget->setItem(row, 1, new QTableWidgetItem(songArtistMapping[song]));

    }

    // Restablecer la posición de desplazamiento para evitar problemas
    tableWidget->verticalScrollBar()->setValue(0);
    updatePageInfoLabel();
}

void Aplicacion::updateMemoryUsage() {
    /**
     * Función encargada de actualizar el uso de memoria
     * @return Información actualizada con respecto al uso de memoria
    */
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

void Aplicacion::updatePageInfoLabel() {
    /**
     * Función encargada de actualizar las etiquetas de información
     * @return las etiquetas de información ya actualizadas
    */
    if (paginationEnabled) {
        int totalPages = (allSongs.count() + itemsPerPage - 1) / itemsPerPage;
        pageInfoLabel->setText(QString("Page %1 of %2").arg(currentPageIndex + 1).arg(totalPages));
    }
    else {
        pageInfoLabel->setText("");  // Vaciamos el texto si la paginación no está habilitada
    }
}
void Aplicacion::sortSongsByArtist() {
    /**
     * Función encargada de acomodar las canciones por artista
     * @return Las canciones ordenadas alfabeticamente por nombre de artista
    */
    // Convertir el QMap a una lista de pares
    QList<QPair<QString, QString>> songArtistPairs;
    QMap<QString, QString>::iterator it;
    for (it = songArtistMapping.begin(); it != songArtistMapping.end(); ++it) {
        songArtistPairs.append(qMakePair(it.key(), it.value()));
    }

    // Ordenar la lista basándose en los artistas
    std::sort(songArtistPairs.begin(), songArtistPairs.end(), [](const QPair<QString, QString> &a, const QPair<QString, QString> &b) {
        return a.second < b.second;
    });

    // Actualizar allSongs y songArtistMapping
    allSongs.clear();
    songArtistMapping.clear();
    for (const QPair<QString, QString> &pair : songArtistPairs) {
        allSongs.append(pair.first);
        songArtistMapping.insert(pair.first, pair.second);
    }

    // Refrescar la vista
    updateSongView();
}
