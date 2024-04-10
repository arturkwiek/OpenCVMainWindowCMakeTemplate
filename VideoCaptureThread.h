#ifndef VIDEOCAPTURETHREAD_H
#define VIDEOCAPTURETHREAD_H

#include <QObject>
#include <opencv2/opencv.hpp>

class VideoCaptureThread : public QObject
{
    Q_OBJECT
public:
    explicit VideoCaptureThread(int cameraIndex, QObject *parent = nullptr);
    explicit VideoCaptureThread(QString source, QObject *parent = nullptr);
    void stop();

signals:
    void frameReady(cv::Mat frame);
    void finished();
public slots:
    void process();
    void setCameraIndex(int index);

private:
    cv::VideoCapture capture;
    bool stopped;
    int cameraIndex;
    QString source;
};

#endif // VIDEOCAPTURETHREAD_H
