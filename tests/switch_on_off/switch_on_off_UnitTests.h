#ifndef UNITTESTS_H
#define UNITTESTS_H

#include <QObject>
#include <QtTest>

class switch_on_off_UnitTests : public QObject
{
    Q_OBJECT

public:
    switch_on_off_UnitTests();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void switch_on_off_test();

};

#endif // UNITTESTS_H
