#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QBuffer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowIcon(QIcon(QStringLiteral(":/background/mainlogo")));//F:/Projects/QTprojects/QT_learning/could_selected_pictures/pic.png
    ui->setupUi(this);
    this->Init();//初始化
}
MainWindow::~MainWindow()
{
    delete ui;
    delete finalbuf;
    delete image;
    delete image2;
    delete myCom;
}
void MainWindow::Init()
{
    ui->txtReceive->setReadOnly(true);
    ui->edgeBtn->setEnabled(false);//不允许处理
    ui->saveFormerButton->setEnabled(false);//不允许保存原图
    ui->photoBtn->setEnabled(false);//不允许拍照
    ui->saveBtn->setEnabled(false);//不允许保存后来的图片
    ui->edgeSlider->setEnabled(false);//不允许参数滑动条
    ui->serialOpenButton->setEnabled(false);//不使能打开串口

    this->finalbuf = new QByteArray;
    this->image = new QImage();
    this->image2 =new QImage;
    tcpClient=new QTcpSocket(this);
    tcpClient->abort();//取消原有连接

    connect(tcpClient,SIGNAL(readyRead()),this,SLOT(ReadData()));

    connect(tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(ReadError(QAbstractSocket::SocketError)));

    connect(ui->actionExit,SIGNAL(triggered()),qApp,SLOT(closeAllWindows()));//关闭主窗口
    connect(ui->actionClear,SIGNAL(triggered()),this,SLOT(slotClearAll()));//清除图片
    connect(ui->actionOpenImage,SIGNAL(triggered()),this,SLOT(slotOpen()));//打开图片
    connect(ui->actionAbout,SIGNAL(triggered()),qApp,SLOT(aboutQt()));//打开Qt
    ui->edgeSlider->setValue(18);
    QString str = QString("%1").arg(ui->edgeSlider->value());
    ui->edgeLabel->setText("しきい値:"+str);
    serialInit();
    edgeValue = ui->edgeSlider->value();

}
void MainWindow::ReadData()
{
    QByteArray buffer=tcpClient->readAll();
    QByteArray abuf;
    static bool read_mid_flag = false;
    static bool head_flag = false;
    int i=0;
    head_flag = false;
    for(;i<buffer.length()-1;i++)
    {
        if(!read_mid_flag)
        {
            if((buffer[i]==-1)&&(buffer[i+1]==-40))
            {
                abuf=buffer.right(buffer.length()-i);
                finalbuf->clear();
                finalbuf->append(abuf);
                read_mid_flag=true;
                head_flag =true;
            }
        }
        if(read_mid_flag)
        {
            if((buffer[i]==-1)&&(buffer[i+1]==-39))
            {
                abuf=buffer.left(i+2);
                finalbuf->append(abuf);
                read_mid_flag=false;
                image->loadFromData(*finalbuf);
                ui->showLabel->clear();
                ui->showLabel->setPixmap(QPixmap::fromImage(*image));//将结果在label上显示
                ui->resolutionLabel->setText("画面解像度:"+QString::number(ui->showLabel->width())+"*"+QString::number(ui->showLabel->height()));
                ui->photoBtn->setEnabled(true);
                ui->serialOpenButton->setEnabled(true);
            }
        }
    }
    if(!head_flag)
    {
        finalbuf->append(buffer);
    }
}
void MainWindow::ReadError(QAbstractSocket::SocketError)
{
    tcpClient->disconnectFromHost();
    ui->connectBtn->setText("连接");
    ui->txtReceive->append(tr("连接服务器失败,原因:%1").arg(tcpClient->errorString()));
}
void MainWindow::on_connectBtn_clicked()
{
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString strTime = time.toString("yyyy-MM-dd hh:mm:ss ddd"); //设置显示格式

    if (ui->connectBtn->text()=="连接")
    {
        tcpClient->connectToHost(ui->txtIP->text(),ui->txtPort->text().toInt());
        if (tcpClient->waitForConnected(1000))
        {
            ui->connectBtn->setText("断开");
            ui->txtReceive->append(tr("连接嵌入式端成功 %1").arg(strTime));

            ui->serialOpenButton->setEnabled(true);
            ui->serialOpenButton->setText(tr("打开串口"));
        }
    }
    else
    {
        tcpClient->disconnectFromHost();
        tcpClient->waitForDisconnected(1000);
        if (tcpClient->state() == QAbstractSocket::UnconnectedState || tcpClient->waitForDisconnected(1000))
        {
            ui->connectBtn->setText("连接");
            ui->txtReceive->append(tr("断开连接成功 %1").arg(strTime));
            ui->edgeBtn->setEnabled(false);//不允许处理
            ui->saveFormerButton->setEnabled(false);//不允许保存原图
            ui->saveBtn->setEnabled(false);//不允许保存后来的图片
            ui->photoBtn->setEnabled(false);//不允许拍照
            ui->edgeSlider->setEnabled(false);//不允许滑动            
            ui->serialOpenButton->setText(tr("打开串口"));
            ui->serialOpenButton->setEnabled(false);//不使能打开串口
        }
    }
}
void MainWindow::on_photoBtn_clicked()
{
    *image2 = image->copy();
    qDebug() << "showLabel宽高" <<ui->showLabel->width() << ui->showLabel->height();
    ui->photoLabel->clear();
    ui->photoLabel->setPixmap(QPixmap::fromImage(*image2));//将结果在label上显示
    qDebug() << "photoLabel宽高" <<ui->photoLabel->width() << ui->photoLabel->height();
    ui->edgeBtn->setEnabled(true);//允许处理
    ui->saveFormerButton->setEnabled(true);//允许保存原图
    ui->edgeSlider->setEnabled(false);
}
void MainWindow::on_saveFormerButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(
                    this, "保存元画像",
                    ".",
                    "画像 (*.jpg *.bmp *.pbm *.pgm *.png *.ppm *.xbm *.xpm);;所有文件(*.*)");
    if(!fileName.isEmpty())
    {
        saveFormImage(fileName);
    }
    else
    {
        return;
    }

}
bool MainWindow::saveFormImage(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(this,tr("元画像を保存する"),tr("保存できない %1:\n%2").arg(filename).arg(file.errorString()));
        return false;
    }
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image2->save(&buffer, "JPG");
    file.write(ba);
    QMessageBox::information(this,tr("元画像を保存する"),tr("元画像が保存した：\n%1").arg(filename));
    return true;
}
void MainWindow::slotClearAll()
{
    ui->photoLabel->clear();//清除图片
    ui->resultLabel->clear();
    ui->edgeBtn->setEnabled(false);//不允许处理
    ui->saveFormerButton->setEnabled(false);//不允许保存原图
    ui->saveBtn->setEnabled(false);
}
void MainWindow::on_edgeBtn_clicked()
{
    Mat resultmat = Canny_check();//resultmat完整！
    resultimage = Mat2QImage(resultmat);//将mat格式转换为Qimage格式
    ui->resultLabel->clear();
    ui->resultLabel->setPixmap(QPixmap::fromImage(resultimage));//将结果在label上显示

    ui->saveBtn->setEnabled(true);
    ui->edgeSlider->setEnabled(true);//允许参数滑动条
    connect(ui->edgeSlider, SIGNAL(valueChanged(int)), this, SLOT(edgeSBmove(int)));//使能通过参数处理图片
    ui->edgeBtn->setEnabled(false);
}
Mat MainWindow::Canny_check()
{

    //载入原始图
    Mat srcImage;
    srcImage = QImage2Mat(*image2);
    //	高阶的canny用法，转成灰度图，降噪，用canny，最后将得到的边缘作为掩码，拷贝原图到效果图上，得到彩色的边缘图
    Mat dstImage,grayImage,g_cannyMat_output;
    RNG g_rng(12345);
    vector<vector<Point> > g_vContours;
    vector<Vec4i> g_vHierarchy;

    //创建与src同类型和大小的矩阵(dst)
    dstImage.create( srcImage.size(), srcImage.type() );

    //将原图像转换为灰度图像
    cvtColor( srcImage, grayImage, COLOR_BGR2GRAY );

    //先用使用 3x3内核来降噪（均值滤波）
    blur( grayImage, grayImage, Size(1,1) );

    //运行Canny算子
    Canny( grayImage, g_cannyMat_output, edgeValue, edgeValue*3,3 );

    // 找到轮廓
    //RETR_EXTERNAL,只检测最外部轮廓
    findContours( g_cannyMat_output, g_vContours, g_vHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

    // 计算矩
    vector<Moments> mu(g_vContours.size() );
    for(unsigned int i = 0; i < g_vContours.size(); i++ )
    { mu[i] = moments( g_vContours[i], false ); }

    //  计算中心矩
    vector<Point2f> mc( g_vContours.size() );
    for( unsigned int i = 0; i < g_vContours.size(); i++ )
    { mc[i] = Point2f( static_cast<float>(mu[i].m10/mu[i].m00), static_cast<float>(mu[i].m01/mu[i].m00 )); }

    // 绘制轮廓
    Mat drawing = Mat::zeros( g_cannyMat_output.size(), CV_8UC3 );
    for( unsigned int i = 0; i< g_vContours.size(); i++ )
    {
        Scalar color = Scalar( g_rng.uniform(0, 255), g_rng.uniform(0,255), g_rng.uniform(0,255) );//随机生成颜色值
        drawContours( drawing, g_vContours, i, color, 2, 8, g_vHierarchy, 0, Point() );//绘制外层和内层轮廓
        //circle( drawing, mc[i], 4, color, -1, 8, 0 );;//绘制圆
        store_length.push_back(arcLength( g_vContours[i], true ));
    }
    //ContoursSize = g_vContours.size();

//    //【5】将g_dstImage内的所有元素设置为0
//    dstImage = Scalar::all(0);

//    //【6】使用Canny算子输出的边缘图g_cannyDetectedEdges作为掩码，来将原图g_srcImage拷到目标图g_dstImage中
//    srcImage.copyTo( dstImage, edge);
    sort(store_length.begin(),store_length.end());
    qDebug() <<"==================================================";
    for(unsigned  int p = 0; p< g_vContours.size(); p++ )
    {
        qDebug() << store_length[p];
    }

    qDebug() <<store_length.capacity()<<"  capacity1";

    for(int k =0;k<g_vContours.size();k++)
    {
        store_length.pop_back();
    }
    qDebug() <<store_length.capacity()<<"  capacity2";
    //store_length.swap(vector<float>());  //清除容器并最小化它的容量，不行！！
    inLength = ui->reallineEdit->text().toFloat();

    outLength = inLength*store_length[g_vContours.size()-3]/2/(store_length[g_vContours.size()-1]+store_length[g_vContours.size()-2]);
    float delta = ui->deltaLineEdit->text().toFloat();
    outLength = outLength + delta;
    qDebug() <<"outLength="<<outLength;

    QString str = QString::number(outLength, 'f', 2);//取小数点后2位

    float minlength = ui->minlengthLineEdit->text().toFloat();
    if(outLength >=minlength)
    {
        ui->finallengthLabel->setText("傷の長さ："+str+" cm");
    }
    else
    {
        ui->finallengthLabel->setText("傷がない！！！");
    }
    return drawing;
}
QImage MainWindow::Mat2QImage(const Mat &mat)
{
    //8-bitsunsigned,NO.OFCHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        QVector<QRgb>colorTable;
        for(int i=0;i<256;i++)
            colorTable.push_back(qRgb(i,i,i));
        const uchar*qImageBuffer=(const uchar*)mat.data;
        QImage img(qImageBuffer,mat.cols,mat.rows,mat.step,QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    if(mat.type()==CV_8UC3)
    {
        const uchar *qImageBuffer=(const uchar*)mat.data;
        QImage img(qImageBuffer,mat.cols,mat.rows,mat.step,QImage::Format_RGB888);
        return img.rgbSwapped();

    }
    else
    {
        qDebug()<<"ERROR:MatcouldnotbeconvertedtoQImage.";
        return QImage();
    }
}
Mat MainWindow::QImage2Mat(const QImage& qimage)
{
    Mat mat = Mat(qimage.height(), qimage.width(), CV_8UC4, (uchar*)qimage.bits(), qimage.bytesPerLine());
    Mat mat2 = Mat(mat.rows, mat.cols, CV_8UC3 );
    int from_to[] = { 0,0, 1,1, 2,2 };
    mixChannels( &mat, 1, &mat2, 1, from_to, 3 );
    return mat2;
}
bool MainWindow::saveFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(this,tr("结果を保存する"),tr("保存できない %1:\n%2").arg(filename).arg(file.errorString()));
        return false;
    }
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    resultimage.save(&buffer, "JPG");
    file.write(ba);
    QMessageBox::information(this,tr("结果を保存する"),tr("ここに保存された：\n%1").arg(filename));
    return true;
}
void MainWindow::on_saveBtn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(
                    this, "结果を保存する",
                    ".",
                    "画像 (*.jpg *.bmp *.pbm *.pgm *.png *.ppm *.xbm *.xpm);;所有文件(*.*)");
    if(!fileName.isEmpty())
    {
        saveFile(fileName);
    }
    else
    {
        return;
    }

}
void MainWindow::slotOpen()
{
    QString fileName = QFileDialog::getOpenFileName(
                    this, "画像ファイルを開く",
                    ".",
                    "画像 (*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm);;所有文件(*.*)");
    if(fileName != "")
    {
        if(image2->load(fileName))
        {
            ui->photoLabel->clear();
            ui->photoLabel->setPixmap(QPixmap::fromImage(*image2));//将结果在label上显示
            qDebug() << "photoLabel宽高" <<ui->photoLabel->width() << ui->photoLabel->height();
            ui->edgeBtn->setEnabled(true);//允许处理
            ui->saveFormerButton->setEnabled(true);//允许保存原图
        }
        else
        {
            QMessageBox::information(this,
                            tr("Fail!"),
                            tr("Fail!"));
            delete image; return;
        }
    }
}
void MainWindow::edgeSBmove(int value)//滑动条滑动后的槽
{
    edgeValue = ui->edgeSlider->value();
    qDebug() << edgeValue;
    ui->edgeLabel->setText("パラメータ値:"+QString::number(edgeValue));
    Mat resultmat = Canny_check();//resultmat完整！
    resultimage = Mat2QImage(resultmat);//将mat格式转换为Qimage格式
    ui->resultLabel->clear();
    ui->resultLabel->setPixmap(QPixmap::fromImage(resultimage));//将结果在label上显示
    ui->edgeBtn->setEnabled(false);//不允许处理
}
void MainWindow::on_focusButton_clicked()
{
    QObject *signalSender = sender();
    if (signalSender->objectName() == "focusButton")
    {
        myCom->write("1\r\n");
    }
}
void MainWindow::on_serialOpenButton_clicked()
{
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString strTime = time.toString("yyyy-MM-dd hh:mm:ss ddd"); //设置显示格式
    //serialInit();
    if(ui->serialOpenButton->text()==tr("打开串口"))
    {


        //设置波特率
        switch(ui->BaudBox->currentText().toInt())
        {
        case 1200: myComSetting.BaudRate = BAUD1200;
        case 2400: myComSetting.BaudRate = BAUD2400;
        case 4800: myComSetting.BaudRate = BAUD4800;
        case 9600: myComSetting.BaudRate = BAUD9600;
        case 19200: myComSetting.BaudRate = BAUD19200;
        case 38400: myComSetting.BaudRate = BAUD38400;
        case 115200: myComSetting.BaudRate = BAUD115200;
        case 256000: myComSetting.BaudRate = BAUD256000;
        case 921600: myComSetting.BaudRate = BAUD921600;
        default: break;
        }
        //设置数据位数
        switch(ui->BitNumBox->currentIndex())
        {
        case 8: myComSetting.DataBits = DATA_8; break;
        case 7: myComSetting.DataBits = DATA_7;break;
        case 6: myComSetting.DataBits = DATA_6;; break;
        case 5: myComSetting.DataBits = DATA_5;; break;
        default:break;
        }
        //设置奇偶校验
        if (ui->ParityBox->currentText() == QString::fromLocal8Bit("無し"))
            myComSetting.Parity = PAR_NONE;
        else if (ui->ParityBox->currentText() == QString::fromLocal8Bit("奇数"))
            myComSetting.Parity = PAR_ODD;
        else if (ui->ParityBox->currentText() == QString::fromLocal8Bit("偶数"))
            myComSetting.Parity = PAR_EVEN;
        //设置停止位
        if (ui->StopBox->currentText() == "1")
            myComSetting.StopBits =STOP_1;
        else if (ui->StopBox->currentText() == "1.5")
            myComSetting.StopBits =STOP_1_5;
        else if (ui->StopBox->currentText() == "2")
            myComSetting.StopBits =STOP_2;
        //设置流控制
        myComSetting.FlowControl = FLOW_OFF;
        //设置延时
        myComSetting.Timeout_Millisec =100;//ms

        //定义串口对象，并传递参数，在构造函数里对其进行初始化
        myCom = new Win_QextSerialPort(myComSetting,QextSerialBase::EventDriven);
        //设置串口名
        myCom->setPortName(ui->PortBox->currentText());
        //打开串口
        myCom->open(QIODevice::ReadWrite);

        //关闭设置菜单使能
        ui->PortBox->setEnabled(false);
        ui->BaudBox->setEnabled(false);
        ui->BitNumBox->setEnabled(false);
        ui->ParityBox->setEnabled(false);
        ui->StopBox->setEnabled(false);
        ui->serialOpenButton->setText(tr("关闭串口"));

        ui->focusButton->setEnabled(true);
        ui->beepButton->setEnabled(true);
        ui->ledCouldButton->setEnabled(true);

        ui->txtReceive->append(tr("已打开串口！！"));
        ui->connectBtn->setEnabled(false);
        //连接信号槽
        connect(myCom, SIGNAL(readyRead()), this, SLOT(Read_Data()));
    }
    else
    {
//        QObject *signalSender = sender();
//        if (signalSender->objectName() == "serialOpenButton")
//        {
//                QString str = "6\r\n";
//                QByteArray sendData = str.toUtf8();
//                myCom->write(sendData);
//        }
        ui->serialOpenButton->setEnabled(false);
        ui->txtReceive->append(tr("已关闭串口！！"));
        tcpClient->disconnectFromHost();
        tcpClient->waitForDisconnected(1000);
        //关闭串口
        myCom->close();
        //恢复设置使能
        ui->PortBox->setEnabled(true);
        ui->BaudBox->setEnabled(true);
        ui->BitNumBox->setEnabled(true);
        ui->ParityBox->setEnabled(true);
        ui->StopBox->setEnabled(true);
        ui->serialOpenButton->setText(tr("打开串口"));

        ui->focusButton->setEnabled(false);
        ui->beepButton->setText(tr("打开蜂鸣器"));
        ui->beepButton->setEnabled(false);
        ui->ledCouldButton->setEnabled(false);
        ui->ledCouldButton->setText(tr("打开闪光灯"));
        ui->connectBtn->setEnabled(true);
    }
}
void MainWindow::serialInit()
{
    //查找可用的串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->PortBox->addItem(serial.portName());
            serial.close();
        }
    }
    //设置波特率下拉菜单默认显示第三项
    ui->BaudBox->setCurrentIndex(2);

    //关闭发送按钮的使能
    ui->focusButton->setEnabled(false);
    //关闭蜂鸣器按钮的使能
    ui->beepButton->setEnabled(false);
    //关闭闪光灯按钮的使能
    ui->ledCouldButton->setEnabled(false);
    qDebug() << tr("Com界面设定成功！");

}
void MainWindow::on_beepButton_clicked()
{
    QObject *signalSender = sender();
    if (signalSender->objectName() == "beepButton")
    {
        if(ui->beepButton->text() =="打开蜂鸣器")
        {
            ui->beepButton->setText("关闭蜂鸣器");
            QString str = "444\r\n";
            QByteArray sendData = str.toUtf8();
            myCom->write(sendData);
        }
        else
        {
            myCom->write("5555\r\n");
            ui->beepButton->setText("打开蜂鸣器");
        }
    }
}
//可能的冒泡排序
//    for(vector<int>::size_type ix=0; ix<store_length.size(); ++ix)
//    {
//        for(vector<int>::size_type iy=0; iy<store_length.size()-ix-1; ++iy)
//        {
//            if(store_length[iy] < store_length[iy+1])
//            {
//                int t;
//                t = store_length[iy];
//                store_length[iy] = store_length[iy+1];
//                store_length[iy+1] = t;
//            }
//        }
//    }

//读取接收到的数据
void MainWindow::Read_Data()
{
    QByteArray buf;
    QString temp;
    buf = myCom->readAll();
    qDebug()<<buf.length()<<buf.toHex();
//    if(buf.length() == 0)
//        return;
//    和这句相同：if(!buf.isEmpty())
    if(!buf.isEmpty())
    {
        for(int i=0;i<buf.length()-1;i++)
        {
            if((buf[i] == 'g')&&(buf[i+1] == 'o'))
            {
               qDebug()<<"go!!";
               ui->txtReceive->append(tr("可以连接了！！"));
               ui->connectBtn->setEnabled(true);
            }
        }
        QString myStrTemp = QString::fromLocal8Bit( buf );
        ui->txtReceive->append(myStrTemp);
    }
    buf.clear();
}

void MainWindow::on_ledCouldButton_clicked()
{
    QObject *signalSender = sender();
    if (signalSender->objectName() == "ledCouldButton")
    {
        if(ui->ledCouldButton->text() =="打开闪光灯")
        {
            ui->ledCouldButton->setText("关闭闪光灯");
            myCom->write("2\r\n");
        }
        else
        {
            myCom->write("3\r\n");
            ui->ledCouldButton->setText("打开闪光灯");
        }
    }
}
