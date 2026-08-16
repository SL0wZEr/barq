#pragma once

#include <QFileSystemWatcher>
#include <QObject>
#include <QString>
#include <QTimer>

class QWindow;

// The resident state of barq: the note buffer's debounced auto-save, the
// Omarchy theme palette, and show/hide of the one window.
class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool darkMode READ darkMode NOTIFY darkModeChanged)
    Q_PROPERTY(qreal textScale READ textScale NOTIFY textScaleChanged)
    Q_PROPERTY(QString themeBackground READ themeBackground NOTIFY themeColorsChanged)
    Q_PROPERTY(QString themeForeground READ themeForeground NOTIFY themeColorsChanged)
    Q_PROPERTY(QString themeAccent READ themeAccent NOTIFY themeColorsChanged)
    Q_PROPERTY(QString themeSelection READ themeSelection NOTIFY themeColorsChanged)
    Q_PROPERTY(bool dirty READ dirty NOTIFY dirtyChanged)

public:
    explicit Backend(QObject *parent = nullptr);

    void setWindow(QWindow *window);

    bool darkMode() const { return m_darkMode; }
    qreal textScale() const { return m_textScale; }
    QString themeBackground() const { return m_themeBackground; }
    QString themeForeground() const { return m_themeForeground; }
    QString themeAccent() const { return m_themeAccent; }
    QString themeSelection() const { return m_themeSelection; }
    bool dirty() const { return m_dirty; }

    Q_INVOKABLE QString loadNote() const;
    Q_INVOKABLE void noteEdited(const QString &text);
    Q_INVOKABLE void hide();
    Q_INVOKABLE void quit();

public slots:
    void toggle();
    void setDarkMode(bool darkMode);
    void setTextScale(qreal textScale);
    void flush();

signals:
    void darkModeChanged();
    void textScaleChanged();
    void themeColorsChanged();
    void dirtyChanged();
    void summoned();

private:
    void show();
    void saveNow();
    void setDirty(bool dirty);
    void loadOmarchyTheme();
    void watchOmarchyTheme();
    QString notePath() const;

    QWindow *m_window = nullptr;
    QString m_pendingText;
    bool m_havePending = false;
    bool m_dirty = false;
    bool m_darkMode = true;
    qreal m_textScale = 1.0;
    QString m_themeBackground;
    QString m_themeForeground;
    QString m_themeAccent;
    QString m_themeSelection;
    QTimer m_saveTimer;
    QFileSystemWatcher m_themeWatcher;
};
