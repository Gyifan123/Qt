#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>


#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTextBrowser>
#include <QDataStream>
#include <QByteArray>
#include "win_qextserialport.h"
#include "qextserialbase.h"
#include "qextserialport.h"

using namespace cv;
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
    QImage resultimage;
    qint8 edgeValue=50;

private slots:
    void ReadData();

    void on_connectBtn_clicked();

    void ReadError(QAbstractSocket::SocketError);

    void on_photoBtn_clicked();

    void on_edgeBtn_clicked();

    void on_focusButton_clicked();

    void on_saveBtn_clicked();

    void slotClearAll();

    void slotOpen();

    void on_saveFormerButton_clicked();

    void edgeSBmove(int);//滑动条滑动后的槽

    void on_serialOpenButton_clicked();

    void on_beepButton_clicked();

    void Read_Data();
    void on_ledCouldButton_clicked();



private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpClient;//创建TCP套接字

    QImage *image;
    QImage *image2;
    QByteArray *finalbuf;//而不能写成QByteArray *finalbuf;

    Win_QextSerialPort *myCom;
    struct PortSettings myComSetting ;//定义一个结构体，用来存放串口各个参数

    Mat QImage2Mat(const QImage& qimage);
    QImage Mat2QImage(const Mat &mat);

    void Init();
    void serialInit();
    Mat Canny_check();
    bool saveFile(const QString &filename);
    bool saveFormImage(const QString &filename);
    float inLength;//周长
    float outLength;//划痕长度
    vector<float>store_length;
};

#endif // MAINWINDOW_H
