#ifndef QTREMOTEOBJECTSIPCDEMO_DEMO_H
#define QTREMOTEOBJECTSIPCDEMO_DEMO_H

#include <rep_demo_source.h>

class Demo : public DemoSource {
public:
    explicit Demo(QObject *parent = nullptr);
    ~Demo() override;

    int add(int a, int b) override;
};


#endif //QTREMOTEOBJECTSIPCDEMO_DEMO_H
