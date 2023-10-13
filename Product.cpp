#include "Product.h"


int GetID(const QString& key)
{
    return CProductManager::Instance()->GetID(key);
}



////////////////////////////////////////////////////////////////////////////////////
CProductInfo::CProductInfo(QObject *parent) : QObject(parent)
{

}

CProductInfo::~CProductInfo()
{

}


