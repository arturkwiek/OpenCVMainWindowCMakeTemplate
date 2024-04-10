#include "VideoCaptureThread.h"
#include <opencv2/imgproc.hpp>
#include <QDebug>

VideoCaptureThread::VideoCaptureThread(int cameraIndex, QObject *parent)
    : QObject(parent)
    , stopped(false)
    , cameraIndex(cameraIndex)
{
    qDebug() << "ll";
}

VideoCaptureThread::VideoCaptureThread(QString source, QObject *parent)
    : QObject(parent), source(source) // 'source' może być indeksem kamery lub ścieżką do pliku
{
}

void VideoCaptureThread::process() {
    if (source.contains("://") || source.startsWith("\\\\")) {
        // Źródło jest traktowane jako URL lub ścieżka sieciowa
        qDebug() << "URL lub ścieżka sieciowa";
        capture.open(source.toStdString());
    } else if (source.contains(":/") || source.contains(":\\") ) {
        // Źródło jest traktowane jako ścieżka pliku w Windows
        qDebug() << "ścieżka pliku w Windows";
        capture.open(source.toStdString());
    } else {
        // Próba otwarcia jako indeks kamery
        bool ok;
        int cameraIndex = source.toInt(&ok);
        qDebug() << "indeks kamery";
        if (ok) {
            capture.open(cameraIndex);
        } else {
            qDebug() << "Nie udało się zinterpretować źródła";
            // Nie udało się zinterpretować źródła jako indeks kamery ani jako ścieżka pliku
            emit finished();
            return;
        }
    }

    if (!capture.isOpened()) {
        emit finished();
        return;
    }

    cv::Mat frame;
    while (!stopped && capture.read(frame)) {
        if (frame.empty()) break;
        emit frameReady(frame.clone()); // Używamy clone(), aby uniknąć problemów z właścicielem danych
    }
    capture.release();
    emit finished();
}

void VideoCaptureThread::stop()
{
    stopped = true;
}

void VideoCaptureThread::setCameraIndex(int index) {
    if (cameraIndex != index) {
        stop();
        cameraIndex = index;
        if (!stopped) {
            process();
        }
    }
}
