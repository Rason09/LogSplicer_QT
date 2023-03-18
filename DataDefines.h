#ifndef _DATA_DEFINE_H_
#define _DATA_DEFINE_H_


#include <QString>


//终端产品类型
enum
{
    ID_TERM_ECU_2022 = 0,
    ID_TERM_ZB_2022,
    ID_TERM_698_ORIGINAL,
    ID_TERM_OTHERS,
};

//转存日志文件排序类型
enum
{
    ID_ORDER_DOWN = 0,
    ID_ORDER_UP,
};


//转存日志文件压缩类型
enum
{
    ID_COMPRESS_NONE = 0,
    ID_COMPRESS_TGZ,
};



const QString STR_TERM_ECU_2022     = "22版能源控制器/集中器";
const QString STR_TERM_ZB_2022      = "22版专变III";
const QString STR_TERM_698_ORIGINAL = "698传统终端";
const QString STR_TERM_OTHERS       = "其他";


const QString STR_ORDER_DOWN = "降序";
const QString STR_ORDER_UP   = "升序";
const QString STR_ORDER_UNKNOW   = "未知";

const QString STR_COMPRESS_NONE = "无";
const QString STR_COMPRESS_TGZ  = ".tgz";
const QString STR_COMPRESS_UNKNOW  = "未知";


#endif
