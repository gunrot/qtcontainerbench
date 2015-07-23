#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QMap>
#include <algorithm>
#include <utility>
#include <iostream>
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
            s_vec.push_back(QString("%1").arg(QString::number(i),12) + "                               ");
    }
}

typedef QString tTestKey;
typedef int32_t tTestValue;
#define MAKE_KEY(x) s_vec[x]
//#define MAKE_KEY(x) QString::number(x)
#define CVT2VALUE(x) x.toLong()
#else
#if defined(TEST_KEY_DOUBLE)
typedef double tTestKey;
typedef double tTestValue;
#define CVT2VALUE(x) (x)
#elif defined(TEST_KEY_INT32)
typedef int32_t tTestKey;
typedef int32_t tTestValue;
#define CVT2VALUE(x) (x)
#endif
#define MAKE_KEY(x) x
#endif




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

inline size_t calcContainerCount(int testcount)
{
    return std::max(1UL,100000UL/testcount);
}

class QtContainerBench : public QObject
{
    Q_OBJECT

public:
    QtContainerBench();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_insert_data();
    void testCase_insert();
    void testCase_find();
    void testCase_find_data();
    void testCase_iterate_vector();
    void testCase_iterate_vector_data();

};


QtContainerBench::QtContainerBench()
{
}

void QtContainerBench::initTestCase()
{
    std::cerr << "sizeof(tVecData)=" <<sizeof(tVecData) << std::endl;
    std::cerr << "key is a "
#if defined (TEST_KEY_STRING)
                    << "QString"
#elif defined (TEST_KEY_DOUBlE)
                    << "double"
#elif defined (TEST_KEY_INT32)
                 << "int32_t"
#else
  #error No test key defined
#endif
                  " value type is a int32_t" << std::endl;
}

void QtContainerBench::cleanupTestCase()
{
}
static int testcounts[] = {5,10,20,40,50,80,100,1000,10000,100000,1000000};

