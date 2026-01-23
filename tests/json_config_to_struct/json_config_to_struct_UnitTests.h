#ifndef UNITTESTS_H
#define UNITTESTS_H

#include <QObject>
#include <QtTest>

class json_config_to_struct_UnitTests : public QObject {
    Q_OBJECT

public:
    json_config_to_struct_UnitTests();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void json_to_struct_test();
};

#endif  // UNITTESTS_H
