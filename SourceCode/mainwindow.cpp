#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QDateTime>
#include <QMessageBox>
#include "listenthread.h"
#include "saveimagethread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      m_settings(new QSettings(qApp->applicationDirPath() + "/settings.txt", QSettings::IniFormat)),
      m_imageMutex(new QMutex()),
      m_imageLabel(new QLabel()),
      m_tcpServer(new QTcpServer(this))
{
    ui->setupUi(this);
    getSerialPortList();

    m_imageLabel->setBackgroundRole(QPalette::Base);
    m_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_imageLabel->setScaledContents(true);

    ui->edtPort->setText(m_settings->value("port", 13888).toString());
    ui->action_Fit_to_Window->setChecked(m_settings->value("fitSize", false).toBool());
    ui->cbSerialPort->setCurrentText(m_settings->value("serialPort").toString());
    ui->edtSaveDir->setText(m_settings->value("saveDir", "C:/").toString());
    ui->scrollArea->setBackgroundRole(QPalette::Dark);
    ui->scrollArea->setWidget(m_imageLabel);
    ui->cbDriverIndex->setCurrentText(m_settings->value("driverIndex", "0").toString());
    ui->edtConnector->setText(m_settings->value("connector","M").toString());
    ui->cbExposureMode->setCurrentIndex(m_settings->value("exposureMode", "0").toInt());
    ui->edtExposure->setText(m_settings->value("exposure", "1600").toString());
    ui->edtExposure->setEnabled(ui->cbExposureMode->currentIndex()==1);
    ui->edtPageLength->setText(m_settings->value("pageLength", 10000).toString());
    ui->sliderRateDivisionFactor->setValue(m_settings->value("rateDivisionFactor", 1).toInt());
    ui->ckAutoSave->setChecked(m_settings->value("autoSave", false).toBool());
    ui->btnStop->setEnabled(false);
    updateActions();

    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(tcpserver_newConnection()));
    if (!m_tcpServer->listen(QHostAddress::LocalHost, ui->edtPort->text().toInt()))
    {
        QMessageBox::information(this, "Error", "TcpServer is not listen...");

    }

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_2_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    ui->edtSaveDir->text(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    m_settings->setValue("saveDir", dir);
    ui->edtSaveDir->setText(dir);
}

void MainWindow::Callback(PMCSIGNALINFO SigInfo)
{
    switch(SigInfo->Signal)
    {
    case MC_SIG_SURFACE_PROCESSING:
        if (m_Channel != 0)
        {
            McGetParamPtr(SigInfo->SignalInfo, MC_SurfaceAddr, &m_pCurrent);
            m_imageMutex->lock();
            m_image = new QImage((uchar*)m_pCurrent, m_SizeX, m_SizeY, m_BufferPitch, QImage::Format_Indexed8);
            emit imageChanged(m_image);
            int elapsedPages;
            McGetParamInt(m_Channel, MC_Elapsed_Pg, &elapsedPages);
            int channelState;
            McGetParamInt (m_Channel, MC_ChannelState, &channelState);
            statusBar()->showMessage(QString("Channel State: %1").arg((channelState == MC_ChannelState_ACTIVE? "ACTIVE" : "IDLE")));
            m_imageMutex->unlock();

        }
        break;
    case MC_SIG_ACQUISITION_FAILURE:
        emit raiseError("Acquistion Failure ! GrablinkSnapshortTrigger");
        break;
    default:
        break;
    }
}
void MainWindow::setImage(QImage *newImage)
{
    try {
        if (m_Channel != 0)
        {
            m_imageCount++;
            m_imageLabel->setPixmap(QPixmap::fromImage(*newImage));

            if (ui->ckAutoSave->isChecked())
            {
                SaveImageThread *t = new SaveImageThread(m_settings->value("saveDir").toString(), newImage, this);
                t->start();
            }
            else
            {
                delete newImage;
            }


            m_scaleFactor = 1.0;

            ui->scrollArea->setVisible(true);
            updateActions();

            if (!ui->action_Fit_to_Window->isChecked())
                m_imageLabel->adjustSize();
            else
                fitToWindow();


        }

    } catch (...) {

    }
}
void MainWindow::displayError(QString message)
{
    QMessageBox::critical(this, "Error", message);
}


