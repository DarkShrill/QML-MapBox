#include <QGuiApplication>
#include <QApplication>
#include "QCoreApplication"
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include "networkmanager.h"
#include "locationsource.h"
#include <QVariant>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc,argv);

    QQmlApplicationEngine engine;

    NetworkManager networkManager;
    LocationSource locationSource(networkManager);
    engine.rootContext()->setContextProperty("locationSource", &locationSource);


    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
