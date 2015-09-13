#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <random>
#include <array>

#define UINT unsigned int
#define CHAR char
#define SIZE_T size_t
#define USHORT unsigned short
#define UINT32 uint32_t
#define UINT16 uint16_t
#define UINT8 uint8_t
#define INT int

unsigned int _rotl(unsigned int value, int shift) {
    if ((shift &= 31) == 0)
      return value;
    return (value << shift) | (value >> (32 - shift));
}

unsigned int _rotr(unsigned int value, int shift) {
    if ((shift &= 31) == 0)
      return value;
    return (value >> shift) | (value << (32 - shift));
}

// Bernstein's hash
UINT HashBernstein(const CHAR *key, SIZE_T len) {
    UINT hash = 5381;
    for(UINT i = 0; i < len; ++i)
        hash = 33 * hash + key[i];
    return hash ^ (hash >> 16);
}

// Paul Larson (http://research.microsoft.com/~PALARSON/)
UINT HashLarson(const CHAR *key, SIZE_T len) {
    UINT hash = 0;
    for(UINT i = 0; i < len; ++i)
        hash = 101 * hash + key[i];
    return hash;
}

// Kernighan & Ritchie, "The C programming Language", 3rd edition.
UINT HashKernighanRitchie(const CHAR *key, SIZE_T len) {
    UINT hash = 0;
    for(UINT i = 0; i < len; ++i)
        hash = 31 * hash + key[i];
    return hash;
}

// My hash function
UINT Hash17_unrolled(const CHAR *key, SIZE_T len) {
    UINT hash = 0;
    for(UINT i = 0; i < (len & -2); i += 2) {
        hash = 17 * hash + (key[i] - ' ');
        hash = 17 * hash + (key[i+1] - ' ');
    }
    if(len & 1)
        hash = 17 * hash + (key[len-1] - ' ');
    return hash ^ (hash >> 16);
}

UINT Hash17(const CHAR *key, SIZE_T len) {
    UINT hash = 0;
    for(UINT i = 0; i < len; ++i) {
        hash = 17 * hash + (key[i] - ' ');
    }
    return hash ^ (hash >> 16);
}


// A hash function with multiplier 65599 (from Red Dragon book)
UINT Hash65599(const CHAR *key, SIZE_T len) {
    UINT hash = 0;
    for(UINT i = 0; i < len; ++i)
        hash = 65599 * hash + key[i];
    return hash ^ (hash >> 16);
}

// FNV hash, http://isthe.com/chongo/tech/comp/fnv/
UINT HashFNV1a(const CHAR *key, SIZE_T len) {
    UINT hash = 2166136261;
    for(UINT i = 0; i < len; ++i)
        hash = 16777619 * (hash ^ key[i]);
    return hash ^ (hash >> 16);
}

// Peter Weinberger's hash (from Red Dragon book)
UINT HashWeinberger(const CHAR *key, SIZE_T len) {
    UINT h = 0, g;
    for(UINT i = 0; i < len; ++i) {
        h = (h << 4) + key[i];
        if(( g = (h & 0xF0000000) ) != 0)
            h ^= g >> 24 ^ g;
    }
    return h ^ (h >> 16);
}

// Ramakrishna hash
UINT HashRamakrishna(const CHAR *key, SIZE_T len) {
    UINT h = 0;
    for(UINT i = 0; i < len; ++i) {
        h ^= (h << 5) + (h >> 2) + key[i];
    }
    return h;
}

// http://en.wikipedia.org/wiki/Fletcher's_checksum
UINT HashFletcher(const CHAR * key, SIZE_T len)
{
    const USHORT * data = (const USHORT *)key;
    len /= 2;
    UINT32 sum1 = 0xFFFF, sum2 = 0xFFFF;
    while (len) {
        SIZE_T tlen = len > 360 ? 360 : len;
        len -= tlen;
        do {
            sum1 += *data++;
            sum2 += sum1;
        } while (--tlen);
        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }
    /* Second reduction step to reduce sums to 16 bits */
    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    return sum2 << 16 | sum1;
}

