#include "saveimagethread.h"
#include <QDateTime>
#include <QPixmap>

SaveImageThread::SaveImageThread(QString path, QImage *image, QObject *parent) :
    QThread(parent),
    m_image(image),
    m_path(path)

{

}

void SaveImageThread::run()
{
     QString fileName = QString("%1/%2.bmp").arg(m_path).arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    QPixmap tmp = QPixmap::fromImage(*m_image);
    tmp.save(fileName);
    delete m_image;
}
