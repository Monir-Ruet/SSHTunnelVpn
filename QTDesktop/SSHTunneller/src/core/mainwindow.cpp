#include "mainwindow.h"
#include "socksproxy.h"
#include "ui_mainwindow.h"

#include <settings.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QCoreApplication::setOrganizationName("ProjectX");
    QCoreApplication::setApplicationName("SSHTunnel");

    workerSocksProxyThread = new QThread(this);
}

MainWindow::~MainWindow()
{
    workerSocksProxyThread->quit();
    workerSocksProxyThread->wait();
    delete ui;
}

void MainWindow::on_backButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->homepage);
    delete temp;
}

void MainWindow::on_settingsButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->settings);
    temp = new Credential();
}

void MainWindow::on_saveButton_clicked() {
    credential = temp;
    qDebug() << credential;

    temp = nullptr;
    Settings::instance()->saveCredentials(credential);
    ui->stackedWidget->setCurrentWidget(ui->homepage);
}

void MainWindow::on_payloadInput_textChanged() {
    temp->payload = ui->payloadInput->toPlainText();
}

void MainWindow::on_hostInput_textChanged() {
    QString hostWithPort = ui->hostInput->text();
    QStringList str = hostWithPort.split(":");
    if (str.length() != 2) return;
    bool flag;
    int tempPort = str[1].toInt(&flag);
    if(!flag) return;

    temp->host = str[0];
    temp->port = tempPort;
}

void MainWindow::on_connectButton_clicked() {
    // if (server.isListening()) {
    //     ui->connectButton->setText("Connect");
    //     ui->connectButton->setStyleSheet(
    //         QString::fromUtf8(
    //             "border-radius: 50%;\n"
    //             "border: 10px solid blue;\n"));
    //     addLogging("Server stopped");
    //     return;
    // }
    addLogging("Server Connected");
    ui->connectButton->setText("Connected");
    ui->connectButton->setStyleSheet(
        QString::fromUtf8(
            "border-radius: 50%;\n"
            "border: 10px solid green;\n"
        ));

    // Server * server = new Server();
    SocksProxy *proxy = new SocksProxy;
    // server->moveToThread(workerSocksProxyThread);
    proxy->moveToThread(workerSocksProxyThread);
    connect(workerSocksProxyThread, &QThread::started, proxy, &SocksProxy::open_connection);
    // connect(workerSocksProxyThread, &QThread::started, server, &Server::setupServer);
    connect(workerSocksProxyThread, &QThread::finished, workerSocksProxyThread, &QObject::deleteLater);
    workerSocksProxyThread->start();
}

void MainWindow::addLogging(QString log) {
    // Create a custom widget for text + horizontal line
    QWidget *customWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(customWidget);

    // Add a label for the text
    QLabel *label = new QLabel(log);
    QFont font = label->font();
    font.setPointSize(10);
    label->setFont(font);
    layout->addWidget(label);

    // Add a horizontal line
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);  // Horizontal line
    layout->addWidget(line);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    // Create a QListWidgetItem and set the custom widget
    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(customWidget->sizeHint());  // Set the size hint

    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, customWidget);
}

void MainWindow::on_usernameInput_textEdited(const QString &arg1)
{
    temp->username = ui->usernameInput->text();
}

void MainWindow::on_passwordInput_textChanged(const QString &arg1)
{
    temp->password = ui->passwordInput->text();
}
