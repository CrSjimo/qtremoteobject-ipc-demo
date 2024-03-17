#include "Demo.h"

#include <QTimer>
#include <QEventLoop>

Demo::Demo(QObject *parent) : DemoSource(parent) {
}

Demo::~Demo() = default;

int Demo::add(int a, int b) {
    qDebug() << "input:" << a << b;
    QTimer t;
    t.setInterval(20);
    int i = 0;
    QEventLoop eventLoop;
    connect(&t, &QTimer::timeout, [&] {
        emit progressChanged(i++);
        if (i == 100)
            eventLoop.quit();
    });
    t.start();
    eventLoop.exec();
    qDebug() << "output:" << a + b;
    return a + b;
}
