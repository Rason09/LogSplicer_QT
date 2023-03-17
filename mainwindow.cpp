#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QScrollBar>
#include <QMessageBox>
#include <QThreadPool>

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


    /*工作线程*/
    m_workThread = new CWorker;


    /*注册信号槽*/
    connect(m_workThread, SIGNAL(setRunStatus(bool)), this, SLOT(SetRunStatus(bool)));
    connect(m_workThread, SIGNAL(logMessage(QString)), this, SLOT(LogMessage(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
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
        m_workThread->setPath(ui->lineEdtDir->text());
        QThreadPool::globalInstance()->start(m_workThread);
    }
}