// http://en.wikipedia.org/wiki/Adler-32
UINT32 HashAdler(const CHAR * data, SIZE_T len)
{
    UINT32 a = 1, b = 0;

    while(len > 0) {
        SIZE_T tlen = len > 5550 ? 5550 : len;
        len -= tlen;
        do {
            a += *data++;
            b += a;
        } while (--tlen);

        a %= 65521;
        b %= 65521;
    }
    return (b << 16) | a;
}

// http://murmurhash.googlepages.com/MurmurHash2.cpp
UINT HashMurmur2(const CHAR * key, SIZE_T len)
{
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.

    const unsigned int m = 0x5bd1e995;
    const int r = 24;

    // Initialize the hash to a 'random' value
    UINT seed = 0x3FB0BB5F;
    unsigned int h = seed ^ (UINT)len;

    // Mix 4 bytes at a time into the hash

    const unsigned char * data = (const unsigned char *)key;

    while(len >= 4)
    {
        unsigned int k = *(unsigned int *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array

    switch(len)
    {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0];
            h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

// Paul Hsieh, http://www.azillionmonkeys.com/qed/hash.html
UINT HashPaulHsieh(const CHAR* key, SIZE_T len) {
    UINT hash = (UINT)len, tmp;
    if(len == 0) return 0;

    SIZE_T rem = len & 3;
    len >>= 2;

    /* Main loop */
    for(;len > 0; len--) {
        hash  += *(const UINT16*)key;
        tmp    = (*(const UINT16*) (key+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        key   += 2 * sizeof (UINT16);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch(rem) {
        case 3:
            hash += *(const UINT16*)key;
            hash ^= hash << 16;
            hash ^= key[sizeof (UINT16)] << 18;
            hash += hash >> 11;
            break;
        case 2:
            hash += *(const UINT16*)key;
            hash ^= hash << 11;
            hash += hash >> 17;
            break;
        case 1:
            hash += *key;
            hash ^= hash << 10;
            hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

// Bob Jenkins, http://www.burtleburtle.net/bob/hash/doobs.html
UINT HashOneAtTime(const CHAR* key, SIZE_T len) {
    UINT hash, i;
    for(hash=0, i=0; i<len; ++i) {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

// Arash Partow, http://www.partow.net/programming/hashfunctions/
UINT HashArashPartow(const CHAR* key, SIZE_T len) {
    UINT hash = 0xAAAAAAAA;
    UINT i    = 0;

    for(i = 0; i < (len & -2); i += 2) {
        hash ^=   (hash <<  7) ^ (key[i]) ^ (hash >> 3);
        hash ^= ~((hash << 11) ^ (key[i+1]) ^ (hash >> 5));
    }
    if(len & 1) {
        hash ^= (hash <<  7) ^ (key[len - 1]) ^ (hash >> 3);
    }

    return hash;
}

// CRC-32
#define CRCPOLY 0xEDB88320
#define CRCINIT 0xFFFFFFFF

UINT g_crc_precalc[256];

void CRC32Init() {
    for(UINT i = 0; i <= 0xFF; i++) {
        UINT x = i;
        for(UINT j = 0; j < 8; j++)
            x = (x>>1) ^ (CRCPOLY & (-(INT)(x & 1)));
        g_crc_precalc[i] = x;
    }
}

UINT CRC32(const CHAR* key, SIZE_T len) {
    UINT crc = CRCINIT;
    for(UINT i = 0; i < len; i++)
        crc = g_crc_precalc[(crc ^ key[i]) & 0xFF] ^ (crc >> 8);
    return ~crc;
}

// Universal hash from Sedgewick's book "Algorithms in C", part 4
UINT HashUniversal(const CHAR* key, SIZE_T len) {
    UINT hash = 0, a = 31415, b = 27183;
    for(UINT i = 0; i < len; ++i) {
        hash = a * hash + key[i];
        a *= b;
    }
    return hash;
}

// === lookup3 function by Bob Jenkins ===

#define mix(a,b,c) \
{ \
  a -= c;  a ^= _rotl(c, 4);  c += b; \
  b -= a;  b ^= _rotl(a, 6);  a += c; \
  c -= b;  c ^= _rotl(b, 8);  b += a; \
  a -= c;  a ^= _rotl(c,16);  c += b; \
  b -= a;  b ^= _rotl(a,19);  a += c; \
  c -= b;  c ^= _rotl(b, 4);  b += a; \
}
#define final(a,b,c) \
{ \
  c ^= b; c -= _rotl(b,14); \
  a ^= c; a -= _rotl(c,11); \
  b ^= a; b -= _rotl(a,25); \
  c ^= b; c -= _rotl(b,16); \
  a ^= c; a -= _rotl(c,4);  \
  b ^= a; b -= _rotl(a,14); \
  c ^= b; c -= _rotl(b,24); \
}

UINT HashLookup3( const CHAR* key, SIZE_T length) {
  UINT a,b,c;                                          /* internal state */
  union { const void *ptr; size_t i; } u;

  /* Set up the internal state */
  a = b = c = 0xdeadbeef + ((UINT32)length);

  u.ptr = key;

  if ((u.i & 0x3) == 0) {
    const UINT32 *k = (const UINT32 *)key;         /* read 32-bit chunks */

    /*------ all but last block: aligned reads and affect 32 bits of (a,b,c) */
    while (length > 12)
    {
      a += k[0];
      b += k[1];
      c += k[2];
      mix(a,b,c);
      length -= 12;
      k += 3;
    }

    /*----------------------------- handle the last (probably partial) block */
    /*
     * "k[2]&0xffffff" actually reads beyond the end of the string, but
     * then masks off the part it's not allowed to read.  Because the
     * string is aligned, the masked-off tail is in the same word as the
     * rest of the string.  Every machine with memory protection I've seen
     * does it on word boundaries, so is OK with this.  But VALGRIND will
     * still catch it and complain.  The masking trick does make the hash
     * noticably faster for short strings (like English words).
     */
#ifndef VALGRIND

    switch(length)
    {
    case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
    case 11: c+=k[2]&0xffffff; b+=k[1]; a+=k[0]; break;
    case 10: c+=k[2]&0xffff; b+=k[1]; a+=k[0]; break;
    case 9 : c+=k[2]&0xff; b+=k[1]; a+=k[0]; break;
    case 8 : b+=k[1]; a+=k[0]; break;
    case 7 : b+=k[1]&0xffffff; a+=k[0]; break;
    case 6 : b+=k[1]&0xffff; a+=k[0]; break;
    case 5 : b+=k[1]&0xff; a+=k[0]; break;
    case 4 : a+=k[0]; break;
    case 3 : a+=k[0]&0xffffff; break;
    case 2 : a+=k[0]&0xffff; break;
    case 1 : a+=k[0]&0xff; break;
    case 0 : return c;              /* zero length strings require no mixing */
    }

#else /* make valgrind happy */

    k8 = (const UINT8 *)k;
    switch(length)
    {
    case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
    case 11: c+=((UINT32)k8[10])<<16;  /* fall through */
    case 10: c+=((UINT32)k8[9])<<8;    /* fall through */
    case 9 : c+=k8[8];                   /* fall through */
    case 8 : b+=k[1]; a+=k[0]; break;
    case 7 : b+=((UINT32)k8[6])<<16;   /* fall through */
    case 6 : b+=((UINT32)k8[5])<<8;    /* fall through */
    case 5 : b+=k8[4];                   /* fall through */
    case 4 : a+=k[0]; break;
    case 3 : a+=((UINT32)k8[2])<<16;   /* fall through */
    case 2 : a+=((UINT32)k8[1])<<8;    /* fall through */
    case 1 : a+=k8[0]; break;
    case 0 : return c;
    }

#endif /* !valgrind */

  } else if ((u.i & 0x1) == 0) {
    const UINT16 *k = (const UINT16 *)key;         /* read 16-bit chunks */
    const UINT8  *k8;

    /*--------------- all but last block: aligned reads and different mixing */
    while (length > 12)
    {
      a += k[0] + (((UINT32)k[1])<<16);
      b += k[2] + (((UINT32)k[3])<<16);
      c += k[4] + (((UINT32)k[5])<<16);
      mix(a,b,c);
      length -= 12;
      k += 6;
    }

    /*----------------------------- handle the last (probably partial) block */
    k8 = (const UINT8 *)k;
    switch(length)
    {
    case 12: c+=k[4]+(((UINT32)k[5])<<16);
             b+=k[2]+(((UINT32)k[3])<<16);
             a+=k[0]+(((UINT32)k[1])<<16);
             break;
    case 11: c+=((UINT32)k8[10])<<16;     /* fall through */
    case 10: c+=k[4];
             b+=k[2]+(((UINT32)k[3])<<16);
             a+=k[0]+(((UINT32)k[1])<<16);
             break;
    case 9 : c+=k8[8];                      /* fall through */
    case 8 : b+=k[2]+(((UINT32)k[3])<<16);
             a+=k[0]+(((UINT32)k[1])<<16);
             break;
    case 7 : b+=((UINT32)k8[6])<<16;      /* fall through */
    case 6 : b+=k[2];
             a+=k[0]+(((UINT32)k[1])<<16);
             break;
    case 5 : b+=k8[4];                      /* fall through */
    case 4 : a+=k[0]+(((UINT32)k[1])<<16);
             break;
    case 3 : a+=((UINT32)k8[2])<<16;      /* fall through */
    case 2 : a+=k[0];
             break;
    case 1 : a+=k8[0];
             break;
    case 0 : return c;                     /* zero length requires no mixing */
    }

  } else {                        /* need to read the key one byte at a time */
    const UINT8 *k = (const UINT8 *)key;

    /*--------------- all but the last block: affect some 32 bits of (a,b,c) */
    while (length > 12)
    {
      a += k[0];
      a += ((UINT32)k[1])<<8;
      a += ((UINT32)k[2])<<16;
      a += ((UINT32)k[3])<<24;
      b += k[4];
      b += ((UINT32)k[5])<<8;
      b += ((UINT32)k[6])<<16;
      b += ((UINT32)k[7])<<24;
      c += k[8];
      c += ((UINT32)k[9])<<8;
      c += ((UINT32)k[10])<<16;
      c += ((UINT32)k[11])<<24;
      mix(a,b,c);
      length -= 12;
      k += 12;
    }

    /*-------------------------------- last block: affect all 32 bits of (c) */
    switch(length)                   /* all the case statements fall through */
    {
    case 12: c+=((UINT32)k[11])<<24;
    case 11: c+=((UINT32)k[10])<<16;
    case 10: c+=((UINT32)k[9])<<8;
    case 9 : c+=k[8];
    case 8 : b+=((UINT32)k[7])<<24;
    case 7 : b+=((UINT32)k[6])<<16;
    case 6 : b+=((UINT32)k[5])<<8;
    case 5 : b+=k[4];
    case 4 : a+=((UINT32)k[3])<<24;
    case 3 : a+=((UINT32)k[2])<<16;
    case 2 : a+=((UINT32)k[1])<<8;
    case 1 : a+=k[0];
             break;
    case 0 : return c;
    }
  }

  final(a,b,c);
  return c;
}

const size_t BUCKETS = 256;
const size_t NTESTS = 1L << 20;
const size_t NFUNCTIONS = 18;

typedef unsigned int hash_func_t(const char *key, size_t len);

template <hash_func_t HF>
unsigned int bucket_for(uint64_t key) {
    unsigned int hash = HF((const char *)&key, sizeof(key));
    return hash % BUCKETS;
}

template <hash_func_t HF>
bool collide(uint64_t key1, uint64_t key2) {
    unsigned int bucket1 = bucket_for<HF>(key1);
    unsigned int bucket2 = bucket_for<HF>(key2);
    return bucket1 == bucket2;
}

#define FOR_ALL_HASHES(MACRO) \
    MACRO(HashBernstein, 0); \
    MACRO(HashLarson, 1); \
    MACRO(HashKernighanRitchie, 3); \
    MACRO(Hash17_unrolled, 4); \
    MACRO(Hash17, 5); \
    MACRO(Hash65599, 6); \
    MACRO(HashFNV1a, 7); \
    MACRO(HashWeinberger, 8); \
    MACRO(HashRamakrishna, 9); \
    MACRO(HashFletcher, 10); \
    MACRO(HashMurmur2, 11); \
    MACRO(HashPaulHsieh, 12); \
    MACRO(HashOneAtTime, 13); \
    MACRO(HashArashPartow, 14); \
    MACRO(HashUniversal, 15); \
    MACRO(HashLookup3, 16);

unsigned int our_hash(uint64_t key) {
    return 734507 * key + 58578;
}


// TODO make these not globals!
const uint64_t uint64_t_max = std::numeric_limits<uint64_t>::max();

template <typename Dist>
class Generator {
public:
    std::default_random_engine &engine;
    Dist distribution;
    bool discrete;

    Generator(std::default_random_engine &eng,
              Dist dist,
              bool disc) :
        engine(eng),
        distribution(dist),
        discrete(disc) // One day, let's eliminate this.
    {};

    // Limit the distribution to the range [0.0, 1.0].
    double sample() {
        for (;;) {
            double val = distribution(engine);
            if (val >= 0.0 && val < 1.0) {
                return val;
            }
        }
    }

    // Scale a sample to the range of 64-bit unsigned integers.
    uint64_t operator()() {
        if (discrete) {
            return distribution(engine);
        } else {
            //return sample() * uint64_t_max;
            return sample() * 1000;
        }
    }
};

int main(int argc, const char **argv) {
    size_t collisions[NFUNCTIONS];
    memset(collisions, 0, sizeof(collisions));

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine engine(seed);

    std::normal_distribution<double> distribution(0.0, 1.0);
    Generator< std::normal_distribution<double> > gen(engine, distribution,
            false);
    /*
    std::array<double, 3> intervals = {0.0, 0.00001, 1.0};
    std::array<double, 2> weights = {1.0, 0.000001};
    std::piecewise_constant_distribution<double> distribution
        (intervals.begin(), intervals.end(), weights.begin());
    Generator< std::piecewise_constant_distribution<double> > gen(engine,
            distribution, false);
    std::poisson_distribution<uint64_t> distribution(1000);
    Generator< std::poisson_distribution<uint64_t> > gen(engine,
            distribution, true);
    */
    /*
    std::uniform_int_distribution<uint64_t> distribution(10000900, 10001100);
    Generator< std::uniform_int_distribution<uint64_t> > gen(engine,
            distribution, true);
    */

    // Hash some keys.
    for (int i = 0; i < NTESTS; ++i) {
        uint64_t key1 = gen();
        uint64_t key2 = gen();

        #define COLLIDE(NAME, INDEX) \
            if (collide<NAME>(key1, key2)) { \
                ++collisions[INDEX]; \
            }
        FOR_ALL_HASHES(COLLIDE);

        if (our_hash(key1) % BUCKETS == our_hash(key2) % 251) {
            // printf("%u %llu %llu\n", our_hash(key1) % 251, key1, key2);
            ++collisions[NFUNCTIONS - 1];
        }
    }

    // Print out probabilities.
    #define PRINT_PROB(NAME, INDEX) \
        printf(#NAME ": %f\n", \
            (((float) collisions[INDEX]) / NTESTS) * BUCKETS);
    FOR_ALL_HASHES(PRINT_PROB);

    printf("OurHash: %f\n",
        (((float) collisions[NFUNCTIONS - 1]) / NTESTS) * BUCKETS);
}
