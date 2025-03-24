#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QVBoxLayout>
#include "credential.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void addLogging(QString log);

private slots:
    void on_backButton_clicked();

    void on_settingsButton_clicked();

    void on_saveButton_clicked();

    void on_payloadInput_textChanged();

    void on_hostInput_textChanged();

    void on_connectButton_clicked();

    void on_usernameInput_textEdited(const QString &arg1);

    void on_passwordInput_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    Credential *credential, *temp;
    QThread *workerSocksProxyThread;
};
#endif // MAINWINDOW_H
