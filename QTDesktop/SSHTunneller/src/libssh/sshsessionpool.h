#ifndef SSHSESSIONPOOL_H
#define SSHSESSIONPOOL_H

#include <QObject>
#include <QMutex>
#include <QQueue>
#include <QMutexLocker>
#include <QDebug>
#include <libssh/libssh.h>

class SSHSessionPool : public QObject
{
    Q_OBJECT
public:
    explicit SSHSessionPool(QObject *parent = nullptr);
    ~SSHSessionPool();

    ssh_session* acquireSession();
    void releaseSession(ssh_session* session);
    void setMaxSessions(int maxSessions);

signals:
private:
    ssh_session* createSession();
    QQueue<ssh_session*> sessionPool;
    QMutex mutex;
    int maxSessions;
    int activeSessions;
};

#endif // SSHSESSIONPOOL_H
