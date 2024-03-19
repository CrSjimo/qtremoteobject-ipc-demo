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
    qDebug() << roKey << aliveKey;

    QRemoteObjectNode repNode;
    repNode.connectToNode(QUrl("local:" + roKey));

    std::unique_ptr<DemoReplica> rep_(repNode.acquire<DemoReplica>());

    auto rep = rep_.get();

    QObject::connect(callButton, &QAbstractButton::clicked, [=] {
        rep->add(arg1SpinBox->value(), arg2SpinBox->value());
        callButton->setDisabled(true);
    });

    QObject::connect(rep, &DemoReplica::progressChanged, progressBar, &QProgressBar::setValue);
    QObject::connect(rep, &DemoReplica::completed, [=](int value) {
        resultLabel->setText("Result: " + QString::number(value));
        progressBar->setValue(0);
        callButton->setDisabled(false);
    });

    QObject::connect(rep, &QRemoteObjectReplica::stateChanged, [=](QRemoteObjectReplica::State state) {
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