#include "Task.h"
#include <atomic>


extern std::atomic<unsigned int> g_atomicCount;

CLogTask::CLogTask(const int product_type, const int compress_type, const std::string & path):m_iCompressType(compress_type)
{
    m_ptrSplicer.reset(CreateSplicer(product_type, path));
}

CLogTask::~CLogTask()
{

}


bool CLogTask::Execute()
{
    bool bRst = false;
    if(m_ptrSplicer)
    {
        bRst =  m_ptrSplicer->DoProcess(m_iCompressType);
    }

    g_atomicCount++;

    return bRst;
}
