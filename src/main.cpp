#include <QFontDatabase>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlError>
#include <QQuickStyle>
#include <QUrl>
#include <QWindow>

#include <unistd.h>

#include "backend.h"
#include "singleinstance.h"
#include "systemtheme.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("barq"));
    app.setDesktopFileName(QStringLiteral("barq"));
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("barq")));

    // Local sockets live in a world-shared temp dir, so scope the name per user.
    SingleInstance instance(QStringLiteral("barq-") + QString::number(::getuid()));
    if (!instance.claim())
        return 0; // the resident barq was toggled instead

    QQuickStyle::setStyle(QStringLiteral("Material"));

    Backend backend(&app);
    SystemTheme systemTheme(&app);
    backend.setDarkMode(systemTheme.darkMode());
    QObject::connect(&systemTheme, &SystemTheme::darkModeChanged, &backend,
                     &Backend::setDarkMode);
    backend.setTextScale(systemTheme.textScale());
    QObject::connect(&systemTheme, &SystemTheme::textScaleChanged, &backend,
                     &Backend::setTextScale);
    QObject::connect(&instance, &SingleInstance::toggleRequested, &backend,
                     &Backend::toggle);
    QObject::connect(&app, &QCoreApplication::aboutToQuit, &backend, &Backend::flush);

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::warnings, &app,
                     [](const QList<QQmlError> &warnings) {
        for (const QQmlError &warning : warnings)
            qWarning().noquote() << warning.toString();
    });
    engine.rootContext()->setContextProperty(QStringLiteral("backend"), &backend);
    engine.rootContext()->setContextProperty(
        QStringLiteral("fixedFontFamily"),
        QFontDatabase::systemFont(QFontDatabase::FixedFont).family());

    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    if (engine.rootObjects().isEmpty()) {
        qCritical() << "barq: could not load the interface";
        return -1;
    }

    backend.setWindow(qobject_cast<QWindow *>(engine.rootObjects().constFirst()));

    return app.exec();
}
