#include "md5_dialog.h"
#include "ui_md5_dialog.h"
#include <QFileDialog>
#include <QThreadPool>
#include <QMessageBox>

MD5_Dialog::MD5_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MD5_Dialog)
{
    ui->setupUi(this);

    setFixedSize(this->width(), this->height());     //禁止拉伸
    setWindowModality(Qt::ApplicationModal);         //设置模态窗口,即窗口没关闭不允许其他操作

    ui->lineEdit_FilePath->setReadOnly(true);
    ui->lineEdit_FilePath_2->setReadOnly(true);
    ui->lineEdit_MD5value->setReadOnly(true);
    ui->lineEdit_MD5value_2->setReadOnly(true);

    m_Checker = new CMD5Checker();
    m_Checker->setAutoDelete(false);

    connect(m_Checker, SIGNAL(SendResult(int, QString)), this, SLOT(ShowResult(int, QString)));
}

MD5_Dialog::~MD5_Dialog()
{
    delete m_Checker;
    delete ui;
}

void MD5_Dialog::ShowResult(int iIndex, const QString& value)
{
    if(iIndex == 1)
    {
        ui->lineEdit_MD5value->setText(value);
    }
    else if(iIndex == 2)
    {
        ui->lineEdit_MD5value_2->setText(value);
    }
    else
    {

    }
}

void MD5_Dialog::on_pushbtn_OpenFile_clicked()
{
    QString stFilePath = QFileDialog::getOpenFileName(this, tr("选择文件"), tr(""), tr(""));
    ui->lineEdit_FilePath->setText(stFilePath);
}


void MD5_Dialog::on_pushBtn_StartCal_clicked()
{
    if(ui->lineEdit_FilePath->text().isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("文件不能为空"));
        return;
    }

    ui->lineEdit_MD5value->clear();


    m_Checker->SetFilePath(1, ui->lineEdit_FilePath->text());
    QThreadPool::globalInstance()->start(m_Checker);


    /*
    QString strValue;
    m_Checker->Calculate(ui->lineEdit_FilePath->text(), strValue);
    ui->lineEdit_MD5value->setText(strValue);
    */
}


void MD5_Dialog::on_pushbtn_OpenFile_2_clicked()
{
    QString stFilePath = QFileDialog::getOpenFileName(this, tr("选择文件"), tr(""), tr(""));
    ui->lineEdit_FilePath_2->setText(stFilePath);
}


void MD5_Dialog::on_pushBtn_StartCal_2_clicked()
{
    if(ui->lineEdit_FilePath_2->text().isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("文件不能为空"));
        return;
    }

    ui->lineEdit_MD5value_2->clear();

    m_Checker->SetFilePath(2, ui->lineEdit_FilePath_2->text());
    QThreadPool::globalInstance()->start(m_Checker);

    /*
    QString strValue;
    m_Checker->Calculate(ui->lineEdit_FilePath_2->text(), strValue);
    ui->lineEdit_MD5value_2->setText(strValue);
    */

}