void QtContainerBench::testCase_insert_data()
{
    QTest::addColumn<QString>("testcontainer");
    QTest::addColumn<int>("testcount");
    QString testContainer;

    static const char * tests[] = {
      "QMap_insert",
        "QHash_insert",
        "stdmap_insert",
        "QVector_pb_sort",
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
void QtContainerBench::testCase_find_data()
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

inline void insertdata(std::vector<QMap<tTestKey,tTestValue>> & m,int testcount)
{
    m.clear();
    m.resize(calcContainerCount(testcount));
    for(int i = testcount ;i> 0 ;--i)
    {
        for(size_t x=0;x < m.size();++x)
        {
            m[x].insert(MAKE_KEY(i),i);
        }
    }
}
inline void insertdata(std::vector<QHash<tTestKey,tTestValue>> & m,int testcount)
{
    m.clear();
    m.resize(calcContainerCount(testcount));
    for(int i = testcount ;i> 0 ;--i)
    {
        for(size_t x=0;x < m.size();++x)
        {
            m[x].insert(MAKE_KEY(i),i);
        }
    }

}
inline void insertdata(std::vector<std::map<tTestKey,tTestValue>> & m,int testcount)
{
    m.clear();
    m.resize(calcContainerCount(testcount));
    for(int i = testcount ;i> 0 ;--i)
    {
        for(size_t x=0;x < m.size();++x)
        {
            m[x][MAKE_KEY(i)]=i;
        }
    }
}

inline void insertdata(std::vector<std::unordered_map <tTestKey,tTestValue>> & m,int testcount)
{
    m.clear();
    m.resize(calcContainerCount(testcount));
    for(int i = testcount ;i> 0 ;--i)
    {
        for(size_t x=0;x < m.size();++x)
        {
            m[x][MAKE_KEY(i)]=i;
        }
    }
}
#ifdef USE_BOOST
inline void insertdata(std::vector<boost::unordered_map <tTestKey,tTestValue>> & m,int testcount)
{
    m.clear();
    m.resize(calcContainerCount(testcount));
    for(int i = testcount ;i> 0 ;--i)
    {
        for(size_t x=0;x < m.size();++x)
        {
            m[x][MAKE_KEY(i)]=i;
        }
    }
}
#endif

inline void insertdata(std::vector<QFastHash<tTestKey,tTestValue>> & m,int testcount)
{
    m.clear();
    m.resize(calcContainerCount(testcount));
    for(int i = testcount ;i> 0 ;--i)
    {
        for(size_t x=0;x < m.size();++x)
        {
            m[x][MAKE_KEY(i)]=i;
        }
    }
}

inline void insertdata(std::vector<std::vector<tVecData>>  & m,int testcount ,bool sorted = true)
{
    m.clear();
    m.resize(calcContainerCount(testcount));

    for(int i = testcount ;i>0 ;--i)
    {
        for(size_t x=0;x < m.size();++x)
        {
            m[x].push_back(tVecData(MAKE_KEY(i),i));
        }
    }
    if(!sorted) return;
    for(size_t x=0;x < m.size();++x)
    {
        std::sort(std::begin(m[x]),std::end(m[x]),std::less<tVecData>());
    }
}
inline void insertdata(std::vector<QVector<tVecData>>  & m,int testcount,bool sorted = true)
{
    m.clear();
    m.resize(calcContainerCount(testcount));

    for(int i = testcount ;i>0 ;--i)
    {
        for(size_t x=0;x < m.size();++x)
        {
            m[x].push_back(tVecData(MAKE_KEY(i),i));
        }
    }
    if(!sorted) return;
    for(size_t x=0;x < m.size();++x)
    {
        std::sort(std::begin(m[x]),std::end(m[x]),std::less<tVecData>());
    }
}
void QtContainerBench::testCase_insert()
{
    QFETCH(QString,testcontainer);
    QFETCH(int,testcount);

#ifdef TEST_KEY_STRING
    createStringNumberArray(testcount);
#endif

    if(testcontainer == QLatin1String("QMap_insert"))
    {
        std::vector<QMap<tTestKey,tTestValue>> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = m[0].find(MAKE_KEY(testcount));
        if(*it != testcount) QFAIL( "fail");
    }
    else if (testcontainer == QLatin1String("QHash_insert"))
    {
        std::vector<QHash<tTestKey,tTestValue>> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = m[0].find(MAKE_KEY(testcount));
        if(*it != testcount) QFAIL( "fail");

    }
    else if (testcontainer == QLatin1String("QVector_pb_sort"))
    {
        std::vector<QVector<tVecData>> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = std::lower_bound(std::begin(m[0]),std::end(m[0]),tVecData(MAKE_KEY(testcount),0));
        if(it->second != testcount) QFAIL( "fail");

    }
    else if (testcontainer == QLatin1String("stdvector_pb_sort"))
    {
        std::vector<std::vector<tVecData>> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = std::lower_bound(std::begin(m[0]),std::end(m[0]),tVecData(MAKE_KEY(testcount),0));
        if(it->second != testcount) QFAIL( "fail");

    }
    else if (testcontainer == QLatin1String("stdmap_insert"))
    {
        std::vector<std::map<tTestKey,tTestValue>> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = m[0].find(MAKE_KEY(testcount));
        if(it->second != testcount) QFAIL( "fail");

    }
    else if (testcontainer == QLatin1String("stdunordered_insert"))
    {
        std::vector<std::unordered_map<tTestKey,tTestValue>> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = m[0].find(MAKE_KEY(testcount));
        if(it->second != testcount) QFAIL( "fail");

    }
#ifdef USE_BOOST
    else if (testcontainer == QLatin1String("boostunordered_insert"))
    {
        std::vector<boost::unordered_map<tTestKey,tTestValue>> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = m[0].find(MAKE_KEY(testcount));
        if(it->second != testcount) QFAIL( "fail");

    }
#endif
    else if (testcontainer == QLatin1String("qfasthash_insert"))
    {
        std::vector<QFastHash<tTestKey,tTestValue>> m;

        QBENCHMARK {
            insertdata(m,testcount);
        }
        auto it = m[0].constFind(MAKE_KEY(testcount));
        if(it != testcount) QFAIL( "fail");

    }
}

void QtContainerBench::testCase_find()
{
    QFETCH(QString,testcontainer);
    QFETCH(int,testcount);

#ifdef TEST_KEY_STRING
    createStringNumberArray(testcount);
#endif

    if(testcontainer == QLatin1String("QMap_find"))
    {
        std::vector<QMap<tTestKey,tTestValue>> m;
        insertdata(m,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                for(size_t x=0;x < m.size();++x)
                {
                    auto it = m[x].find(MAKE_KEY(i));
                    if(*it != i) QFAIL( "fail");
                }
            }
        }
    }
    else if (testcontainer == QLatin1String("QMap_constFind"))
    {
        std::vector<QMap<tTestKey,tTestValue>> m;
        insertdata(m,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                for(size_t x=0;x < m.size();++x)
                {
                    auto it = m[x].constFind(MAKE_KEY(i));
                    if(*it != i) QFAIL( "fail");
                }
            }
        }

    }
    else if (testcontainer == QLatin1String("QHash_find"))
    {
        std::vector<QHash<tTestKey,tTestValue>> m;
        insertdata(m,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                for(size_t x=0;x < m.size();++x)
                {
                    auto it = m[x].find(MAKE_KEY(i));
                    if(*it != i) QFAIL( "fail");
                }
            }
        }

    }
    else if (testcontainer == QLatin1String("QHash_constFind"))
    {
        std::vector<QHash<tTestKey,tTestValue>> m;
        insertdata(m,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                for(size_t x=0;x < m.size();++x)
                {
                    auto it = m[x].constFind(MAKE_KEY(i));
                    if(*it != i) QFAIL( "fail");
                }
            }
        }

    }
    else if (testcontainer == QLatin1String("QVector_lowerbound"))
    {
        std::vector<QVector<tVecData>> m;
        insertdata(m,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                for(size_t x=0;x < m.size();++x)
                {
                    auto it = std::lower_bound(std::begin(m[x]),std::end(m[x]),tVecData(MAKE_KEY(i),0));
                    if(it->second != i)
                        QFAIL( "fail");
                }
            }
        }

    }
    else if (testcontainer == QLatin1String("stdvector_lowerbound"))
    {
        std::vector<std::vector<tVecData>> m;
        insertdata(m,testcount);

        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                for(size_t x=0;x < m.size();++x)
                {
                    auto it = std::lower_bound(std::begin(m[x]),std::end(m[x]),tVecData(MAKE_KEY(i),0));
                    if(it->second != i)
                        QFAIL( "fail");
                }
            }
        }

    }
    else if (testcontainer == QLatin1String("stdmap_find"))
    {
        std::vector<std::map<tTestKey,tTestValue>> m;
        insertdata(m,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                for(size_t x=0;x < m.size();++x)
                {
                    auto it = m[x].find(MAKE_KEY(i));
                    if(it->second != i) QFAIL( "fail");
                }
            }
        }

    }
    else if (testcontainer == QLatin1String("stdunordered_find"))
    {
        std::vector<std::unordered_map <tTestKey,tTestValue>> m;
        insertdata(m,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                for(size_t x=0;x < m.size();++x)
                {
                    auto it = m[x].find(MAKE_KEY(i));
                    if(it->second != i) QFAIL( "fail");
                }
            }
        }

    }
