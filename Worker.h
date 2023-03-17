#ifndef _WORKER_H_
#define _WORKER_H_

#include <QObject>
#include <QRunnable>

#define DATA_SIZE 1024*1024

class CWorker : public QObject,public QRunnable
{
    Q_OBJECT
public:
    explicit CWorker(QObject *parent = nullptr);
    void run() override;

    void setPath(const QString &filePath);

signals:
    void setRunStatus(bool);
    void logMessage(const QString& msg);

private:
    bool splice(const QString &srcPath);

private:
    QString filePath;

};

#endif
