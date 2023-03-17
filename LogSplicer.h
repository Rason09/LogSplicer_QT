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
 * @brief ���庯��������
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
 * @brief �ж��Ƿ�ΪĿ¼
 *  S_ISLNK(st_mode)�Ƿ���һ������.
    S_ISREG(st_mode)�Ƿ���һ�������ļ�.
    S_ISDIR(st_mode)�Ƿ���һ��Ŀ¼
    S_ISCHR(st_mode)�Ƿ���һ���ַ��豸.
    S_ISBLK(st_mode)�Ƿ���һ�����豸
    S_ISFIFO(st_mode)�Ƿ���һ��FIFO�ļ�.
    S_ISSOCK(st_mode)�Ƿ���һ��SOCKET�ļ�.
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
     * @brief �����ļ���ƥ���ļ�����
     * 
     * @param file �ļ����ƣ�����·��������׺
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
     * @brief ������ǰĿ¼�������ļ���������洢
     * 
     * @return true 
     * @return false 
     */
    bool ForeachFiles();




    /**
     * @brief ƴ����־ʵ�ֽӿ�
     * 
     * @return true 
     * @return false 
     */
    bool SpliceFiles();

    /**
     * @brief ƴ����־����ӿڣ�����ʹ��
     * struct dirent
        {
        long d_ino; 				   //inode number �����ڵ��
        off_t d_off; 				   //offset to this dirent ��Ŀ¼�ļ��е�ƫ�� 
        unsigned short d_reclen; 	   //length of this d_name �ļ����� 
        unsigned char d_type; 		   //the type of d_name �ļ����� 
        char d_name [NAME_MAX+1]; 	   //file name (null-terminated) �ļ������255�ַ� 
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
