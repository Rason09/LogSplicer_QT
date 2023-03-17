#include "Worker.h"
#include "LogSplicer.h"
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QDir>


CWorker::CWorker(QObject *parent) : QObject(parent),QRunnable()
{
    filePath = "";
}

void CWorker::run()
{
    QDateTime time1 = QDateTime::currentDateTime();//获取系统当前的时间
    uint nStartTime = time1.toTime_t();

    emit setRunStatus(true);
    emit logMessage("=========== 开始拼接：" + filePath + " ===========");


   splice(filePath);


    QDateTime time2 = QDateTime::currentDateTime();//获取系统当前的时间
    uint nEndTime = time2.toTime_t();


    QString totalTime = QString::number(nEndTime-nStartTime, 10);
    emit logMessage("=========== 拼接完成，总耗时: " + totalTime + " 秒" + " ===========");
    emit setRunStatus(false);
}

void CWorker::setPath(const QString &filePath)
{
    this->filePath = filePath;
}

bool CWorker::splice(const QString &srcPath)
{
    QFileInfo file(srcPath);
    if(!file.isDir())
    {
        emit logMessage(srcPath + "不是目录，退出");
        emit setRunStatus(false);
        return false;
    }

    bool bDone = false;    //当前目录拼接标记

    QDir dir(srcPath);
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs);

    /*遍历文件夹*/
    foreach (auto fileInfo, fileInfoList)
    {
        if(fileInfo.fileName() == "dbCenter")
        {
            emit logMessage("dbCenter跳过！");
            continue;
        }

        if(fileInfo.isDir())
        {
           splice(fileInfo.filePath());
        }

        if(fileInfo.isFile())
        {

            if(!bDone)
            {
                emit logMessage("正在拼接：" + srcPath);
                std::unique_ptr<CSplicer> ptr(new CSplicer(srcPath.toStdString()));

                ptr->DoProcess();

                bDone = true;
            }

        }
     }

    return true;
}

