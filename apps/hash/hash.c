// (c) Peter Kankowski, 2008. http://smallcode.weblogs.us
// Hash functions benchmark

//#define USE_PRIME
#define USE_CHAINING
//#define STORE_32

#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef USE_PRIME
#include <intrin.h>
#pragma intrinsic(__emulu)
#endif

#define NUM_OF(A) (sizeof(A)/sizeof((A)[0]))

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



// === Collision resolution ====


typedef UINT (*HASH_FUNC)(const CHAR*, SIZE_T);

#define MAX_TABLE_SIZE 8209
#define MAX_TEXT_LEN (MAX_TABLE_SIZE * 16)

CHAR* g_lines[MAX_TABLE_SIZE];
CHAR g_text[MAX_TEXT_LEN];
UINT g_collisions, g_lines_count, g_table_size_mask;
UINT g_table_size, g_table_size_magic, g_table_size_shift;

#ifdef USE_PRIME
UINT inline mod_table_size(UINT x) {
	UINT mul_hi_dword = (UINT)( __emulu(x, g_table_size_magic) >> 32 );
	UINT ret = x - (mul_hi_dword >> g_table_size_shift) * g_table_size;
	assert(ret == x % g_table_size);
	return ret;
}
#endif


// =================
// Separate chaining
#ifdef USE_CHAINING

struct CHAIN {
	const CHAR* key;
	CHAIN* next;
};
CHAIN* g_table[MAX_TABLE_SIZE];
CHAIN g_chains[MAX_TABLE_SIZE];
UINT g_next_free_chain;

void InitHashTable() {
	g_next_free_chain = 0;
	g_collisions = 0;
	memset(g_table, 0, sizeof(g_table));
}

BOOL Lookup(const CHAR* key, SIZE_T len, HASH_FUNC hash_func, BOOL add) {
#ifdef USE_PRIME
	UINT hash = mod_table_size( hash_func(key, len) );
#else
	UINT hash = hash_func(key, len) & g_table_size_mask;
#endif
	// Look up the value in the chain
	UINT collisions = 0;
	for(CHAIN* chain = g_table[hash]; chain != NULL; chain = chain->next) {
		if( memcmp(chain->key, key, len * sizeof(CHAR)) == 0 )
			return TRUE;
		collisions++;
	}
	// If it was not found, add it
	if(add) {
		CHAIN* next = g_table[hash];
		g_table[hash] = &g_chains[g_next_free_chain++];
		g_table[hash]->next = next;
		g_table[hash]->key = key;
		g_collisions += collisions;
	}
	return FALSE;
}

// =================
// Open Addressing
#else

struct ITEM {
	const CHAR* key;
#ifdef STORE_32
	UINT hash32;
#endif
};

ITEM g_table[MAX_TABLE_SIZE];

void InitHashTable() {
	g_collisions = 0;
	memset(g_table, 0, sizeof(g_table));
}

BOOL Lookup(const CHAR* key, SIZE_T len, HASH_FUNC hash_func, BOOL add) {
	UINT hash32 = hash_func(key, len);
#ifdef USE_PRIME
	UINT hash = mod_table_size( hash32 );
#else
	UINT hash = hash32 & g_table_size_mask;
#endif
	// Look up the value in the table
	UINT collisions = 0;
	while(g_table[hash].key != 0) {
#ifdef STORE_32
		if( g_table[hash].hash32 == hash32 &&
			memcmp(g_table[hash].key, key, len * sizeof(CHAR)) == 0 )
			return TRUE;
#else
		if( memcmp(g_table[hash].key, key, len * sizeof(CHAR)) == 0 )
			return TRUE;
#endif
#ifdef USE_PRIME
		hash = mod_table_size(hash + 1);
#else
		hash = (hash + 1) & g_table_size_mask;
#endif
		collisions++;
	}
	// If it was not found, add it
	if(add) {
		g_table[hash].key = key;
#ifdef STORE_32
		g_table[hash].hash32 = hash32;
#endif
		g_collisions += collisions;
	}
	return FALSE;
}
#endif


// ======= Benchmarks ============

UINT64 inline GetRDTSC(void) {
   __asm {
      ; Flush the pipeline
      XOR eax, eax
      CPUID
      ; Get RDTSC counter in edx:eax
      RDTSC
   }
}

void BenchmarkHashFunction( HASH_FUNC hash_func, const CHAR* name ) {
	printf("%20s: ", name);
	INT min_time = INT_MAX;
	for(INT k = 0; k < 5; k++) {
		InitHashTable();
		UINT64 start = GetRDTSC();

		for(UINT i = 0; i < g_lines_count; ++i)
			Lookup(g_lines[i], strlen(g_lines[i]), hash_func, TRUE);
		
		for(UINT i = 0; i < g_lines_count; ++i)
			Lookup(g_lines[i], strlen(g_lines[i]), hash_func, FALSE);

		UINT64 time = GetRDTSC() - start;
		printf("%10d", (INT)(time + 500) / 1000);
		if(time < min_time)
			min_time = (INT)time;
	}
	printf("|%10d", (INT)(min_time + 500) / 1000);
	printf(" [%5d]\n", g_collisions);
}

