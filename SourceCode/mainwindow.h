#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QLabel>
#include <QMutex>
#include <QScrollBar>
#include "multicam.h"
#include "clBBProtocolLib.h"
#include <QTcpServer>
#include <QTcpSocket>

#define ADDRESS_TriggerMode (0x40304)
#define ADDRESS_ExposureMode (0x40404)
#define ADDRESS_ExposureTime_RAW (0x40464)

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


void MCAPI GlobalCallback (PMCSIGNALINFO SigInfo);

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
  void Callback(PMCSIGNALINFO SigInfo);
public slots:
    void setImage(QImage *newImage);
    void displayError(QString message);
signals:
    void imageChanged(QImage *newImage);
    void raiseError(QString message);
private slots:
    void on_pushButton_2_clicked();

    void on_btnGo_clicked();

    void on_btnStop_clicked();

    void on_btnSave_clicked();

    void on_cbExposureMode_currentIndexChanged(int index);

    void on_sliderRateDivisionFactor_valueChanged(int value);

    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();



    void on_actionExit_triggered();

    void on_actionZoom_In_25_triggered();

    void on_actionZoom_Out_25_triggered();

    void on_action_Normal_Size_triggered();

    void on_action_Fit_to_Window_triggered();

    void on_ckAutoSave_stateChanged(int arg1);


    void tcpserver_newConnection();
    void receive_message(const QString &message);
private:
    void getSerialPortList();
    void modifyBalserCameraParameter();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void updateActions();
private:
    Ui::MainWindow *ui;
    QSettings *m_settings;
    MCHANDLE m_Channel;
    int m_SizeX;
    int m_SizeY;
    int m_BufferPitch;
    PVOID m_pCurrent;
    QMutex *m_imageMutex;
    QImage *m_image;
    QLabel *m_imageLabel;
    int m_imageCount;
    double m_scaleFactor = 1;
    QTcpServer *m_tcpServer;

};
#endif // MAINWINDOW_H
