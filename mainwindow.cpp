#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    this->setFixedSize(1300,700);
    ui->setupUi(this);

    up = new QPushButton("up", this);
    up->setFixedSize(50, 50);
    up->move(1150, 50);
    up->setFont(QFont("Timers", 12, QFont::Bold));
    connect(up, SIGNAL(clicked(bool)), this, SLOT(up_btn_clicked()));

    left = new QPushButton("left", this);
    left->setFixedSize(50, 50);
    left->move(1100, 100);
    left->setFont(QFont("Timers", 12, QFont::Bold));
    connect(left, SIGNAL(clicked(bool)), this, SLOT(left_btn_clicked()));

    down = new QPushButton("down", this);
    down->setFixedSize(50, 50);
    down->move(1150, 150);
    down->setFont(QFont("Timers", 12, QFont::Bold));
    connect(down, SIGNAL(clicked(bool)), this, SLOT(down_btn_clicked()));

    right = new QPushButton("right", this);
    right->setFixedSize(50, 50);
    right->move(1200, 100);
    right->setFont(QFont("Timers", 12, QFont::Bold));
    connect(right, SIGNAL(clicked(bool)), this, SLOT(right_btn_clicked()));

    play = new QPushButton("play", this);
    play->setFixedSize(100, 50);
    play->move(1100, 300);
    play->setFont(QFont("Timers", 12, QFont::Bold));
    connect(play, SIGNAL(clicked(bool)), this, SLOT(play_btn_clicked()));

    displayWin = new QLabel(this);
    displayWin->setFixedSize(960, 600);
    displayWin->move(50,50);


    ld = new loadData();
    compView = new computeView();

    char tmp[20];
    sprintf(tmp, "color-cam%d-f%03d.jpg", ld->cur_camera, ld->frame_num);
    string frame_name = ld->path_arr[ld->cur_camera] + tmp;
    cv::Mat img = cv::imread(frame_name);
    QImage image;
    image = MatToQImage(img);
    displayWin->clear();
    displayWin->setPixmap(QPixmap::fromImage(image));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(DisplayImage()));
    timer->setInterval(50);

    play_status = false;

    ld->get_view_setting(ld->calibParams_path);
    ld->get_allView_calibParams();
    compView->set_ld(ld);
}

void MainWindow::up_btn_clicked()
{

}

void MainWindow::left_btn_clicked()
{
    timer->stop();
    ld->cur_view += 1;
    cout <<"current view id: " << ld->cur_view << endl;
    if(ld->cur_view == ld->view_num)
    {
        QMessageBox::information(this, "warning", "boundary reached!");
        ld->cur_view = ld->view_num - 1;
    }
    else
    {
        ld->get_ref_camera(ld->cur_view);
        cout << "left camera id: " << ld->refIdx_in_cam_arr[0] << "  right camera id: " << ld->refIdx_in_cam_arr[1] << endl;
        ld->get_frame_imgArr(ld->frame_num, ld->refIdx_in_cam_arr[0], ld->refIdx_in_cam_arr[1]);
        ld->get_frame_depArr(ld->frame_num, ld->refIdx_in_cam_arr[0], ld->refIdx_in_cam_arr[1]);

        if(ld->refIdx_in_cam_arr[0] == ld->refIdx_in_cam_arr[1])
            viewSynthesisImage = ld->frame_imgArr[0];
        else
            viewSynthesisImage = compView->viewSynthesis(ld->cur_view);

        QImage image;
        image = MatToQImage(viewSynthesisImage);
        displayWin->setPixmap(QPixmap::fromImage(image));
    }
    cout << "compute finished" << endl;
}

void MainWindow::right_btn_clicked()
{
    timer->stop();
    ld->cur_view -= 1;
    if(ld->cur_view < 0)
    {
        QMessageBox::information(this, "warning", "boundary reached!");
        ld->cur_view = 0;
    }
    else
    {
        ld->get_ref_camera(ld->cur_view);
        ld->get_frame_imgArr(ld->frame_num, ld->refIdx_in_cam_arr[0], ld->refIdx_in_cam_arr[1]);
        ld->get_frame_depArr(ld->frame_num, ld->refIdx_in_cam_arr[0], ld->refIdx_in_cam_arr[1]);

        if(ld->refIdx_in_cam_arr[0] == ld->refIdx_in_cam_arr[1])
            viewSynthesisImage = ld->frame_imgArr[0];
        else
            viewSynthesisImage = compView->viewSynthesis(ld->cur_view);

        QImage image;
        image = MatToQImage(viewSynthesisImage);
        displayWin->setPixmap(QPixmap::fromImage(image));
    }
    cout << "compute finished" << endl;
}

void MainWindow::down_btn_clicked()
{

}

void MainWindow::DisplayImage()
{
    cv::Mat image;
    videoCapture >> image;
    QImage img;
    if(image.empty())
    {
            std::cout << "End of Sequence" << std::endl;
            timer->stop();
            image = cv::imread("F:\\wei\\freeViewProject\\test_pic_video\\ballet-dancer\\MSR3DVideo-Ballet\\cam3\\color-cam3-f000.jpg");
            play_status = false;
            ld->frame_num = 0;
    }
    img = MatToQImage(image);
    displayWin->setPixmap(QPixmap::fromImage(img));
    ld->frame_num ++;
}

void MainWindow::play_btn_clicked()
{
    if(!play_status)
    {
        std::string first_file = "F:\\wei\\freeViewProject\\test_pic_video\\ballet-dancer\\MSR3DVideo-Ballet\\cam3\\color-cam3-f%03d.jpg";
        cv::VideoCapture sequence(first_file);
        videoCapture.open(first_file);
        if (!sequence.isOpened())
        {
                std::cerr << "Failed to open the image sequence!\n" << std::endl;
                return;
        }
        timer->start(50);
        play_status = true;
    }
    else
        timer->start(50);
    std::cout << timer->interval() << std::endl;
}

QImage MainWindow::MatToQImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}