void PrintTestWords(const HASH_FUNC hash_func[], const CHAR* name[], SIZE_T nfunc) {
	static const CHAR* word[] = {"too", "top", "tor", "tpp", "a000", "a001", "a002", "a003",
		"a004", "a005", "a006", "a007", "a008", "a009", "a010", "a", "aa", "aaa"
	};
	
	// Header
	printf( "%4c", ' ' );
	for(UINT j = 0; j < nfunc; j++) {
		CHAR func_name[16];
		strncpy(func_name, name[j], 9);
		func_name[9] = '\0';
		printf( "%10s", func_name );
	}
	printf( "\n" );

	for(UINT i = 0; i < NUM_OF(word); i++) {
		printf( "%4s", word[i] );
		for(UINT j = 0; j < nfunc; j++)
			printf( "%10x", hash_func[j](word[i], strlen(word[i])) );
		printf( "\n" );
	}
}

UINT NextPowerOfTwo(UINT x) {
	// Henry Warren, "Hacker's Delight", ch. 3.2
	x--;
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return x + 1;
}

UINT NextLog2(UINT x) {
	// Henry Warren, "Hacker's Delight", ch. 5.3
	if(x <= 1) return x;
	x--;
	UINT n = 0;
	UINT y;
	y = x >>16; if(y) {n += 16; x = y;}
	y = x >> 8; if(y) {n +=  8; x = y;}
	y = x >> 4; if(y) {n +=  4; x = y;}
	y = x >> 2; if(y) {n +=  2; x = y;}
	y = x >> 1; if(y) return n + 2;
	return n + x;
}


int main(int argc, char* argv[]) {
	CRC32Init();
	// Check value from the guide by Ross N. Williams
	assert( CRC32("123456789", 9) == 0xCBF43926 );

	if(argc <= 1) {
		puts("Usage: hash text-file");
		return 1;
	}

	// Read the file line-by-line into g_text buffer
	FILE* file = fopen(argv[1], "r");
	if(!file) {
		puts("Can't open the file");
		return 1;
	}
	CHAR* text = g_text;
	SIZE_T remaining = MAX_TEXT_LEN;
	UINT i = 0;

	while( fgets(text, (UINT)remaining, file) && i < MAX_TABLE_SIZE / 2 ) {
		SIZE_T len = strlen(text);
		if(text[len-1] == '\n')
			text[--len] = '\0';
		if(len) {
			g_lines[i++] = text;
			text += len + 1;
			remaining -= len + 1;
		}
	}
	fclose(file);
	g_lines_count = i;
#ifdef USE_PRIME
	                     // 1  2  4   8  16  32  64  128  256  512  1024  2048  4096  8192
	UINT closest_prime[] = {2, 2, 5, 11, 17, 37, 67, 131, 257, 521, 1031, 2053, 4099, 8209,
						    16411, 32771, 65537, 131101, 262147, 524309, 1048583, 2097169,
							4194319, 8388617, 16777259, 33554467, 67108879,	134217757,
							268435459, 536870923, 1073741827};
	UINT magic[32]         = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x7f218557, 0, 0xffd008ff, 0x7fbc240d};
	UINT shift[32]         = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 12, 12};
	UINT log = NextLog2(i) + 1;
	g_table_size = closest_prime[ log ];
	g_table_size_magic = magic[ log ];
	g_table_size_shift = shift[ log ];
	if( g_table_size_magic == 0 )
		printf("table size not supported: %d", g_table_size);
	printf("%d lines read\n"  "%d elements in the table\n",
		g_lines_count, g_table_size);
#else
	g_table_size_mask = NextPowerOfTwo(i) * 2 - 1;
	printf("%d lines read\n"  "%d elements in the table\n",
		g_lines_count, g_table_size_mask + 1);
#endif

	// Run the benchmarks
	static const HASH_FUNC func[] = {HashBernstein, HashKernighanRitchie,
		Hash17_unrolled, Hash65599, HashFNV1a, HashUniversal,
		HashWeinberger, HashLarson,
		HashPaulHsieh, HashOneAtTime, HashLookup3, HashArashPartow,
		CRC32, HashRamakrishna, HashFletcher, HashMurmur2 };
	static const CHAR* name[] = {"Bernstein", "Kernighan&Ritchie",
		"x17 unrolled", "x65599", "FNV-1a", "universal",
		"Weinberger", "HashLarson",
		"Paul Hsieh", "One At Time", "lookup3", "Arash Partow",
		"CRC-32", "Ramakrishna", "Fletcher", "Murmur2" };
	assert(NUM_OF(name) == NUM_OF(func));
	//PrintTestWords(func + 6, name + 6, NUM_OF(func) - 6);
	for(UINT i = 0; i < NUM_OF(func); i++)
		BenchmarkHashFunction(func[i], name[i]);

	return 0;
}
