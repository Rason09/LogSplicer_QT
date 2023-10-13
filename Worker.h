#ifndef _WORKER_H_
#define _WORKER_H_

#include <QObject>
#include <QRunnable>
#include <QMap>
#include "Product.h"
#include "LogSplicer.h"

#define DATA_SIZE 1024*1024

class CWorker : public QObject,public QRunnable
{
    Q_OBJECT
public:
    explicit CWorker(QObject *parent = nullptr);
    virtual ~CWorker();
    void run() override;

    void SetPath(const QString &filePath);
    void SetProduct(const CProductInfo& obj);

signals:
    void SetRunStatus(bool);

private:
    bool Splice(const QString &srcPath);

private:
    CProductInfo m_product;
    QString m_filePath;

    std::unique_ptr<CSplicer> m_ptrTool;

    unsigned int m_uiTaskNum;          //并发任务个数

};

#endif
