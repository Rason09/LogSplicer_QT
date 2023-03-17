#include "LogSplicer.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>


using namespace std;



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



/**
 * @brief Construct a new CSplicer object
 *
 * @param path
 */
CSplicer::CSplicer(const std::string &path):m_strDir(path)
{

}

/**
 * @brief Destroy the CSplicer object
 *
 */
CSplicer::~CSplicer()
{

}

/**
 * @brief Set the Path object
 *
 * @param path
 */
void CSplicer::SetPath(const std::string &path)
{
    m_strDir = path;
}


/**
 * @brief Get the Path object
 *
 * @return std::string
 */
std::string CSplicer::GetPath() const
{
    return m_strDir;
}

/**
 * @brief Get the Nick Name object
 *
 * @param file
 * @return std::string
 */
std::string CSplicer::GetNickName(const std::string &file)
{
    std::string::size_type pos = file.find(".", 0);
    return file.substr(0, pos);
}

/**
 * @brief 插入文件至匹配文件队列
 *
 * @param file 文件名称，不含路径，含后缀
 * @return true
 * @return false
 */
bool CSplicer::InsertFile(const std::string &file)
{
    std::string strPattern = GetNickName(file);
    auto iter = m_mapFiles.find(strPattern);
    if(iter != m_mapFiles.end())
    {
        iter->second.insert(file);
    }
    else
    {
        std::set<string, CComparer> setFiles;
        setFiles.insert(file);
        m_mapFiles.insert(pair<string, set<string, CComparer>>(strPattern, setFiles));
    }

    return true;
}

/**
 * @brief
 *
 */
void CSplicer::ShowFiles()
{
    for(auto& item : m_mapFiles)
    {
        std::cout << "##############" << item.first << ":" << endl;

        for(auto& file : item.second)
        {
            std::cout << file << endl;
        }
    }
}

/**
 * @brief 遍历当前目录下所有文件，并归类存储
 *
 * @return true
 * @return false
 */
bool CSplicer::ForeachFiles()
{
#ifdef LINUX
    if(!IsDir(m_strDir))
    {
        printf("%s is not directory, error!", m_strDir.c_str());
        return false;
    }

    DIR *pDir;
    struct dirent *pDirent;

    if((pDir = opendir(m_strDir.c_str())) == nullptr)
    {
        printf("open %s dir error!\n", m_strDir.c_str());
        return false;
    }

    if(pDir)
    {
        std::string strFilePath;
        while((pDirent = readdir(pDir)))
        {
            /*跳过"."和".."*/
            if(strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0)
            {
                continue;
            }

            std::string strFileName = pDirent->d_name;
            strFilePath = m_strDir + "/" + strFileName;


            printf("Foreach: m_strDir = %s, stFileName = %s, strFilePath = %s\n", m_strDir.c_str(), strFileName.c_str(), strFilePath.c_str());

            /*跳过目录*/
            if(IsDir(strFilePath))
            {
                printf("Directory(%s) no need splice!\n", strFilePath.c_str());
                continue;
            }

            InsertFile(strFileName);
        }
    }
    closedir(pDir);

#else

    QString srcPath = QString::fromStdString(m_strDir);

    QDir dir(srcPath);
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs);

    /*遍历文件夹*/
    foreach (auto fileInfo, fileInfoList)
    {
        if(fileInfo.isDir())
        {
           continue;
        }

        if(fileInfo.isFile())
        {
            QString qFileName = fileInfo.fileName();
            InsertFile(qFileName.toStdString());
        }
     }

#endif

    return true;
}

/**
 * @brief 拼接日志实现接口
 *
 * @return true
 * @return false
 */
bool CSplicer::SpliceFiles()
{
    for(auto& item : m_mapFiles)
    {
        /*只有一个文件就不需要拼接*/
        if(item.second.size() < 2)
        {
            continue;
        }

        std::string strSpliceLogFile = m_strDir + "/" + "Splice_" + item.first + ".log";
        remove(strSpliceLogFile.c_str());


        for(auto& file : item.second)
        {
            std::string strSrcLogFile = m_strDir + "/" + file;
#ifdef LINUX
            std::string strCmd = "cat " + strSrcLogFile + " >> " + strSpliceLogFile;
            system(strCmd.c_str());
#else
            CopyFile(strSrcLogFile, strSpliceLogFile);
#endif
        }
    }

    return true;
}


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
bool CSplicer::DoProcess()
{
    ForeachFiles();
    SpliceFiles();

    return true;
}


bool CSplicer::CopyFile(const std::string &srcFile, const std::string &dstFile)
{
    QFile qSrcFile(QString::fromStdString(srcFile));
    QFile qDstFile(QString::fromStdString(dstFile));


    if(qSrcFile.open(QFile::ReadOnly))
    {
        QTextStream reader(&qSrcFile);
        QString content = reader.readAll();

        if(qDstFile.open(QFile::WriteOnly | QFile::Text | QFile::Append))
        {
            QTextStream writer(&qDstFile);
            writer << content;

            qDstFile.close();
        }

        qSrcFile.close();
    }

    return true;
}
