#pragma once

#include <QLocalServer>
#include <QObject>
#include <QString>

// One barq per session: the first launch owns a local socket; every later
// launch just pokes it and exits. That poke is what makes the summon
// keybinding instant — the resident instance only has to show its window.
class SingleInstance : public QObject {
    Q_OBJECT

public:
    explicit SingleInstance(const QString &name, QObject *parent = nullptr);

    // True when this process is the resident instance; false when a running
    // instance was poked instead and this process should exit.
    bool claim();

signals:
    void toggleRequested();

private:
    void handleConnection();

    QString m_name;
    QLocalServer m_server;
};
