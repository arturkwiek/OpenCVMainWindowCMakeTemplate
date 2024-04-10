#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "videocapturethread.h"
#include <opencv2/imgproc.hpp>
#include <QPixmap>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , videoCaptureThread(nullptr)
    , thread(nullptr)
{
    ui->setupUi(this);

    // Tutaj możesz zainicjalizować listę kamer dostępnych w systemie
    // Przykład: ui->cameraComboBox->addItem("Kamera 1");
}

MainWindow::~MainWindow()
{
    if (thread != nullptr) {
        thread->quit();
        thread->wait();
    }
    delete ui;
}

void MainWindow::on_startButton_clicked()
{
    if (videoCaptureThread == nullptr) {
        int cameraIndex = ui->cameraComboBox->currentIndex();
        videoCaptureThread = new VideoCaptureThread(cameraIndex);
        thread = new QThread(this);

        videoCaptureThread->moveToThread(thread);
        connect(thread, &QThread::started, videoCaptureThread, &VideoCaptureThread::process);
        connect(videoCaptureThread, &VideoCaptureThread::frameReady, this, [&](cv::Mat frame) {
            cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
            QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
            ui->videoLabel->setPixmap(QPixmap::fromImage(img).scaled(ui->videoLabel->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
        });
        connect(videoCaptureThread, &VideoCaptureThread::finished, thread, &QThread::quit);
        connect(thread, &QThread::finished, this, [&]() {
            delete videoCaptureThread;
            videoCaptureThread = nullptr;
            delete thread;
            thread = nullptr;
        });

        thread->start();
    }
}

void MainWindow::on_stopButton_clicked()
{
    if (videoCaptureThread != nullptr) {
        videoCaptureThread->stop();
    }
}

void MainWindow::on_cameraComboBox_currentIndexChanged(int index)
{
    if (videoCaptureThread != nullptr) {
        // Zakładamy, że 'stop' natychmiast zatrzymuje przetwarzanie i zamyka kamerę
        videoCaptureThread->stop();
        thread->quit();
        thread->wait();

        delete videoCaptureThread; // Usuwamy bezpośrednio, bez requestStop
        videoCaptureThread = nullptr;

        delete thread; // Usuwanie stworzonego wcześniej wątku
        thread = nullptr; // Ustawienie wskaźnika na nullptr, aby uniknąć dzikiego wskaźnika
    }

    // Stworzenie nowego wątku i VideoCaptureThread z nowym indeksem kamery
    thread = new QThread(this); // Utworzenie nowego wątku
    videoCaptureThread = new VideoCaptureThread(index); // Utworzenie nowego VideoCaptureThread z nowym indeksem
    videoCaptureThread->moveToThread(thread); // Przeniesienie VideoCaptureThread do nowego wątku

    // Ponowne połączenie sygnałów i slotów
    connect(thread, &QThread::started, videoCaptureThread, &VideoCaptureThread::process);
    connect(videoCaptureThread, &VideoCaptureThread::frameReady, this, [&](cv::Mat frame) {
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        ui->videoLabel->setPixmap(QPixmap::fromImage(img).scaled(ui->videoLabel->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
    });
    connect(videoCaptureThread, &VideoCaptureThread::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, this, [this]() {
        // Zapewnienie, że zasoby są czyszczone po zakończeniu wątku
        if (thread) {
            thread->deleteLater(); // Usuwanie wątku po bezpiecznym zakończeniu
        }
        if (videoCaptureThread) {
            videoCaptureThread->deleteLater(); // Opcjonalnie, jeśli masz ustawiony destruktor do bezpiecznego usuwania
        }
        thread = nullptr;
        videoCaptureThread = nullptr;
    });

    // Rozpoczęcie przetwarzania w nowym wątku
    thread->start();
}

void MainWindow::on_btnLoadVideo_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Otwórz wideo"), "", tr("Pliki wideo (*.mp4 *.avi)"));

    if (fileName.isEmpty())
    {
        qDebug() << "file name empty";
        return;
    }
    else
    {
        qDebug() << "file: " << fileName;
        ui->lblVideoFile->setText(fileName);
    }

    if (thread != nullptr) {

        qDebug() << "zamykanie istniejącego wątku";
        // Zakładamy, że 'stop' natychmiast zatrzymuje przetwarzanie i zamyka kamerę
        videoCaptureThread->stop();
        thread->quit();
        thread->wait();

        qDebug() << "usuwanie videoCaptureThread";
        delete videoCaptureThread; // Usuwamy bezpośrednio, bez requestStop
        videoCaptureThread = nullptr;

        qDebug() << "usuwanie thread";
        delete thread; // Usuwanie stworzonego wcześniej wątku
        thread = nullptr; // Ustawienie wskaźnika na nullptr, aby uniknąć dzikiego wskaźnika
    }

    qDebug() << "nowy thread";
    thread = new QThread(this);
    qDebug() << "nowy VideoCaptureThread";
    videoCaptureThread = new VideoCaptureThread(fileName); // Użyj ścieżki do pliku jako argumentu
    videoCaptureThread->moveToThread(thread);

    // Ponownie ustaw połączenia sygnał-slot (tak samo, jak w on_cameraComboBox_currentIndexChanged)

    qDebug() << "startuj thread";
    thread->start();
}

