
#ifndef _MD5_CHECKER_H_
#define _MD5_CHECKER_H_

#include <QObject>
#include <QRunnable>


class CMD5Checker : public QObject,public QRunnable
{
    Q_OBJECT
public:
    explicit CMD5Checker(QObject *parent = nullptr);
    ~CMD5Checker();
    void run() override;

    void SetFilePath(int iIndex, const QString& strFilePath);

    bool Calculate(const QString& strFileName, QString& strMD5);

signals:
    void SendResult(int iIndex, const QString& value);

private:


private:
    int m_iIndex;
    QString m_FilePath;

};

#endif
