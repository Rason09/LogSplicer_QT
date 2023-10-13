#include "Worker.h"
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QDir>
#include "Logger.h"
#include "DataDefines.h"
#include "Task.h"
#include <chrono>
#include <atomic>


extern int g_order;


std::atomic<unsigned int> g_atomicCount(0);  //统计任务个数

CWorker::CWorker(QObject *parent) : QObject(parent),QRunnable()
{
    m_filePath = "";
}

CWorker::~CWorker()
{


}

void CWorker::run()
{
    QDateTime time1 = QDateTime::currentDateTime();//获取系统当前的时间
    uint nStartTime = time1.toTime_t();

    emit SetRunStatus(true);

    LOG(m_product.ToString());
    LOG("=========== 开始拼接：" + m_filePath + " ===========");

    m_uiTaskNum = 0;
    g_atomicCount.store(0);

    g_order = GetID(m_product.GetOrder());     //开始拼接前先设置好日志排序方式，用于后续set自动排序
    Splice(m_filePath);

#ifdef CONCURRENT_MODE
    while(true)
    {
        /*
        if(CThreadPool<CTask>::Instance()->IsIdle())
        {
            LOG("线程执行完成");
            break;
        }
        */

        if(m_uiTaskNum == g_atomicCount.load())
        {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
#endif


    QDateTime time2 = QDateTime::currentDateTime();//获取系统当前的时间
    uint nEndTime = time2.toTime_t();


    QString totalTime = QString::number(nEndTime-nStartTime, 10);
    LOG("=========== 拼接完成，总耗时: " + totalTime + " 秒" + " ===========");


    emit SetRunStatus(false);
}

void CWorker::SetPath(const QString &filePath)
{
    this->m_filePath = filePath;
}

void CWorker::SetProduct(const CProductInfo& obj)
{
    this->m_product = obj;
}

bool CWorker::Splice(const QString &srcPath)
{
    QFileInfo file(srcPath);
    if(!file.isDir())
    {
        LOG(srcPath + "不是目录，退出");
        emit SetRunStatus(false);
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
            LOG("dbCenter跳过！");
            continue;
        }

        if(fileInfo.isDir())
        {
           Splice(fileInfo.filePath());
        }

        if(fileInfo.isFile())
        {

            if(!bDone)
            {


#ifdef CONCURRENT_MODE
                //LOG("正在拼接：" + srcPath);

                m_uiTaskNum++;
                CTask *pTask = new CLogTask(GetID(m_product.GetName()), GetID(m_product.GetType()), srcPath.toStdString());
                CThreadPool<CTask>::Instance()->Append(pTask);
#else
                //LOG("正在拼接：" + srcPath);
                m_ptrTool.reset(CreateSplicer(GetID(m_product.GetName()), srcPath.toStdString()));

                if(m_ptrTool)
                {
                    m_ptrTool->DoProcess(GetID(m_product.GetType()));
                }
#endif

                bDone = true;
            }

        }
     }

    return true;
}

