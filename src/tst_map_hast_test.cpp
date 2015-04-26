#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QMap>
#include <algorithm>
#include <utility>
#include <map>
#include <unordered_map>
#include <qfasthash_p.h>


//typedef  std::pair<int,int> tVecData;
struct tVecData
{
    tVecData(int f,int s)
        :first(f)
        ,second(s)
    {}
    tVecData() :tVecData(0,0) {}
    int first;
    int second;
};

class Map_hast_Test : public QObject
{
    Q_OBJECT

public:
    Map_hast_Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

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

    for(int count : {10,100,1000/*,10000,100000*/})
    {
        for (unsigned int t = 0; t < sizeof(tests)/sizeof(tests[0]);++t)
        {

            QString text;
            text += QLatin1String(tests[t]);
            text +=  QLatin1String(" -- ");
            text += QString::number(count);


            QTest::newRow(text.toLatin1().constData()) << QString(tests[t]) << count;
        }

    }
}
void Map_hast_Test::testCase_find()
{
    QFETCH(QString,testcontainer);
    QFETCH(int,testcount);
    if(testcontainer == QLatin1String("QMap_find"))
    {
        QMap<int,int> m_mapTest;
        for(int i = testcount ;i> 0 ;--i)
        {
            m_mapTest.insert(i,i);
        }
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
        QMap<int,int> m_mapTest;
        for(int i = testcount ;i> 0 ;--i)
        {
            m_mapTest.insert(i,i);
        }
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
        QHash<int,int> m_hashTest;
        for(int i = testcount ;i> 0 ;--i)
        {
            m_hashTest.insert(i,i);
        }
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
        QHash<int,int> m_hashTest;
        for(int i = testcount ;i> 0 ;--i)
        {
            m_hashTest.insert(i,i);
        }
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
        std::map<int,int> m_stdmapTest;
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
        std::unordered_map <int,int> m_stdunorderedTest;
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
        QFastHash<int,int> m_fasthash_Test;
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
