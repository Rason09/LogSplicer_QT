#ifndef TASK_H
#define TASK_H


#include <memory>
#include "LogSplicer.h"
#include "ThreadPool.hpp"




class CLogTask: public CTask
{
public:
    CLogTask(const int product_type, const int compress_type, const std::string & path);
    virtual ~CLogTask();

    bool Execute() override;

private:
    std::unique_ptr<CSplicer> m_ptrSplicer;

    //int m_iProductType;         //当前产品类型
    int m_iCompressType;          //当前日志压缩类型
    //std::string m_strPath;      //当前需要执行日志拼接任务的目录（绝对路径）

};


#endif // TASK_H
