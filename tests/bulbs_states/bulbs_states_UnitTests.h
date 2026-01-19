#ifndef UNITTESTS_H
#define UNITTESTS_H

#include <QObject>
#include <QtTest>

class bulbs_states_UnitTests : public QObject
{
    Q_OBJECT

public:
    bulbs_states_UnitTests();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void bulbs_items_test();

};

#endif // UNITTESTS_H
