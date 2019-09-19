#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include "QDebug"
#include <QString>
#include <QTimer>
#include <QMessageBox>
#include "loaddata.h"
#include "computeview.h"
#include <iostream>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QPushButton *up;
    QPushButton *left;
    QPushButton *down;
    QPushButton *right;
    QPushButton *play;
    QLabel *displayWin;                         //display video

    QTimer* timer;
    cv::VideoCapture videoCapture;
    bool play_status;

    QImage MatToQImage(const cv::Mat& mat);     //transfer Mat to QImage

    loadData* ld;
    computeView* compView;

    int frame_num;                              // current frame
    int cur_view;                               // current view
    int cur_camera;                             // current camera

    int cam_ref[2];                             //reference camera
    cv::Mat viewSynthesisImage;                 //synthesis image

private slots:
    void left_btn_clicked();
    void up_btn_clicked();
    void right_btn_clicked();
    void down_btn_clicked();
    void play_btn_clicked();

    void DisplayImage();
};

#endif // MAINWINDOW_H