void MCAPI GlobalCallback(PMCSIGNALINFO SigInfo)
{
    if (SigInfo && SigInfo->Context)
    {
        MainWindow* pWindow = (MainWindow*)SigInfo->Context;
        pWindow->Callback(SigInfo);
    }
}
void MainWindow::on_btnGo_clicked()
{
    try {
        on_btnSave_clicked();

        modifyBalserCameraParameter();

        m_imageCount = 0;
        McOpenDriver(NULL);
        McSetParamInt(MC_CONFIGURATION, MC_ErrorHandling, MC_ErrorHandling_MSGBOX);
        McSetParamStr(MC_CONFIGURATION, MC_ErrorLog, "C:\\error.log");
        McCreate(MC_CHANNEL, &m_Channel);
        McSetParamInt(m_Channel, MC_DriverIndex, m_settings->value("driverIndex", "0").toInt());
        McSetParamStr(m_Channel, MC_Connector, m_settings->value("connector", "M").toString().toStdString().c_str());
        McSetParamStr(m_Channel, MC_CamFile, "raL12288-66km_L12288RG");
        McSetParamInt(m_Channel, MC_TapConfiguration, MC_TapConfiguration_FULL_8T8);
        McSetParamInt(m_Channel, MC_TapGeometry, MC_TapGeometry_1X8);
        McSetParamInt(m_Channel, MC_ResetEdge, MC_ResetEdge_GOHIGH);
        McSetParamInt(m_Channel, MC_AcquisitionMode, MC_AcquisitionMode_WEB);
        McSetParamInt(m_Channel, MC_PageLength_Ln, m_settings->value("pageLength", 10000).toInt());
        McSetParamInt(m_Channel, MC_TrigMode, MC_TrigMode_IMMEDIATE);
        McSetParamInt(m_Channel, MC_NextTrigMode, MC_NextTrigMode_REPEAT);
        McSetParamInt(m_Channel, MC_SeqLength_Ln, MC_INDETERMINATE);
        McSetParamInt(m_Channel, MC_Expose, MC_Expose_PLSTRG);
        McSetParamInt(m_Channel, MC_LineRateMode, MC_LineRateMode_PULSE);
        McSetParamInt(m_Channel, MC_ForwardDirection, MC_ForwardDirection_A_LEADS_B);
        McSetParamInt(m_Channel, MC_LineTrigCtl, MC_LineTrigCtl_DIFF_PAIRED);
        McSetParamInt(m_Channel, MC_LineTrigLine, MC_LineTrigLine_DIN1_DIN2);
        McSetParamInt(m_Channel, MC_LineTrigEdge, MC_LineTrigEdge_ALL_A_B);
        McSetParamInt(m_Channel, MC_LineTrigFilter, MC_LineTrigFilter_Filter_10us);
        McSetParamInt(m_Channel, MC_RateDivisionFactor, ui->sliderRateDivisionFactor->value());
        McGetParamInt(m_Channel, MC_ImageSizeX, &m_SizeX);
        McGetParamInt(m_Channel, MC_ImageSizeY, &m_SizeY);
        McGetParamInt(m_Channel, MC_BufferPitch, &m_BufferPitch);
        McSetParamInt(m_Channel, MC_SignalEnable + MC_SIG_SURFACE_PROCESSING, MC_SignalEnable_ON);
        McSetParamInt(m_Channel, MC_SignalEnable + MC_SIG_ACQUISITION_FAILURE, MC_SignalEnable_ON);
        //----Options----

        McRegisterCallback(m_Channel, GlobalCallback, this);
        disconnect(this, &MainWindow::imageChanged, this, &MainWindow::setImage);
        connect(this, &MainWindow::imageChanged, this, &MainWindow::setImage);
         McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_ACTIVE);
        ui->btnGo->setEnabled(false);
        ui->btnStop->setEnabled(true);

        ui->frame->setEnabled(false);

    } catch (...) {

    }
}

