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

extern int g_order;

CWorker::CWorker(QObject *parent) : QObject(parent),QRunnable()
{
    m_filePath = "";

    m_mapInfo.insert(STR_TERM_ECU_2022,     ID_TERM_ECU_2022);
    m_mapInfo.insert(STR_TERM_ZB_2022,      ID_TERM_ZB_2022);
    m_mapInfo.insert(STR_TERM_698_ORIGINAL, ID_TERM_698_ORIGINAL);
    m_mapInfo.insert(STR_TERM_OTHERS,       ID_TERM_OTHERS);
    m_mapInfo.insert(STR_ORDER_DOWN,        ID_ORDER_DOWN);
    m_mapInfo.insert(STR_ORDER_UP,          ID_ORDER_UP);
    m_mapInfo.insert(STR_COMPRESS_NONE,     ID_COMPRESS_NONE);
    m_mapInfo.insert(STR_COMPRESS_TGZ,      ID_COMPRESS_TGZ);

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

    g_order = GetID(m_product.GetOrder());     //开始拼接前先设置好日志排序方式，用于后续set自动排序
    Splice(m_filePath);


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

int CWorker::GetID(const QString& key)
{
    return m_mapInfo[key];
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
                //LOG("正在拼接：" + srcPath);
                m_ptrTool.reset(CreateSplicer(GetID(m_product.GetName()), srcPath.toStdString()));

                if(m_ptrTool)
                {
                    m_ptrTool->DoProcess(GetID(m_product.GetType()));
                }

                bDone = true;
            }

        }
     }

    return true;
}

