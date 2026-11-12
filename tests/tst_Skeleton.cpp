#include <QtTest>

class Skeleton : public QObject
{
    Q_OBJECT

private slots:
    void runs() { QVERIFY(true); }
};

QTEST_MAIN(Skeleton)
#include "tst_Skeleton.moc"