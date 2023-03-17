#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Worker.h"

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

public slots:
    void SetRunStatus(bool bRun);
    void LogMessage(const QString& msg);

private slots:
    void on_btnOpenFile_clicked();

    void on_btnRun_clicked();

private:
    Ui::MainWindow *ui;

    CWorker *m_workThread;
    bool m_bRun;
};
#endif // MAINWINDOW_H
