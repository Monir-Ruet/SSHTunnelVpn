#include "ssh.h"

SSH* SSH::m_instance = nullptr;  // Static definition of m_instance
QMutex SSH::mutex;               // Static definition of mutex

SSH::SSH(QObject* parent)
    : QObject{parent} {
    m_instance = nullptr;
}

SSH::~SSH(){
    if (m_session != nullptr) {
        ssh_free(m_session);
    }
}

SSH* SSH::instance(){
    QMutexLocker locker(&mutex);

    if(m_instance == nullptr){
        m_instance = new SSH();
    }

    return m_instance;
}

ssh_session SSH::getSession() const {
    return m_session;
}

int SSH::connectToServer() {
    int rc;

    if (m_session != nullptr) {
        return 1; // Already connected
    }

    m_session = ssh_new();
    if (m_session == nullptr) {
        // std::cerr << "Error creating SSH m_session." << std::endl;
        return -1;
    }

    // Set SSH options
    int verbosity = SSH_LOG_PROTOCOL;  // Equivalent to -v
    int port = 22;                     // Equivalent to -p 22
    ssh_options_set(m_session, SSH_OPTIONS_HOST, "bd2.vpnjantit.com");
    // ssh_options_set(m_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    ssh_options_set(m_session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(m_session, SSH_OPTIONS_USER, "monir1231-vpnjantit.com");
    ssh_options_set(m_session, SSH_OPTIONS_STRICTHOSTKEYCHECK, "no");
    ssh_options_set(m_session, SSH_OPTIONS_KNOWNHOSTS, "/dev/null");

    // Connect to the server
    rc = ssh_connect(m_session);
    if (rc != SSH_OK) {
        fprintf(stderr, "Error connecting to remote server: %s\n", ssh_get_error(m_session));
        ssh_free(m_session);
        return -1;
    }

    rc = ssh_userauth_password(m_session, NULL, "12345");
    if (rc != SSH_AUTH_SUCCESS) {
        fprintf(stderr, "Error authenticating with password: %s\n", ssh_get_error(m_session));
        ssh_free(m_session);
        return -1;
    }
    // http_get = "GET / HTTP/1.1\r\nHost: bd2.vpnjantit.com\r\nUser-Agent: curl/8.7.1\r\nAccept: */*\r\n\r\n";
    return 1;
}