void MainWindow::on_btnStop_clicked()
{
    try {
        McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE);
        McDelete(m_Channel);
        m_Channel = 0;
        McCloseDriver();
        ui->frame->setEnabled(true);
    } catch (...) {

    }
    ui->btnGo->setEnabled(true);
    ui->btnStop->setEnabled(false);
}

void MainWindow::on_btnSave_clicked()
{
    m_settings->setValue("port", ui->edtPort->text());
    m_settings->setValue("saveDir", ui->edtSaveDir->text());
    m_settings->setValue("serialPort", ui->cbSerialPort->currentText());
    m_settings->setValue("driverIndex", ui->cbDriverIndex->currentText());
    m_settings->setValue("connector", ui->edtConnector->text());
    m_settings->setValue("exposureMode", QString::number(ui->cbExposureMode->currentIndex()));
    m_settings->setValue("exposure", ui->edtExposure->text());
    m_settings->setValue("pageLength", ui->edtPageLength->text());
    m_settings->setValue("rateDivisionFactor", ui->sliderRateDivisionFactor->value());
    m_settings->setValue("fitSize", ui->action_Fit_to_Window->isChecked());
    m_settings->setValue("autoSave", ui->ckAutoSave->isChecked());
}


void MainWindow::on_cbExposureMode_currentIndexChanged(int index)
{
    ui->edtExposure->setEnabled(index ==1);
}

void MainWindow::getSerialPortList()
{
    ui->cbSerialPort->clear();
    CLINT32 res = CL_ERR_NO_ERR;
    CLUINT32 numPorts = 0;
    res = clGetNumPorts(&numPorts);
    if (res != CL_ERR_NO_ERR)
    {
        return;
    }

    if (numPorts == 0)
    {
        return;
    }

    char szManuName[260] = {0};
    char szPortID[260] = {0};

    CLUINT32 manuNameLen = sizeof(szManuName);
    CLUINT32 portIDLen = sizeof(szPortID);
    CLUINT32 versionNumber = 0;

    if (numPorts > 1)
    {
        for (CLUINT32 i = 0; i < numPorts; ++i)
        {
            manuNameLen = sizeof(szManuName);
            portIDLen = sizeof(szPortID);
            versionNumber = 0;

            res = clGetPortInfo(i, szManuName, &manuNameLen, szPortID, &portIDLen, &versionNumber);
            if (res != CL_ERR_NO_ERR)
            {
                continue;
            }
            ui->cbSerialPort->addItem(QString("%1#%2").arg(szManuName).arg(szPortID));
        }
    }

}

