#include <QCoreApplication>
#include <QLocalSocket>
#include "Demo.h"

int main(int argc, char **argv) {
    QCoreApplication a(argc, argv);

    auto roKey = a.arguments()[1];
    auto aliveKey = a.arguments()[2];

    QLocalSocket socket;
    socket.connectToServer(aliveKey);
    socket.waitForConnected(1000);
    if (socket.state() != QLocalSocket::ConnectedState) {
        qDebug() << "Cannot connect to alive socket";
        exit(1);
    }
    QObject::connect(&socket, &QLocalSocket::disconnected, []() {
        QCoreApplication::exit(1);
    });

    Demo demo;
    QRemoteObjectHost host(QUrl("local:" + roKey));
    host.enableRemoting(&demo);

    return a.exec();
}