#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Worker.h"
#include "Product.h"
#include "md5_dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void AddLog(const char *fmt,...);
    void LoadProductsToComboBox();

public slots:
    void SetRunStatus(bool bRun);
    void LogMessage(const QString& msg);

private slots:
    void on_btnOpenFile_clicked();

    void on_btnRun_clicked();


    void onProductComboBoxIndexChangeSlot(int index);


    void on_action_about_triggered();

    void on_action_MD5Func_triggered();

    void on_action_OtherFuncs_triggered();

private:
    Ui::MainWindow *ui;
    MD5_Dialog *m_pDlgMD5;

    bool m_bRun;
    CWorker *m_workThread;

    std::vector<CProductInfo*> m_vProducts;

};
#endif // MAINWINDOW_H
