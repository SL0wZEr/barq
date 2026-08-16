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

    // Raw-socket poke of a resident instance, safe to call before any
    // Q*Application exists. Keeping Qt's GUI stack out of the poke path is
    // what makes the summon keybinding fast. True if an instance answered
    // (and this process should exit).
    static bool pokeExisting(const QString &name);

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
