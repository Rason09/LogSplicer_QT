#ifndef _LOG_SPLICER_H_
#define _LOG_SPLICER_H_

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <memory>

#ifdef LINUX
#include <sys/dir.h>
#include <sys/stat.h>
#endif

#include <cstdio>
#include <QFileInfo>

using namespace std;

#define LOG_DOWN_ORDER 1

/**
 * @brief 定义函数对象类
 * 
 */
class CComparer
{
public:
    bool operator()(const string& lhs, const string& rhs)  
    {
#ifdef LOG_DOWN_ORDER
        if(lhs.length() == rhs.length())
        {
            return lhs > rhs;
        }
        else
        {
            return lhs.length() > rhs.length();
        }
#else
        if(lhs.length() == rhs.length())
        {
            return lhs < rhs;
        }
        else
        {
            return lhs.length() < rhs.length();
        }
#endif
    }
};

/**
 * @brief 判断是否为目录
 *  S_ISLNK(st_mode)是否是一个连接.
    S_ISREG(st_mode)是否是一个常规文件.
    S_ISDIR(st_mode)是否是一个目录
    S_ISCHR(st_mode)是否是一个字符设备.
    S_ISBLK(st_mode)是否是一个块设备
    S_ISFIFO(st_mode)是否是一个FIFO文件.
    S_ISSOCK(st_mode)是否是一个SOCKET文件.
 * @return true 
 * @return false 
*/



class CSplicer
{
    using MapPatternFiles = std::map<string, set<string, CComparer>>;   //<pattern,set<files>>

public:
    /**
     * @brief Construct a new CSplicer object
     * 
     * @param path 
     */
    CSplicer(const std::string &path);


    /**
     * @brief Destroy the CSplicer object
     * 
     */
    ~CSplicer();


    /**
     * @brief Set the Path object
     * 
     * @param path 
     */
    void SetPath(const std::string &path);


    /**
     * @brief Get the Path object
     * 
     * @return std::string 
     */
    std::string GetPath() const;


    /**
     * @brief Get the Nick Name object
     * 
     * @param file 
     * @return std::string 
     */
    std::string GetNickName(const std::string &file);


    /**
     * @brief 插入文件至匹配文件队列
     * 
     * @param file 文件名称，不含路径，含后缀
     * @return true 
     * @return false 
     */
    bool InsertFile(const std::string &file);


    /**
     * @brief 
     * 
     */
    void ShowFiles();


    /**
     * @brief 遍历当前目录下所有文件，并归类存储
     * 
     * @return true 
     * @return false 
     */
    bool ForeachFiles();




    /**
     * @brief 拼接日志实现接口
     * 
     * @return true 
     * @return false 
     */
    bool SpliceFiles();

    /**
     * @brief 拼接日志处理接口，对外使用
     * struct dirent
        {
        long d_ino; 				   //inode number 索引节点号
        off_t d_off; 				   //offset to this dirent 在目录文件中的偏移 
        unsigned short d_reclen; 	   //length of this d_name 文件名长 
        unsigned char d_type; 		   //the type of d_name 文件类型 
        char d_name [NAME_MAX+1]; 	   //file name (null-terminated) 文件名，最长255字符 
        }
     * @param m_strDir 
     * @return true 
     * @return false 
     */
    bool DoProcess();

    bool CopyFile(const std::string &srcFile, const std::string &dstFile);

private:

    std::string m_strDir;
    MapPatternFiles m_mapFiles;

};


#endif
