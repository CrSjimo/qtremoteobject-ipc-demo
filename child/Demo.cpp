#include "Demo.h"

#include <QtConcurrent/QtConcurrent>

Demo::Demo(QObject *parent) : DemoSource(parent) {
}

Demo::~Demo() = default;

void Demo::add(int a, int b) {
    qDebug() << "input:" << a << b;
    auto ppPromise = new QFutureInterface<int> *(nullptr);
    auto ret = QtConcurrent::run([=] {
        while(!*ppPromise);
        for (int i = 0; i < 100; i++) {
            (*ppPromise)->setProgressValue(i);
            QThread::msleep(20);
        }
        qDebug() << "output:" << a + b;
        delete *ppPromise;
        delete ppPromise;
        return a + b;
    });
    *ppPromise = new QFutureInterface<int>(ret.d);
    auto watcher = new QFutureWatcher<int>;
    connect(watcher, &QFutureWatcher<int>::progressValueChanged, this, &Demo::progressChanged);
    connect(watcher, &QFutureWatcher<int>::finished, this, [=] {
        emit completed(watcher->result());
        watcher->deleteLater();
    });
    watcher->setFuture(ret);
}
