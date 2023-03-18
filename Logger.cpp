
#include "Logger.h"


CLogger::CLogger(QObject* parent):QObject(parent)
{

}

CLogger::~CLogger()
{

}

void CLogger::Write(const QString& msg)
{
    emit Log(msg);
}



CLogger g_Logger;
void LOG(const QString& msg)
{
    g_Logger.Write(msg);
}



