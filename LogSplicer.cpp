#include "LogSplicer.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QProcess>
#include <QApplication>
#include <QObject>


using namespace std;


int g_order = ID_ORDER_DOWN;

CSplicer* CreateSplicer(int iTermType, const std::string& path)
{
    CSplicer *ptr = nullptr;
    switch(iTermType)
    {
        case ID_TERM_ECU_2022:
            ptr = new CSplicerECU_2022(path);
            break;
        case ID_TERM_ZB_2022:
        case ID_TERM_698_ORIGINAL:
            ptr = new CSplicerZB_2022(path);
            break;
        case ID_TERM_OTHERS:
            ptr = new CSplicerOthers(path);
            break;
        default:
            LOG("不支持的产品类型" + QString::number(iTermType));
            break;

    }

    return ptr;
}



////////////////////////////////////// CSplicer ////////////////////////////////////////

/**
 * @brief ?ж???????
 *  S_ISLNK(st_mode)????????????.
    S_ISREG(st_mode)???????????????.
    S_ISDIR(st_mode)??????????
    S_ISCHR(st_mode)????????????豸.
    S_ISBLK(st_mode)???????????豸
    S_ISFIFO(st_mode)????????FIFO???.
    S_ISSOCK(st_mode)????????SOCKET???.
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
 * @brief ???????????????????
 *
 * @param file ????????????·?????????
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
        //std::cout << "##############" << item.first << ":" << endl;

        for(auto& file : item.second)
        {
            std::cout << file << endl;
        }
    }
}

/**
 * @brief ????????????????????????????
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
            /*????"."??".."*/
            if(strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0)
            {
                continue;
            }

            std::string strFileName = pDirent->d_name;
            strFilePath = m_strDir + "/" + strFileName;


            printf("Foreach: m_strDir = %s, stFileName = %s, strFilePath = %s\n", m_strDir.c_str(), strFileName.c_str(), strFilePath.c_str());

            /*??????*/
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

    /*?????????*/
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
 * @brief ?????????????????????
 * struct dirent
    {
    long d_ino; 				   //inode number ?????????
    off_t d_off; 				   //offset to this dirent ????????е????
    unsigned short d_reclen; 	   //length of this d_name ???????
    unsigned char d_type; 		   //the type of d_name ???????
    char d_name [NAME_MAX+1]; 	   //file name (null-terminated) ?????????255???
    }
 * @param m_strDir
 * @return true
 * @return false
 */
bool CSplicer::DoProcess(int iOrder, int iCompress)
{
    ForeachFiles();
    SpliceFiles(iOrder, iCompress);

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

bool CSplicer::ProcessTgzType()
{

    QString tempFolderPath = QString::fromStdString(m_strDir) + "/temp";     //临时文件夹，用于存放解压后的日志文件

    QDir tempDir(tempFolderPath);
    if (!tempDir.exists())
    {
        tempDir.mkdir(tempFolderPath);
    }


    for(auto& item : m_mapFiles)
    {
        /*只有1个文件就不需要拼接*/
        if(item.second.size() < 2)
        {
            continue;
        }

        std::string strSpliceLogFile = m_strDir + "/" + "Splice_" + item.first + ".log";
        remove(strSpliceLogFile.c_str());


        for(auto& file : item.second)
        {
            std::string strSrcLogFile = m_strDir + "/" + file;

            if(QString::fromStdString(file).right(4) == ".tgz")
            {
                if(ExtractTarGz(QString::fromStdString(strSrcLogFile), tempFolderPath))
                {
                    std::string strNewSrcLogFile = tempFolderPath.toStdString() + "/" + item.first + ".log";
                    CopyFile(strNewSrcLogFile, strSpliceLogFile);
                }
            }
            else
            {
                CopyFile(strSrcLogFile, strSpliceLogFile);
            }


        }
        LOG("拼接日志: " + QString::fromStdString(strSpliceLogFile));
    }

    tempDir.removeRecursively();

    return true;
}

bool CSplicer::ExtractTarGz(QString fileName, QString dstPath)
{
    if(fileName.length()<5 || fileName.right(4)!=".tgz")
    {
        LOG(fileName + "文件格式错误, 必须是 '.tgz'");
        return false;
    }
    if(dstPath.length() == 0)
    {
        LOG("dest path is null!");
        return false;
    }
    int ret;
    QProcess p;

    //LOG(fileName + " Compress To " + dstPath);

    fileName.replace("/", "\\");
    //exe????·??
    QString binPath = QApplication::applicationDirPath() + "/";
    binPath.replace("/", "\\");
    qDebug()<<binPath;

    //1.先用gzip.exe -dfk filename.tgz解压成filename.tar
    QStringList args;
    args.append("-d");
    args.append("-f");
    args.append("-k");
    args.append(fileName);
    ret = p.execute(binPath + "gzip.exe", args);
    if(ret != 0)
    {
        LOG("gzip 命令执行失败");
        return false;
    }
    //qDebug()<<"file.tgz -> file.tar OK!";

    //2.去掉原文件的后缀.tgz, 新文件后缀为.tar
    fileName = fileName.left(fileName.length()-4) + ".tar";

    //3.使用7za.exe x filename.tar -y -o dstDir
    QStringList args1;
    args1.append("x");
    args1.append(fileName);
    args1.append("-y");
    dstPath.replace("/", "\\");
    args1.append("-o" + dstPath);
    p.execute(binPath + "7za.exe", args1);
    if(ret != 0)
    {
        LOG("7za 命令执行失败");
        return false;
    }
    //qDebug()<<"file.tar -> file OK!";

    QFile::remove(fileName);      //解压之后的新文件filename.tar要记得删除

    return true;
}

bool CSplicer::ProcessNoneType()
{
    for(auto& item : m_mapFiles)
    {
        /*只有1个文件就没必要拼接了*/
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
        LOG("拼接日志: " + QString::fromStdString(strSpliceLogFile));
    }

    return true;
}

////////////////////////////////////// CSplicerECU_2022 ////////////////////////////////////////
/**
 * @brief ???????????
 *
 * @return true
 * @return false
 */
bool CSplicerECU_2022::SpliceFiles(int iOrder, int iCompress)
{
    g_order = iOrder;

    if(iCompress == ID_COMPRESS_NONE)
    {
        ProcessNoneType();
    }
    else
    {
        LOG("暂不支持22版EUU日志压缩类型tgz");
        return false;
    }

    return true;
}




////////////////////////////////////// CSplicerZB_2022 ////////////////////////////////////////
/**
 * @brief ???????????
 *
 * @return true
 * @return false
 */
bool CSplicerZB_2022::SpliceFiles(int iOrder, int iCompress)
{
    g_order = iOrder;

    if(iCompress == ID_COMPRESS_TGZ)
    {
        ProcessTgzType();
    }
    else
    {
        LOG("暂不支持22版专变3型日志无压缩类型");
        return false;
    }


    return true;
}

////////////////////////////////////// CSplicerOthers ////////////////////////////////////////

bool CSplicerOthers::SpliceFiles(int iOrder, int iCompress)
{
    g_order = iOrder;

    switch(iCompress)
    {
        case ID_COMPRESS_NONE:
            return ProcessNoneType();
            break;
        case ID_COMPRESS_TGZ:
            return ProcessTgzType();
            break;
        default:
            break;
    }

    return true;
}



