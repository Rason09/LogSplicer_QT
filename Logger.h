#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include <QObject>

class CLogger : public QObject
{
    Q_OBJECT

public:
    explicit CLogger(QObject* parent = nullptr);
    virtual ~CLogger();

    void Write(const QString& msg);

signals:
    void Log(const QString& msg);

};


extern CLogger g_Logger;
void LOG(const QString& msg);



#endif
