/*
 *                        _oo0oo_
 *                       o8888888o
 *                       88" . "88
 *                       (| -_- |)
 *                       0\  =  /0
 *                     ___/`---'\___
 *                   .' \\|     |// '.
 *                  / \\|||  :  |||// \
 *                 / _||||| -:- |||||- \
 *                |   | \\\  - /// |   |
 *                | \_|  ''\---/''  |_/ |
 *                \  .-\__  '-'  ___/-. /
 *              ___'. .'  /--.--\  `. .'___
 *           ."" '<  `.___\_<|>_/___.' >' "".
 *          | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *          \  \ `_.   \_ __\ /__ _/   .-` /  /
 *      =====`-.____`.___ \_____/___.-`___.-'=====
 *                        `=---='
 * 
 * 
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *            佛祖保佑     永不宕机     永无BUG
 */

/*
 * @Company: willfar
 * @Description: Description about the file
 * @Version: V1.0
 * @Autor: Rason
 * @Date: 2023-04-13 16:22:35
 * @LastEditors: Rason
 * @LastEditTime: 2023-04-13 19:05:11
 */

#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <vector>
#include <deque>
#include <thread>
#include <functional>
#include <mutex>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <condition_variable>






//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief The CTask class
///
///线程执行的任务基类，使用者需要继承该类并实现Execute方法
class CTask
{
public:
    CTask(){};
    virtual ~CTask(){};

    virtual bool Execute() = 0;
};




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static std::string get_tid()
{
    std::stringstream tmp;
    tmp << std::this_thread::get_id();
    return tmp.str();
}



/**
 * @brief 线程池简单实现
 * 
 * @tparam T 任务：可以是对象，可以是函数，结构体等等
 */
template <typename T>
class CThreadPool
{
public:
    CThreadPool(const CThreadPool&) = delete;
    const CThreadPool& operator=(const CThreadPool&) = delete;

    static CThreadPool<T>* Instance()
    {
        static CThreadPool<T> inst;
        return &inst;
    }

    void Start();
    void Stop();
    void Append(T* task); 
    void SetThreadNum(int num);

    bool IsIdle();

private:
     CThreadPool(int init_size=16);
    ~CThreadPool();

    bool IsStarted();

    void ThreadLoop();
    T* GetTask();

    typedef std::vector<std::thread*> WorkThreads;
    typedef std::deque<T*> TaskQueue;

    int m_iThreadSize;

    WorkThreads m_vThreads;
    TaskQueue m_queTasks;

    std::mutex m_mutex;
    std::condition_variable m_cond;

    bool m_bStarted;
};



template <typename T>
CThreadPool<T>::CThreadPool(int init_size):m_iThreadSize(init_size),m_mutex(),m_cond(),m_bStarted(false)
{
    //Start();
}


template <typename T>
CThreadPool<T>::~CThreadPool()
{
    if(m_bStarted)
    {
        Stop();
    }
}

template <typename T>
void CThreadPool<T>::SetThreadNum(int num)
{
    m_iThreadSize = num;
}


template <typename T>
void CThreadPool<T>::Start()
{
    assert(m_vThreads.empty());

    m_bStarted = true;
    m_vThreads.reserve(m_iThreadSize);

    for (int i = 0; i < m_iThreadSize; ++i)
    {
        m_vThreads.push_back(new std::thread(std::bind(&CThreadPool::ThreadLoop, this)));
    }
}


template <typename T>
void CThreadPool<T>::Stop()
{
    printf("CThreadPool::stop() stop.\n");
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_bStarted = false;
        m_cond.notify_all();
        printf("CThreadPool::stop() notifyAll().\n");
    }

    for (auto thread : m_vThreads)
    {
        thread->join();
        delete thread;
    }

    for(auto task : m_queTasks)
    {
        delete task;
    }

    m_vThreads.clear();
}


template <typename T>
bool CThreadPool<T>::IsStarted() 
{ 
    return m_bStarted; 
}


template <typename T>
bool CThreadPool<T>::IsIdle()
{
    return m_queTasks.empty();
}

template <typename T>
void CThreadPool<T>::ThreadLoop()
{
    printf("CThreadPool::threadLoop() tid : %s start.\n", get_tid().c_str() );
    while(m_bStarted)
    {
        T* pTask = GetTask();
        if(pTask)
        {
            pTask->Execute();
            delete pTask;
        }
    }
    printf("CThreadPool::threadLoop() tid : %s exited.\n", get_tid().c_str());
}



template <typename T>
void CThreadPool<T>::Append(T* task)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    /*while(m_tasks.isFull())
    {//when m_tasks have maxsize
    cond2.notify_one();
    }
    */
    m_queTasks.push_back(task);
    m_cond.notify_one();
}


template <typename T>
T* CThreadPool<T>::GetTask()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    //always use a while-loop, due to spurious wakeup
    while(m_queTasks.empty() && m_bStarted)
    {
        printf("CThreadPool::take() tid : %s wait.\n", get_tid().c_str());
        m_cond.wait(lock);
    }

    printf("CThreadPool::take() tid : %s wakeup.\n", get_tid().c_str());

    T* ptrTask = nullptr;

    int size = m_queTasks.size();
    if(!m_queTasks.empty() && m_bStarted)
    {
        ptrTask = m_queTasks.front();
        m_queTasks.pop_front();
        assert(size - 1 == m_queTasks.size());
        /*if (TaskQueueSize_ > 0)
        {
            cond2.notify_one();
        }*/
    }

    return ptrTask;
}



#endif   /**_THREAD_POOL_H_*/

