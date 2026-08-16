#include "singleinstance.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QLocalSocket>

#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

bool SingleInstance::pokeExisting(const QString &name) {
    // QLocalServer places named sockets in the temp dir on Linux.
    const QByteArray path =
        QFile::encodeName(QDir::tempPath() + QLatin1Char('/') + name);

    sockaddr_un addr {};
    addr.sun_family = AF_UNIX;
    if (path.size() + 1 > static_cast<int>(sizeof(addr.sun_path)))
        return false;
    std::memcpy(addr.sun_path, path.constData(), path.size() + 1);

    const int fd = ::socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (fd < 0)
        return false;

    const bool answered =
        ::connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == 0;
    if (answered) {
        const char toggle[] = "toggle\n";
        (void)::write(fd, toggle, sizeof(toggle) - 1);
    }
    ::close(fd);
    return answered;
}

SingleInstance::SingleInstance(const QString &name, QObject *parent)
    : QObject(parent), m_name(name) {
    connect(&m_server, &QLocalServer::newConnection, this,
            &SingleInstance::handleConnection);
}

bool SingleInstance::claim() {
    QLocalSocket probe;
    probe.connectToServer(m_name);
    if (probe.waitForConnected(300)) {
        probe.write("toggle\n");
        probe.flush();
        probe.waitForBytesWritten(300);
        probe.disconnectFromServer();
        return false;
    }

    // Nobody answered; clear any socket a crashed instance left behind.
    QLocalServer::removeServer(m_name);
    if (!m_server.listen(m_name))
        qWarning() << "barq: could not claim instance socket" << m_name
                   << "- running without toggle support";
    return true;
}

void SingleInstance::handleConnection() {
    while (QLocalSocket *client = m_server.nextPendingConnection()) {
        connect(client, &QLocalSocket::disconnected, client, &QObject::deleteLater);
        client->disconnectFromServer();
        emit toggleRequested();
    }
}
