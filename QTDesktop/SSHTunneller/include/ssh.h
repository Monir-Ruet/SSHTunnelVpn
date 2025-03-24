#ifndef SSH_H
#define SSH_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMutex>
#include <libssh/libssh.h>

class SSH : public QObject {
    Q_OBJECT
public:
    explicit SSH(QObject* parent = nullptr);
    ~SSH();
    int connectToServer();
    static SSH* instance();
    ssh_session getSession() const;

private:
    SSH(const SSH&) = delete;
    SSH& operator=(const SSH&) = delete;

    static SSH* m_instance;
    ssh_session m_session;
    static QMutex mutex;
};

#endif  // SSH_H
