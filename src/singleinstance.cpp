#include "singleinstance.h"

#include <QDebug>
#include <QLocalSocket>

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