#ifdef USE_BOOST
    else if (testcontainer == QLatin1String("boostunordered_find"))
    {
        std::vector<boost::unordered_map <tTestKey,tTestValue>> m;
        insertdata(m,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                for(size_t x=0;x < m.size();++x)
                {
                    auto it = m[x].find(MAKE_KEY(i));
                    if(it->second != i) QFAIL( "fail");
                }
            }
        }

    }
#endif
    else if (testcontainer == QLatin1String("qfasthash_find"))
    {
        std::vector<QFastHash<tTestKey,tTestValue>> m;
        insertdata(m,testcount);
        QBENCHMARK {
            for(int i = 1 ;i <= testcount ;++i)
            {
                for(size_t x=0;x < m.size();++x)
                {
                    auto it = m[x].constFind(MAKE_KEY(i));
                    if(it != i) QFAIL( "fail");
                }
            }
        }

    }
    else
    {
        QFAIL( "fail, unknown test ");
    }

}


void QtContainerBench::testCase_iterate_vector_data()
{
    QTest::addColumn<QString>("testcontainer");
    QTest::addColumn<int>("testcount");
    QString testContainer;
    static const char * tests[] = {
        "stdvector_fwd_it",
        "QVector_fwd_it",
        "stdvector_fwd_idx",
        "QVector_fwd_idx",
        "stdvector_pushback",
        "QVector_pushback"
     };

     for (unsigned int t = 0; t < sizeof(tests)/sizeof(tests[0]);++t)
     {

        for(int count : testcounts)
        {
            QString text;
            text += QLatin1String(tests[t]);
            text = text.leftJustified(30, ' ', true);
            text +=  QLatin1String(" -- ");
            text += QString::number(count).leftJustified(12, ' ');


            QTest::newRow(text.toLatin1().constData()) << QString(tests[t]) << count;
        }

    }
}
void QtContainerBench::testCase_iterate_vector()
{
    QFETCH(QString,testcontainer);
    QFETCH(int,testcount);

#ifdef TEST_KEY_STRING
    createStringNumberArray(testcount);
#endif

    if (testcontainer == QLatin1String("QVector_fwd_it"))
    {
        std::vector<QVector<tVecData>> m;
        insertdata(m,testcount);
        QBENCHMARK {
                for(size_t x=0;x < m.size();++x)
                {
                    for(auto it =std::begin(m[x]) ;it != std::end(m[x]) ; ++it)
                    {
                        if((it->second) == 0)
                            QFAIL( "fail");
                    }
                 }
        }

    }
    else if (testcontainer == QLatin1String("stdvector_fwd_it"))
    {
        std::vector<std::vector<tVecData>> m;
        insertdata(m,testcount);

        QBENCHMARK {
            for(size_t x=0;x < m.size();++x)
            {
                for(auto it =std::begin(m[x]) ;it != std::end(m[x]) ; ++it)
                {
                    if((it->second) == 0)
                        QFAIL( "fail");
                }
            }
         }
    }
    else    if (testcontainer == QLatin1String("QVector_fwd_idx"))
    {
        std::vector<QVector<tVecData>> m;
        insertdata(m,testcount);
        QBENCHMARK {
                for(size_t x=0;x < m.size();++x)
                {
                    size_t sz= m[x].size();
                    auto & v = m[x];
                    for(size_t i = 0 ;i < sz ; ++i)
                    {
                        auto & e =v[i];
                        if((e.second) == 0)
                            QFAIL( "fail");
                    }
                 }
        }

    }
    else if (testcontainer == QLatin1String("stdvector_fwd_idx"))
    {
        std::vector<std::vector<tVecData>> m;
        insertdata(m,testcount);

        QBENCHMARK {
            for(size_t x=0;x < m.size();++x)
            {
                size_t sz= m[x].size();
                auto & v = m[x];
                for(size_t i = 0 ;i < sz ; ++i)
                {
                    auto & e =v[i];
                    if((e.second) == 0)
                        QFAIL( "fail");
                }
             }
          }
    }
    else if(testcontainer == QLatin1String("stdvector_pushback"))
    {
        std::vector<std::vector<tVecData>> m;

        QBENCHMARK {
            insertdata(m,testcount,false);
        }

    }
    else if(testcontainer == QLatin1String("QVector_pushback"))
    {
        std::vector<QVector<tVecData>> m;

        QBENCHMARK {
            insertdata(m,testcount,false);
        }

    }
    else
    {
        QFAIL( "fail, unknown test ");
    }

}


QTEST_MAIN(QtContainerBench)

#include "tst_qtcontainerbench.moc"
