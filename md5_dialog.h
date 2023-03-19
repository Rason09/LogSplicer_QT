#ifndef MD5_DIALOG_H
#define MD5_DIALOG_H

#include <QDialog>
#include "MD5Checker.h"

namespace Ui {
class MD5_Dialog;
}

class MD5_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit MD5_Dialog(QWidget *parent = nullptr);
    ~MD5_Dialog();

public slots:
    void ShowResult(int iIndex, const QString& value);

private slots:
    void on_pushbtn_OpenFile_clicked();

    void on_pushBtn_StartCal_clicked();

    void on_pushbtn_OpenFile_2_clicked();

    void on_pushBtn_StartCal_2_clicked();



private:
    Ui::MD5_Dialog *ui;

    CMD5Checker *m_Checker;
};

#endif // MD5_DIALOG_H
