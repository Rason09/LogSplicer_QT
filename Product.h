#ifndef _PRODUCT_H_
#define _PRODUCT_H_

#include <QObject>
#include <QMap>
#include "DataDefines.h"

int GetID(const QString& key);

///////////////////////////////////////////////////////////////////////////////
class CProductManager
{
public:
    static CProductManager* Instance()
    {
        static CProductManager inst;
        return &inst;
    }


    int GetID(const QString& key)
    {
        return m_mapInfo[key];
    }

private:
    CProductManager()
    {
        m_mapInfo.insert(STR_TERM_ZB_2022,      ID_TERM_ZB_2022);
        m_mapInfo.insert(STR_TERM_ECU_2022,     ID_TERM_ECU_2022);
        m_mapInfo.insert(STR_TERM_698_ORIGINAL, ID_TERM_698_ORIGINAL);
        m_mapInfo.insert(STR_TERM_OTHERS,       ID_TERM_OTHERS);
        m_mapInfo.insert(STR_ORDER_DOWN,        ID_ORDER_DOWN);
        m_mapInfo.insert(STR_ORDER_UP,          ID_ORDER_UP);
        m_mapInfo.insert(STR_COMPRESS_NONE,     ID_COMPRESS_NONE);
        m_mapInfo.insert(STR_COMPRESS_TGZ,      ID_COMPRESS_TGZ);
    }
    ~CProductManager()
    {

    }

    QMap<QString, int> m_mapInfo;      //名称-》标识ID,用于查找名称对应的ID
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CProductInfo : public QObject
{
    Q_OBJECT

public:
    explicit CProductInfo(QObject *parent = nullptr);
    virtual ~CProductInfo();

    CProductInfo(QString name,
                 QString order,
                 QString type,
                 QObject *parent = nullptr)
                : QObject(parent),
                  m_strName(name),
                  m_strOrder(order),
                  m_strType(type)

    {

    }

    CProductInfo(const CProductInfo &product, QObject *parent = nullptr)
            : QObject(parent)
    {
        m_strName  = product.GetName();
        m_strOrder = product.GetOrder();
        m_strType  = product.GetType();
    }

    CProductInfo &operator = (const CProductInfo &product)
    {
        if (this == &product)
        {
            return *this;
        }

        m_strName  = product.GetName();
        m_strOrder = product.GetOrder();
        m_strType  = product.GetType();

        return *this;
    }

    friend bool operator == (const CProductInfo &lhs, const CProductInfo &rhs)
    {
        return (lhs.GetName() == rhs.GetName()
                && lhs.GetOrder() == rhs.GetOrder()
                && lhs.GetType() == rhs.GetType());
    }

    friend bool operator != (const CProductInfo &lhs, const CProductInfo &rhs)
    {
        return !(lhs.GetName() == rhs.GetName()
                && lhs.GetOrder() == rhs.GetOrder()
                && lhs.GetType() == rhs.GetType());
    }

public:
    QString GetName() const
    {
        return m_strName;
    }

    void SetName(const QString &strName)
    {
        m_strName = strName;
    }

    QString GetOrder() const
    {
        return m_strOrder;
    }

    void SetOrder(const QString &strOrder)
    {
        m_strOrder = strOrder;
    }

    QString GetType() const
    {
        return m_strType;
    }

    void SetType(const QString &strType)
    {
        m_strType = strType;
    }

    QString ToString()
    {
        return QString("产品类型：%1，排序方式：%2， 压缩类型：%3").arg(this->GetName()) .arg(this->GetOrder()).arg(this->GetType());
    }


signals:

public slots:

private:

    QString m_strName;        //产品名称
    QString m_strOrder;       //日志排序方式
    QString m_strType;        //日志压缩方式
};


Q_DECLARE_METATYPE(CProductInfo)

#endif
