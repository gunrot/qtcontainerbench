/****************************************************************************
**
** Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Marc Mutz <marc.mutz@kdab.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QFASTHASH_P_H
#define QFASTHASH_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qglobal.h>

#include <QtCore/qvector.h>
#include <QtCore/qhash.h>

#include <algorithm>

#define qfhDebug if (true) {} else qDebug
//#define qfhDebug qDebug

QT_BEGIN_NAMESPACE

namespace QtPrivate {

template <typename T>
class Optional { // ### todo: specialize for T* (bit-stealing) and pimpl'ed classes (dto)
    T t;
    bool exists;

public:
    Optional() : t(), exists(false) {}
    explicit Optional(const T &t) : t(t), exists(true) {}
    explicit Optional(T &&t) : t(std::move(t)), exists(true) {}

    operator T&() Q_DECL_NOTHROW { return t; }
    operator const T&() const Q_DECL_NOTHROW { return t; }

    bool engaged() const Q_DECL_NOTHROW { return exists; }
};

template <typename T>
inline bool operator==(const Optional<T> &lhs, const Optional<T> &rhs)
    Q_DECL_NOEXCEPT_EXPR(noexcept(static_cast<const T&>(lhs) == static_cast<const T&>(rhs)))
{ return lhs.engaged() == rhs.engaged() && lhs.engaged() && static_cast<const T&>(lhs) == static_cast<const T&>(rhs); }

template <typename T>
inline bool operator==(const Optional<T> &lhs, const T &rhs)
    Q_DECL_NOEXCEPT_EXPR(noexcept(static_cast<const T&>(lhs) == rhs))
{ return lhs.engaged() && static_cast<const T&>(lhs) == rhs; }

template <typename T>
inline bool operator==(const T &lhs, const Optional<T> &rhs)
    Q_DECL_NOEXCEPT_EXPR(noexcept(lhs == static_cast<const T&>(rhs)))
{ return rhs.engaged() && lhs == static_cast<const T&>(rhs); }

template <typename T>
inline bool operator!=(const Optional<T> &lhs, const Optional<T> &rhs)
    Q_DECL_NOEXCEPT_EXPR(noexcept(lhs == rhs))
{ return !operator==(lhs, rhs); }

template <typename T>
inline bool operator!=(const Optional<T> &lhs, const T &rhs)
    Q_DECL_NOEXCEPT_EXPR(noexcept(lhs == rhs))
{ return !operator==(lhs, rhs); }

template <typename T>
inline bool operator!=(const T &lhs, const Optional<T> &rhs)
    Q_DECL_NOEXCEPT_EXPR(noexcept(lhs == rhs))
{ return !operator==(lhs, rhs); }

class QFastHashImpl {
protected:
    static Q_CORE_EXPORT size_t nextPrime(size_t target);
};

} // namespace QtPrivate

template <typename K, typename V>
class QFastHash : QtPrivate::QFastHashImpl
{
public:
    typedef K key_type;
    typedef V mapped_type;
    typedef std::pair<const K, V> value_type;
    typedef size_t size_type;
private:
    typedef typename QVector<QtPrivate::Optional<key_type> >::const_iterator internal_key_iterator;

public:
    QFastHash() : m_keys(), m_values(), m_size(0), m_maxLoadFactor(0.8) {}

    void swap(QFastHash &other) {
        m_keys.swap(other.m_keys);
        m_values.swap(other.m_values);
        qSwap(m_size, other.m_size);
        qSwap(m_maxLoadFactor, other.m_maxLoadFactor);
    }

    bool empty() const { return m_size == 0; }

    size_type size() const { return m_size; }
    size_type capacity() const { return size_type(m_keys.size()); }

    float max_load_factor() const { return m_maxLoadFactor; }
    void max_load_factor(float factor)
    {
        factor = qBound(0.1f, factor, 0.8f); // rather arbitrary choices (but ]0,1[ are hard limits)
        m_maxLoadFactor = factor;
        if (size())
            considerGrowingTable(size());
    }
    float load_factor() const { return 1.0f * size() / capacity(); } // ### numerically stable?

    int begin()   const { return 0; }
    int end()     const { return 0; }
    int cbegin()  const { return 0; }
    int cend()    const { return 0; }
    int rbegin()  const { return 0; }
    int rend()    const { return 0; }
    int crbegin() const { return 0; }
    int crend()   const { return 0; }



    mapped_type &operator[](const key_type &key)
    {
        const hash_code hc = hash(key);
        const size_type idx = probe(hc, key);
        if (idx < capacity() && m_keys[idx].engaged())
            return static_cast<mapped_type&>(m_values[idx]);
        return insertInternal(hc, key, mapped_type());
    }

    mapped_type constFind(const key_type &key)
    {
        const hash_code hc = hash(key);
        const size_type idx = probe(hc, key);
        if (idx < capacity() && m_keys[idx].engaged())
            return static_cast<mapped_type&>(m_values[idx]);
        return mapped_type();
    }


private:
    typedef uint hash_code;

    hash_code hash(const key_type &key) const
    {
        using QT_PREPEND_NAMESPACE(qHash);
        return qHash(key);
    }

    size_type probe(hash_code hc, const key_type &key) const
    {
        qfhDebug("probe called with size = %4zu, capacity = %4zu", size(), capacity());
        const size_type cap = capacity();
        if (!cap)
            return 0;
        const size_type h = static_cast<size_type>(hc);
        size_type i = h % cap;
        // linear probing, step == 1
        while (m_keys[i].engaged() && m_keys[i] != key)
            i = (i + 1) % cap;
        return i;
    }

    void considerGrowingTable(size_type requestedSize)
    {
        if (requestedSize >= max_load_factor() * capacity()) {
            qfhDebug("considerGrowingTable(%4zu) (size=%4zu, capacity=%4zu, mlf=%6.5f -> %s to %4zu",
                     requestedSize, size(), capacity(), max_load_factor(), "grow", this->nextPrime((requestedSize + 1) / max_load_factor()));
            grow(requestedSize + 1); // +1 since size() must always be < capacity() (except for 0 == 0)
        } else {
            qfhDebug("considerGrowingTable(%4zu) (size=%4zu, capacity=%4zu, mlf=%6.5f -> %s",
                   requestedSize, size(), capacity(), max_load_factor(), "don't grow");
        }
    }

    void grow(size_type requestedSize)
    {
        QFastHash grown;
        grown.max_load_factor(max_load_factor());
        grown.capacity(this->nextPrime(requestedSize / max_load_factor()));
        for (size_type i = 0, end = capacity(); i != end; ++i) {
            if (m_keys[i].engaged())
                grown.insertWithoutCheck(m_keys[i], m_values[i]);
        }
        swap(grown);
    }

    void capacity(size_type n) {
        Q_ASSERT(m_size == 0);
        qfhDebug("setting capacity to %4zu", n);
        m_keys.resize(n);
        m_values.resize(n);
    }

    mapped_type &insertInternal(hash_code hc, const key_type &key, const mapped_type &value)
    {
        considerGrowingTable(size() + 1);
        const size_type idx = probe(hc, key);
        if (!m_keys[idx].engaged()) {
            // ### not exception-safe!
            m_keys[idx] = QtPrivate::Optional<key_type>(key);
            m_values[idx] = QtPrivate::Optional<mapped_type>(value);
            ++m_size;
        }
        return static_cast<mapped_type&>(m_values[idx]);
    }

    void insertWithoutCheck(const key_type &key, const mapped_type &value)
    {
        const size_type idx = probe(hash(key), key);
        if (!m_keys[idx].engaged())
            ++m_size;
        m_keys[idx] = QtPrivate::Optional<key_type>(key);
        m_values[idx] = QtPrivate::Optional<mapped_type>(value);
    }

private:
    QVector<QtPrivate::Optional<key_type> > m_keys;
    QVector<QtPrivate::Optional<mapped_type> > m_values;
    size_t m_size;
    float m_maxLoadFactor;
};


QT_END_NAMESPACE

#endif // QFASTHASH_P_H
