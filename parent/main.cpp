#include <QApplication>
#include <QMainWindow>
#include <QBoxLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <rep_demo_replica.h>
#include <QProcess>
#include <QLocalServer>
#include <QUuid>

int main(int argc, char **argv) {
    QApplication a(argc, argv);

    QMainWindow win;
    auto mainWidget = new QWidget;
    auto mainLayout = new QVBoxLayout;
    auto argsLayout = new QHBoxLayout;
    auto arg1SpinBox = new QSpinBox;
    auto arg2SpinBox = new QSpinBox;
    argsLayout->addWidget(arg1SpinBox);
    argsLayout->addWidget(arg2SpinBox);
    mainLayout->addLayout(argsLayout);
    auto callButton = new QPushButton("Call");
    mainLayout->addWidget(callButton);
    auto resultLabel = new QLabel;
    mainLayout->addWidget(resultLabel);
    auto progressBar = new QProgressBar;
    mainLayout->addWidget(progressBar);
    auto stateLabel = new QLabel;
    mainLayout->addWidget(stateLabel);
    auto crashButton = new QPushButton("Crash");
    mainLayout->addWidget(crashButton);

    mainWidget->setLayout(mainLayout);
    win.setCentralWidget(mainWidget);

    auto roKey = QUuid::createUuid().toString(QUuid::Id128);
    auto aliveKey = QUuid::createUuid().toString(QUuid::Id128);

    QRemoteObjectNode repNode;
    repNode.connectToNode(QUrl("local:" + roKey));

    std::unique_ptr<DemoReplica> rep(repNode.acquire<DemoReplica>());

    QObject::connect(callButton, &QAbstractButton::clicked, [&] {
        auto ret = rep->add(arg1SpinBox->value(), arg2SpinBox->value());
        auto watcher = new QRemoteObjectPendingCallWatcher(ret);
        callButton->setDisabled(true);
        QObject::connect(watcher, &QRemoteObjectPendingCallWatcher::finished, [=](QRemoteObjectPendingCallWatcher *self) {
            callButton->setDisabled(false);
            if (self->error() == QRemoteObjectPendingCall::InvalidMessage)
                resultLabel->setText("Result: ERROR");
            else
                resultLabel->setText("Result: " + QString::number(self->returnValue().toInt()));
            progressBar->setValue(0);
            self->deleteLater();
        });
    });

    QObject::connect(rep.get(), &DemoReplica::progressChanged, progressBar, &QProgressBar::setValue);

    QObject::connect(rep.get(), &QRemoteObjectReplica::stateChanged, [=](QRemoteObjectReplica::State state) {
        auto e = QMetaEnum::fromType<QRemoteObjectReplica::State>();
        stateLabel->setText(QString("State: ") + e.valueToKey(state));
    });

    QObject::connect(crashButton, &QPushButton::clicked, [] {
        QObject * volatile o = nullptr;
        o->setParent(nullptr);
    });

    QLocalServer server;
    server.listen(aliveKey);

    QProcess p;
    p.setProgram("../child/child.exe");
    p.setArguments({roKey, aliveKey});
    p.setProcessChannelMode(QProcess::ForwardedChannels);
    p.start();

    win.show();

    return a.exec();
}