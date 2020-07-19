#ifndef SAVEIMAGETHREAD_H
#define SAVEIMAGETHREAD_H

#include <QThread>
#include <QImage>

class SaveImageThread : public QThread
{
    Q_OBJECT
public:
    explicit SaveImageThread(QString path, QImage *image, QObject *parent = nullptr);

    void run() override;
signals:
private:
    QImage* m_image;
    QString m_path;

};

#endif // SAVEIMAGETHREAD_H
