#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QMap>
#include <algorithm>
#include <utility>
#include <map>
#include <unordered_map>
#include "qfasthash_p.h"
#include <stdint.h>
#ifdef USE_BOOST
#include <boost/unordered_map.hpp>
#endif

#ifdef TEST_KEY_STRING

namespace std{
    /* std::hash specialization for QString so it can be used
     * as a key in std::unordered_map */

    template<> struct hash<QString> {
        typedef QString Key;
        typedef uint result_type;
        inline uint operator()(const QString &s) const { return qHash(s); }
    };
}

std::size_t hash_value(const QString &s)
    {
        boost::hash<int> hasher;
        return hasher(qHash(s));
    }

static std::vector<QString>  s_vec;
void createStringNumberArray(size_t n)
{

    if(n  >= s_vec.size())
    {
         for (auto i=s_vec.size(); i <= n; ++i)
            s_vec.push_back(QString::number(i)+"                               ");
    }
}

typedef QString tTestKey;
#define MAKE_KEY(x) s_vec[x]
//#define MAKE_KEY(x) QString::number(x)
#else
typedef int32_t tTestKey;
#define MAKE_KEY(x) x
#endif

typedef int32_t tTestValue;

struct tVecData
{
    tVecData(tTestKey f,tTestValue s)
        :first(f)
        ,second(s)
    {}
    tVecData()
        :
#ifndef TEST_KEY_STRING
        first(0),
#endif
        second(0)
    {}
    bool operator<( const tVecData &t2) const
    {
        return first < t2.first;
    }
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


Map_hast_Test::Map_hast_Test()
{
}

void Map_hast_Test::initTestCase()
{

}

void Map_hast_Test::cleanupTestCase()
{
}
static int testcounts[] = {100};//{5,7,10,12,15,17,20,25,29,34,41,47, 50,75,80,90,100};//,1000,10000,100000,1000000};

void Map_hast_Test::testCase_insert_data()
{
    QTest::addColumn<QString>("testcontainer");
    QTest::addColumn<int>("testcount");
    QString testContainer;

    static const char * tests[] = {
      "QMap_insert",
        "QHash_insert",
        "stdmap_insert",
        "stdvector_pb_sort",
        "stdunordered_insert",
        "qfasthash_insert",
#ifdef USE_BOOST
        "boostunordered_insert"
#endif
     };


    for (unsigned int t = 0; t < sizeof(tests)/sizeof(tests[0]);++t)
    {
        for(int count : testcounts)
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
        "stdvector_lowerbound",
        "stdmap_find",
        "stdunordered_find",
        "qfasthash_find",
#ifdef USE_BOOST
        "boostunordered_find"
#endif
     };

     for (unsigned int t = 0; t < sizeof(tests)/sizeof(tests[0]);++t)
     {

        for(int count : testcounts)
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
        m.insert(MAKE_KEY(i),i);
    }

}
inline void insertdata(QHash<tTestKey,tTestValue> & m,int testcount)
{
    for(int i = testcount ;i> 0 ;--i)
    {
        m.insert(MAKE_KEY(i),i);
    }

}
inline void insertdata(std::map<tTestKey,tTestValue> & m,int testcount)
{
    for(int i = testcount ;i> 0 ;--i)
    {
        m[MAKE_KEY(i)]=i;
    }
}

inline void insertdata(std::unordered_map <tTestKey,tTestValue> & m,int testcount)
{
    for(int i = testcount ;i> 0 ;--i)
    {
        m[MAKE_KEY(i)]=i;
    }
}
#ifdef USE_BOOST
inline void insertdata(boost::unordered_map <tTestKey,tTestValue> & m,int testcount)
{
    for(int i = testcount ;i> 0 ;--i)
    {
        m[MAKE_KEY(i)]=i;
    }
}
#endif

inline void insertdata(QFastHash<tTestKey,tTestValue> & m,int testcount)
{
    for(int i = testcount ;i> 0 ;--i)
    {
        m[MAKE_KEY(i)]=i;
    }
}

inline void insertdata(std::vector<tVecData>  & m,int testcount)
{

     for(int i = testcount ;i>0 ;--i)
     {
         m.push_back(tVecData(MAKE_KEY(i),i));
     }

     std::sort(std::begin(m),std::end(m),std::less<tVecData>());
}
void Map_hast_Test::testCase_insert()
{
    QFETCH(QString,testcontainer);
    QFETCH(int,testcount);

#ifdef TEST_KEY_STRING
    createStringNumberArray(testcount);
#endif

    if(testcontainer == QLatin1String("QMap_insert"))
    {
        QMap<tTestKey,tTestValue> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = m.find(MAKE_KEY(testcount));
        if(*it != testcount) QFAIL( "fail");
    }
    else if (testcontainer == QLatin1String("QHash_insert"))
    {
        QHash<tTestKey,tTestValue> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = m.find(MAKE_KEY(testcount));
        if(*it != testcount) QFAIL( "fail");

    }
    else if (testcontainer == QLatin1String("stdvector_pb_sort"))
    {
        std::vector<tVecData> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = std::lower_bound(std::begin(m),std::end(m),tVecData(MAKE_KEY(testcount),0));
        if(it->second != testcount) QFAIL( "fail");

    }
    else if (testcontainer == QLatin1String("stdmap_insert"))
    {
        std::map<tTestKey,tTestValue> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = m.find(MAKE_KEY(testcount));
        if(it->second != testcount) QFAIL( "fail");

    }
    else if (testcontainer == QLatin1String("stdunordered_insert"))
    {
        std::unordered_map<tTestKey,tTestValue> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = m.find(MAKE_KEY(testcount));
        if(it->second != testcount) QFAIL( "fail");

    }
#ifdef USE_BOOST
    else if (testcontainer == QLatin1String("boostunordered_insert"))
    {
        boost::unordered_map<tTestKey,tTestValue> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = m.find(MAKE_KEY(testcount));
        if(it->second != testcount) QFAIL( "fail");

    }
#endif
    else if (testcontainer == QLatin1String("qfasthash_insert"))
    {
        QFastHash<tTestKey,tTestValue> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = m.constFind(MAKE_KEY(testcount));
        if(it != testcount) QFAIL( "fail");

    }
}

void Map_hast_Test::testCase_find()
{
    QFETCH(QString,testcontainer);
    QFETCH(int,testcount);

#ifdef TEST_KEY_STRING
    createStringNumberArray(testcount);
#endif

    if(testcontainer == QLatin1String("QMap_find"))
    {
        QMap<tTestKey,tTestValue> m_mapTest;
        insertdata(m_mapTest,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                auto it = m_mapTest.find(MAKE_KEY(i));
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
                auto it = m_mapTest.constFind(MAKE_KEY(i));
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
                auto it = m_hashTest.find(MAKE_KEY(i));
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
                auto it = m_hashTest.constFind(MAKE_KEY(i));
                if(*it != i) QFAIL( "fail");
            }
        }

    }
    else if (testcontainer == QLatin1String("QVector_lowerbound"))
    {
       QVector<tVecData> m_vecTest;
        for(int i = 1 ;i<= testcount ;++i)
        {
            m_vecTest.push_back(tVecData(MAKE_KEY(i),i));
        }
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                auto it = std::lower_bound(std::begin(m_vecTest),std::end(m_vecTest),tVecData(MAKE_KEY(i),0));
                if(it->second != i) QFAIL( "fail");
            }
        }

    }
    else if (testcontainer == QLatin1String("stdvector_lowerbound"))
    {
       std::vector<tVecData> m_vecTest;
        for(int i = 1 ;i<= testcount ;++i)
        {
            m_vecTest.push_back(tVecData(MAKE_KEY(i),i));
        }
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                auto it = std::lower_bound(std::begin(m_vecTest),std::end(m_vecTest),tVecData(MAKE_KEY(i),0));
                if(it->second != i) QFAIL( "fail");
            }
        }

    }
    else if (testcontainer == QLatin1String("stdmap_find"))
    {
        std::map<tTestKey,tTestValue> m_stdmapTest;
        insertdata(m_stdmapTest,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                auto it = m_stdmapTest.find(MAKE_KEY(i));
                if(it->second != i) QFAIL( "fail");
            }
        }

    }
    else if (testcontainer == QLatin1String("stdunordered_find"))
    {
        std::unordered_map <tTestKey,tTestValue> m_stdunorderedTest;
        insertdata(m_stdunorderedTest,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                auto it = m_stdunorderedTest.find(MAKE_KEY(i));
                if(it->second != i) QFAIL( "fail");
            }
        }

    }
#ifdef USE_BOOST
    else if (testcontainer == QLatin1String("boostunordered_find"))
    {
        boost::unordered_map <tTestKey,tTestValue> m_stdunorderedTest;
        insertdata(m_stdunorderedTest,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                auto it = m_stdunorderedTest.find(MAKE_KEY(i));
                if(it->second != i) QFAIL( "fail");
            }
        }

    }
#endif
    else if (testcontainer == QLatin1String("qfasthash_find"))
    {
        QFastHash<tTestKey,tTestValue> m_fasthash_Test;
        insertdata(m_fasthash_Test,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                auto it = m_fasthash_Test.constFind(MAKE_KEY(i));
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
