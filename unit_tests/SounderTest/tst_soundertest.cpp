#include <QtTest>
#include "Sounder.h"
#include "QDir"

class SounderTest : public QObject
{
    Q_OBJECT

public:
    SounderTest();
    ~SounderTest();

private slots:
    void test_case1();

};

SounderTest::SounderTest()
{

}

SounderTest::~SounderTest()
{

}

void SounderTest::test_case1()
{
  Sounder sounder;
  QStringList testList;
  sounder.getSoundsList(testList);
  qDebug()<<"Sounds list count: "<<testList.count();
  sounder.playSound("welcome.mp3");
}

QTEST_APPLESS_MAIN(SounderTest)

#include "tst_soundertest.moc"
