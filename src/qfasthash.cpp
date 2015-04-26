#include "qfasthash_p.h"

QT_BEGIN_NAMESPACE

// BEGIN copied from qhash.cpp (and then int -> size_t)
/*
    The prime_deltas array contains the difference between a power
    of two and the next prime number:

    prime_deltas[i] = nextprime(2^i) - 2^i

    Basically, it's sequence A092131 from OEIS, assuming:
    - nextprime(1) = 1
    - nextprime(2) = 2
    and
    - left-extending it for the offset 0 (A092131 starts at i=1)
    - stopping the sequence at i = 28 (the table is big enough...)
*/

static const uchar prime_deltas[] = {
    0,  0,  1,  3,  1,  5,  3,  3,  1,  9,  7,  5,  3, 17, 27,  3,
    1, 29,  3, 21,  7, 17, 15,  9, 43, 35, 15,  0,  0,  0,  0,  0
};

/*
    The primeForNumBits() function returns the prime associated to a
    power of two. For example, primeForNumBits(8) returns 257.
*/

static inline size_t primeForNumBits(size_t numBits)
{
    return (size_t(1) << numBits) + prime_deltas[numBits];
}

/*
    Returns the smallest integer n such that
    primeForNumBits(n) >= hint.
*/
static size_t countBits(size_t hint)
{
    size_t numBits = 0;
    size_t bits = hint;

    while (bits > 1) {
        bits >>= 1;
        numBits++;
    }

    if (numBits >= sizeof(prime_deltas)) {
        numBits = sizeof(prime_deltas) - 1;
    } else if (primeForNumBits(numBits) < hint) {
        ++numBits;
    }
    return numBits;
}
// END copied from qhash.cpp


// static
size_t QtPrivate::QFastHashImpl::nextPrime(size_t n)
{
    const size_t result = primeForNumBits(qMax(countBits(n), size_t(4)));
    qfhDebug("nextPrime(%4zu) = %4zu", n, result);
    return result;
}

QT_END_NAMESPACE
