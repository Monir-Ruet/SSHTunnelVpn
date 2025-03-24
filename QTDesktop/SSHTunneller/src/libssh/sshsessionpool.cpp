#include "sshsessionpool.h"

SSHSessionPool::SSHSessionPool(QObject *parent)
    : QObject{parent}, maxSessions(10), activeSessions(0)
{}

SSHSessionPool::~SSHSessionPool()
{
    while (!sessionPool.isEmpty()) {
        ssh_session* session = sessionPool.dequeue();
        delete session;
    }
}

ssh_session *SSHSessionPool::acquireSession()
{
    QMutexLocker locker(&mutex);

    if (!sessionPool.isEmpty()) {
        // Reuse an existing session from the pool
        return sessionPool.dequeue();
    }

    if (activeSessions < maxSessions) {
        // Create a new session if the pool is not full
        activeSessions++;
        return createSession();
    }

    // No available sessions and pool is full
    qDebug() << "No available sessions in the pool.";
}

void SSHSessionPool::releaseSession(ssh_session *session)
{
    QMutexLocker locker(&mutex);

    if (session) {
        // Reset the session state if needed
        if (ssh_is_connected(*session)) {
            // Optionally, reset the session (e.g., close channels, reset state)
        }
        sessionPool.enqueue(session);
    }
}

void SSHSessionPool::setMaxSessions(int maxSessions)
{
    QMutexLocker locker(&mutex);
    this->maxSessions = maxSessions;
}

ssh_session *SSHSessionPool::createSession()
{
    ssh_session m_session = ssh_new();
    if (m_session == nullptr) {
        // std::cerr << "Error creating SSH m_session." << std::endl;
        return nullptr;
    }

    // Set SSH options
    int verbosity = SSH_LOG_PROTOCOL;  // Equivalent to -v
    int port = 22;                     // Equivalent to -p 22
    ssh_options_set(m_session, SSH_OPTIONS_HOST, "bd2.vpnjantit.com");
    ssh_options_set(m_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    ssh_options_set(m_session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(m_session, SSH_OPTIONS_USER, "monir1231-vpnjantit.com");
    ssh_options_set(m_session, SSH_OPTIONS_STRICTHOSTKEYCHECK, "no");
    ssh_options_set(m_session, SSH_OPTIONS_KNOWNHOSTS, "/dev/null");

    // Connect to the server
    int rc = ssh_connect(m_session);
    if (rc != SSH_OK) {
        fprintf(stderr, "Error connecting to remote server: %s\n", ssh_get_error(m_session));
        ssh_free(m_session);
        return nullptr;
    }

    rc = ssh_userauth_password(m_session, NULL, "12345");
    if (rc != SSH_AUTH_SUCCESS) {
        fprintf(stderr, "Error authenticating with password: %s\n", ssh_get_error(m_session));
        ssh_free(m_session);
        return nullptr;
    }
}
