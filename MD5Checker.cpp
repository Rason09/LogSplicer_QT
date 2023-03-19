
#include "MD5Checker.h"
#include <QFile>
#include <QByteArray>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QDebug>


CMD5Checker::CMD5Checker(QObject* parent):QObject(parent), QRunnable()
{

}

CMD5Checker::~CMD5Checker()
{

}


void CMD5Checker::run()
{
    QString strMD5;
    Calculate(m_FilePath, strMD5);
    emit SendResult(m_iIndex, strMD5);

}

void CMD5Checker::SetFilePath(int iIndex, const QString& strFilePath)
{
    m_iIndex = iIndex;
    m_FilePath = strFilePath;
}

bool CMD5Checker::Calculate(const QString& strFileName, QString& strMD5)
{
    QFile file(strFileName);
    QByteArray fileArray;

    if (!file.open(QIODevice::ReadOnly))
    {
        //打开失败
        qDebug() << strFileName << "打开失败";
        return false;
    }
    fileArray = file.readAll();
    file.close();

    QByteArray md5 = QCryptographicHash::hash(fileArray, QCryptographicHash::Md5);
    strMD5 = md5.toHex();
    return true;
}
