#ifndef _DATA_DEFINE_H_
#define _DATA_DEFINE_H_


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// 关于产品类型说明：
///
/// 22版能源控制器/集中器(老)  |  降序   |  无压缩
///
/// 22版终端通用             |  降序    | .tgz
///
/// 698传统终端              |  降序    | .tgz
///
///
///
///
///
///
///
///
///
///
///
///
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////




#include <QString>


#define CONCURRENT_MODE        //多线程并发模式


//终端产品类型
enum
{
    ID_TERM_ECU_2022 = 0,         //22集中器和ECU（老），日志不压缩的方式
    ID_TERM_ZB_2022,              //22专变，适用22版所有终端
    ID_TERM_698_ORIGINAL,         //传统698终端
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



const QString STR_TERM_ZB_2022      = "22版终端通用";
const QString STR_TERM_ECU_2022     = "22版能源控制器/集中器(老)";
const QString STR_TERM_698_ORIGINAL = "698传统终端";
const QString STR_TERM_OTHERS       = "其他";


const QString STR_ORDER_DOWN = "降序";
const QString STR_ORDER_UP   = "升序";
const QString STR_ORDER_UNKNOW   = "未知";

const QString STR_COMPRESS_NONE = "无";
const QString STR_COMPRESS_TGZ  = ".tgz";
const QString STR_COMPRESS_UNKNOW  = "未知";


#endif