void MainWindow::modifyBalserCameraParameter()
{
    hSerRef hRef = NULL;
    CLINT32 res = CL_ERR_NO_ERR;
    CLUINT32 portIndex((CLUINT32)ui->cbSerialPort->currentIndex()); // initialize with invalid value

    res = clSerialInit(portIndex, &hRef);
    if (res != CL_ERR_NO_ERR)
    {
        hRef = NULL;
        return;
    }

    res = clSetBaudRate(hRef, CL_BAUDRATE_9600);
    if (res != CL_ERR_NO_ERR)
    {
        goto ErrorReturn;
    }

    //========
    //0:off, 1on
    CLUINT32 TriggerModeOn = 1;
    res = clbbp_AllSerialWriteRegister(hRef, ADDRESS_TriggerMode, (const CLINT8*)&TriggerModeOn, sizeof(TriggerModeOn), 350);
    if (res != CL_ERR_NO_ERR)
    {
        goto ErrorReturn;
    }
    //0:off, 1:Timed, 2:TiggerWidth
    CLUINT32 exposureMode = ui->cbExposureMode->currentIndex();
    res = clbbp_AllSerialWriteRegister(hRef, ADDRESS_ExposureMode, (const CLINT8*)&exposureMode, sizeof(exposureMode), 350);
    if (res != CL_ERR_NO_ERR)
    {
        goto ErrorReturn;
    }
    if (exposureMode == 1)
    {
        CLUINT32 rawExposureTime = ui->edtExposure->text().toUInt();
        res = clbbp_AllSerialWriteRegister(hRef, ADDRESS_ExposureTime_RAW, (const CLINT8*)&rawExposureTime, sizeof(rawExposureTime), 350);
        if (res != CL_ERR_NO_ERR)
        {
            goto ErrorReturn;
        }

    }
    //========
ErrorReturn:
    if (hRef != NULL)
    {
        clSerialClose(hRef);
        hRef = NULL;
    }

}

void MainWindow::scaleImage(double factor)
{
    if (m_imageLabel->pixmap())
    {
        m_scaleFactor *= factor;
        m_imageLabel->resize(m_scaleFactor * m_imageLabel->pixmap()->size());

        adjustScrollBar(ui->scrollArea->horizontalScrollBar(), factor);
        adjustScrollBar(ui->scrollArea->verticalScrollBar(), factor);
        ui->actionZoom_In_25->setEnabled(m_scaleFactor < 3.0);
        ui->actionZoom_Out_25->setEnabled(m_scaleFactor > 0.333);

    }




}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

void MainWindow::updateActions()
{
    ui->actionZoom_In_25->setEnabled(!ui->action_Fit_to_Window->isChecked());
    ui->actionZoom_Out_25->setEnabled(!ui->action_Fit_to_Window->isChecked());
    ui->action_Normal_Size->setEnabled(!ui->action_Fit_to_Window->isChecked());
}

void MainWindow::on_sliderRateDivisionFactor_valueChanged(int value)
{
    ui->lblRateDivisionFactor->setText(QString::number(value));
}

void MainWindow::zoomIn()
{
    scaleImage(1.25);
}

void MainWindow::zoomOut()
{
   scaleImage(0.8);
}

void MainWindow::normalSize()
{
    m_imageLabel->adjustSize();
    m_scaleFactor = 1.0;
}

void MainWindow::fitToWindow()
{
    bool fitToWindow = ui->action_Fit_to_Window->isChecked();
    ui->scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        normalSize();
    updateActions();
}


void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionZoom_In_25_triggered()
{
    zoomIn();
}

void MainWindow::on_actionZoom_Out_25_triggered()
{
    zoomOut();
}

void MainWindow::on_action_Normal_Size_triggered()
{
    normalSize();
}

void MainWindow::on_action_Fit_to_Window_triggered()
{
    fitToWindow();
    m_settings->setValue("fitSize", ui->action_Fit_to_Window->isChecked());
}

void MainWindow::on_ckAutoSave_stateChanged(int arg1)
{
    Q_UNUSED(arg1)

    m_settings->setValue("autoSave", ui->ckAutoSave->isChecked());

}

void MainWindow::tcpserver_newConnection()
{
    qDebug() << "new client connect";
       QTcpSocket *socket = m_tcpServer->nextPendingConnection();
       ListenThread *t = new ListenThread(socket, this);
       connect(t, SIGNAL(receive_message(const QString&)), this, SLOT(receive_message(const QString&)));
       t->start();

}

void MainWindow::receive_message(const QString &message)
{
    if (message == "GO")
    {
        if (!ui->btnStop->isEnabled())
            on_btnGo_clicked();
    }
    else if (message == "Stop")
    {
        if (!ui->btnGo->isEnabled())
            on_btnStop_clicked();
    }

}

