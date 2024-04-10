#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "VideoCaptureThread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_cameraComboBox_currentIndexChanged(int index);

    void on_btnLoadVideo_clicked();

private:
    Ui::MainWindow *ui;
    VideoCaptureThread* videoCaptureThread;
    QThread* thread;
};
#endif // MAINWINDOW_H
