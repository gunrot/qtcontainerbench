#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QMap>
#include <algorithm>
#include <utility>
#include <map>
#include <unordered_map>
#include <qfasthash_p.h>
#include <stdint.h>

typedef int32_t tTestKey;
typedef int32_t tTestValue;
//typedef  std::pair<int,int> tVecData;
struct tVecData
{
    tVecData(tTestKey f,tTestValue s)
        :first(f)
        ,second(s)
    {}
    tVecData()
        :first(0)
        ,second(0)
    {}
    tTestKey first;
    tTestValue second;
};

class Map_hast_Test : public QObject
{
    Q_OBJECT

public:
    Map_hast_Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_insert_data();
    void testCase_insert();
    void testCase_find();
    void testCase_find_data();

};

inline bool operator<(const tVecData &t1, const tVecData &t2)
{
    return t1.first < t2.first;
}
Map_hast_Test::Map_hast_Test()
{
}

void Map_hast_Test::initTestCase()
{

}

void Map_hast_Test::cleanupTestCase()
{
}
static int testcounts[] = {10,20,50,80,100,1000,10000,100000,1000000};

void Map_hast_Test::testCase_insert_data()
{
    QTest::addColumn<QString>("testcontainer");
    QTest::addColumn<int>("testcount");
    QString testContainer;
    static const char * tests[] = {
      "QMap_insert",
        "QHash_insert",
        "stdmap_insert"
     };


    for(int count : testcounts)
    {
        for (unsigned int t = 0; t < sizeof(tests)/sizeof(tests[0]);++t)
        {

            QString text;
            text += QLatin1String(tests[t]);
            text = text.leftJustified(20, ' ', true);
            text +=  QLatin1String(" -- ");
            text += QString::number(count).leftJustified(12, ' ');


            QTest::newRow(text.toLatin1().constData()) << QString(tests[t]) << count;
        }

    }
}
void Map_hast_Test::testCase_find_data()
{
    QTest::addColumn<QString>("testcontainer");
    QTest::addColumn<int>("testcount");
    QString testContainer;
    static const char * tests[] = {
      "QMap_find",
        "QMap_constFind",
        "QHash_find",
        "QHash_constFind",
        "QVector_lowerbound",
        "stdmap_find",
        "stdunordered_find",
        "qfasthash_find"
     };

    for(int count : testcounts)
    {
        for (unsigned int t = 0; t < sizeof(tests)/sizeof(tests[0]);++t)
        {

            QString text;
            text += QLatin1String(tests[t]);
            text = text.leftJustified(20, ' ', true);
            text +=  QLatin1String(" -- ");
            text += QString::number(count).leftJustified(12, ' ');


            QTest::newRow(text.toLatin1().constData()) << QString(tests[t]) << count;
        }

    }
}

inline void insertdata(QMap<tTestKey,tTestValue> & m,int testcount)
{
    for(int i = testcount ;i> 0 ;--i)
    {
        m.insert(i,i);
    }

}
inline void insertdata(QHash<tTestKey,tTestValue> & m,int testcount)
{
    for(int i = testcount ;i> 0 ;--i)
    {
        m.insert(i,i);
    }

}
inline void insertdata(std::map<tTestKey,tTestValue> & m,int testcount)
{
    for(int i = testcount ;i> 0 ;--i)
    {
        m[i]=i;
    }
}


void Map_hast_Test::testCase_insert()
{
    QFETCH(QString,testcontainer);
    QFETCH(int,testcount);

    if(testcontainer == QLatin1String("QMap_insert"))
    {
        QMap<tTestKey,tTestValue> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = m.find(testcount);
        if(*it != testcount) QFAIL( "fail");
    }
    else if (testcontainer == QLatin1String("QHash_insert"))
    {
        QHash<tTestKey,tTestValue> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = m.find(testcount);
        if(*it != testcount) QFAIL( "fail");

    }
    else if (testcontainer == QLatin1String("stdmap_insert"))
    {
        std::map<tTestKey,tTestValue> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = m.find(testcount);
        if(it->first != testcount) QFAIL( "fail");

    }
}

void Map_hast_Test::testCase_find()
{
    QFETCH(QString,testcontainer);
    QFETCH(int,testcount);
    if(testcontainer == QLatin1String("QMap_find"))
    {
        QMap<tTestKey,tTestValue> m_mapTest;
        insertdata(m_mapTest,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                auto it = m_mapTest.find(i);
                if(*it != i) QFAIL( "fail");
            }
        }
    }
    else if (testcontainer == QLatin1String("QMap_constFind"))
    {
        QMap<tTestKey,tTestValue> m_mapTest;
        insertdata(m_mapTest,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                auto it = m_mapTest.constFind(i);
                if(*it != i) QFAIL( "fail");
            }
        }

    }
    else if (testcontainer == QLatin1String("QHash_find"))
    {
        QHash<tTestKey,tTestValue> m_hashTest;
        insertdata(m_hashTest,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                auto it = m_hashTest.find(i);
                if(*it != i) QFAIL( "fail");
            }
        }

    }
    else if (testcontainer == QLatin1String("QHash_constFind"))
    {
        QHash<tTestKey,tTestValue> m_hashTest;
        insertdata(m_hashTest,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                auto it = m_hashTest.constFind(i);
                if(*it != i) QFAIL( "fail");
            }
        }

    }
    else if (testcontainer == QLatin1String("QVector_lowerbound"))
    {
       QVector<tVecData> m_vecTest;
        for(int i = 1 ;i<= testcount ;++i)
        {
            m_vecTest.push_back(tVecData(i,i));
        }
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                auto it = std::lower_bound(std::begin(m_vecTest),std::end(m_vecTest),tVecData(i,0));
                if(it->first != i) QFAIL( "fail");
            }
        }

    }
    else if (testcontainer == QLatin1String("stdmap_find"))
    {
        std::map<tTestKey,tTestValue> m_stdmapTest;
        for(int i = testcount ;i> 0 ;--i)
        {
            m_stdmapTest[i]=i;
        }
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                auto it = m_stdmapTest.find(i);
                if(it->first != i) QFAIL( "fail");
            }
        }

    }
    else if (testcontainer == QLatin1String("stdunordered_find"))
    {
        std::unordered_map <tTestKey,tTestValue> m_stdunorderedTest;
        for(int i = testcount ;i> 0 ;--i)
        {
            m_stdunorderedTest[i]=i;
        }
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                auto it = m_stdunorderedTest.find(i);
                if(it->first != i) QFAIL( "fail");
            }
        }

    }
    else if (testcontainer == QLatin1String("qfasthash_find"))
    {
        QFastHash<tTestKey,tTestValue> m_fasthash_Test;
        for(int i = testcount ;i> 0 ;--i)
        {
            m_fasthash_Test[i]=i;
        }
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                auto it = m_fasthash_Test.constFind(i);
                if(it != i) QFAIL( "fail");
            }
        }

    }
    else
    {
        QFAIL( "fail, unknown test ");
    }

}

QTEST_MAIN(Map_hast_Test)

#include "tst_map_hast_test.moc"
