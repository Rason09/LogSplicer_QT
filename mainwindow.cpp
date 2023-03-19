#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QScrollBar>
#include <QMessageBox>
#include <QThreadPool>
#include "Product.h"
#include "Logger.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*界面布局的初始化操作*/
    setWindowIcon(QIcon("://star.ico"));
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);  //禁止最大化
    setFixedSize(this->width(), this->height());     //禁止拉伸

    ui->plainTxtEditLog->setReadOnly(true);
    ui->lineEdtDir->setReadOnly(true);

#if 0
    QStringList strProductList;
    strProductList << "22版集中器/能源控制器" << "22版专变III" << "19版能源控制器" << "传统698终端";

    ui->cbBoxProduct->clear();
    ui->cbBoxProduct->addItems(strProductList);
#endif
    LoadProductsToComboBox();


    QStringList strOrderList;
    strOrderList << STR_ORDER_DOWN << STR_ORDER_UP;
    ui->cbBoxOrder->clear();
    ui->cbBoxOrder->addItems(strOrderList);

    QStringList strZipList;
    strZipList << STR_COMPRESS_NONE << STR_COMPRESS_TGZ;
    ui->cbBoxZipType->clear();
    ui->cbBoxZipType->addItems(strZipList);


    ui->cbBoxOrder->setDisabled(true);
    ui->cbBoxZipType->setDisabled(true);



    /*工作线程*/
    m_workThread = new CWorker;
    m_workThread->setAutoDelete(false);   //不自动销毁，手动销毁


    /*注册信号槽*/
    connect(m_workThread, SIGNAL(SetRunStatus(bool)), this, SLOT(SetRunStatus(bool)));
    connect(&g_Logger, SIGNAL(Log(QString)), this, SLOT(LogMessage(QString)));
}

MainWindow::~MainWindow()
{
    for(auto item : m_vProducts)
    {
        delete item;
    }

    delete m_workThread;
    delete ui;
}

void MainWindow::LoadProductsToComboBox()
{
    m_vProducts.clear();
    m_vProducts.push_back(new CProductInfo(STR_TERM_ECU_2022, STR_ORDER_DOWN, STR_COMPRESS_NONE));
    m_vProducts.push_back(new CProductInfo(STR_TERM_ZB_2022, STR_ORDER_DOWN, STR_COMPRESS_TGZ));
    m_vProducts.push_back(new CProductInfo(STR_TERM_698_ORIGINAL, STR_ORDER_DOWN, STR_COMPRESS_TGZ));
    m_vProducts.push_back(new CProductInfo(STR_TERM_OTHERS, STR_ORDER_UNKNOW, STR_COMPRESS_UNKNOW));

    ui->cbBoxProduct->clear();

    for (auto& product : m_vProducts)
    {
        QVariant var;
        var.setValue(*product);
        ui->cbBoxProduct->addItem(product->GetName(), var);
    }

    connect( ui->cbBoxProduct,
             SIGNAL(currentIndexChanged(int)),
             this,
             SLOT(onProductComboBoxIndexChangeSlot(int)) );
}

void MainWindow::AddLog(const char *fmt,...)
{
    QPlainTextEdit *plainTextEdit = ui->plainTxtEditLog;
    // 设置当前控件为只读
    if(!ui->plainTxtEditLog->isReadOnly())
    {
        ui->plainTxtEditLog->setReadOnly(true);
    }

    // 设置光标到文本末尾
    plainTextEdit->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);

    va_list list;
    va_start(list,fmt);
    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    QString str = QString().vsprintf(fmt,list);
    va_end(list);
    QString strtext = "";
    strtext.sprintf("[%04d-%02d-%02d %02d:%02d:%02d.%03d]  %s\r\n",date.year(),date.month(),date.day(),time.hour(),time.minute(),time.second(),time.msec(),str.toStdString().data());

    ui->plainTxtEditLog->insertPlainText(strtext);

    // 移动滚动条到底部
    QScrollBar * scrollbar = ui->plainTxtEditLog->verticalScrollBar();
    if(scrollbar)
    {
        scrollbar->setSliderPosition(scrollbar->maximum());
    }
}

void MainWindow::SetRunStatus(bool bRun)
{
    m_bRun = bRun;

    ui->cbBoxProduct->setDisabled(bRun);
    ui->lineEdtDir->setDisabled(bRun);
    ui->btnOpenFile->setDisabled(bRun);
    ui->btnRun->setDisabled(bRun);

}

void MainWindow::LogMessage(const QString& msg)
{
    QPlainTextEdit *plainTextEdit = ui->plainTxtEditLog;
    // 设置当前控件为只读
    if(!ui->plainTxtEditLog->isReadOnly())
    {
        ui->plainTxtEditLog->setReadOnly(true);
    }

    // 设置光标到文本末尾
    plainTextEdit->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);

    QDateTime dateTime= QDateTime::currentDateTime();
    QString strtext = dateTime .toString("[yyyy-MM-dd hh:mm:ss] ") + msg + "\r\n";

    ui->plainTxtEditLog->insertPlainText(strtext);

    // 移动滚动条到底部
    QScrollBar * scrollbar = ui->plainTxtEditLog->verticalScrollBar();
    if(scrollbar)
    {
        scrollbar->setSliderPosition(scrollbar->maximum());
    }
}


void MainWindow::on_btnOpenFile_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::currentPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->lineEdtDir->setText(dir);
}

void MainWindow::on_btnRun_clicked()
{
    if(ui->lineEdtDir->text().isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("目录不能为空"));
        return;
    }
    else
    {
        ui->plainTxtEditLog->clear();      //每次执行都清除一下日志

        CProductInfo product = ui->cbBoxProduct->currentData().value<CProductInfo>();
        if(product.GetName() == "其他")
        {
            product.SetOrder(ui->cbBoxOrder->currentText());
            product.SetType(ui->cbBoxZipType->currentText());
        }

        m_workThread->SetProduct(product);
        m_workThread->SetPath(ui->lineEdtDir->text());
        QThreadPool::globalInstance()->start(m_workThread);
    }
}

void MainWindow::onProductComboBoxIndexChangeSlot(int iIndex)
{
    CProductInfo product = ui->cbBoxProduct->itemData(iIndex).value<CProductInfo>();

    //LOG(product.ToString());

    //排序和压只有选择其他产品类型才可以
    if(product.GetName() == "其他")
    {
        QMessageBox::warning(this, tr("提示"), tr("请选择日志排序方式以及压缩类型！"));
        ui->cbBoxOrder->setDisabled(false);
        ui->cbBoxZipType->setDisabled(false);
        ui->cbBoxOrder->setFocus();
    }
    else
    {
        ui->cbBoxOrder->setDisabled(true);
        ui->cbBoxZipType->setDisabled(true);
    }
}

void MainWindow::on_action_about_triggered()
{
    QMessageBox::information(this, tr("信息"), tr("作者：Rason！"));
}


void MainWindow::on_action_MD5Func_triggered()
{
    //QMessageBox::warning(this, tr("提示"), tr("功能暂未开发，敬请期待！"));

    m_pDlgMD5 = new MD5_Dialog();
    m_pDlgMD5->show();
}


void MainWindow::on_action_OtherFuncs_triggered()
{
    QMessageBox::warning(this, tr("提示"), tr("功能暂未开发，敬请期待！"));
}

