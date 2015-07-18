#include "string.h" // memcpy/memcmp

typedef __uint32_t u32;
typedef __int8_t i8;
typedef __uint8_t u8;

static inline u32 fmix(u32 h) {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

static inline u32 rotl32(u32 x, i8 r) {
    return (x << r) | (x >> (32 - r));
}

u32 MurMur(const char* data, size_t len_) {
    const int len = (int) len_;
    const int nblocks = len / 4;
    u32 h1 = 0xc062fb4a;
    u32 c1 = 0xcc9e2d51;
    u32 c2 = 0x1b873593;
    const u32 * blocks = (const u32*) (data + nblocks * 4);
    int i;
    for(i = -nblocks; i; i++) {
        u32 k1 = blocks[i];
        k1 *= c1;
        k1 = rotl32(k1, 15);
        k1 *= c2;
        h1 ^= k1;
        h1 = rotl32(h1, 13);
        h1 = h1*5+0xe6546b64;
    }
    const u8 * tail = (const u8*)(data + nblocks*4);
    u32 k1 = 0;
    switch(len & 3) {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1: k1 ^= tail[0];
              k1 *= c1; k1 = rotl32(k1,15); k1 *= c2; h1 ^= k1;
    }
    h1 ^= len;
    h1 = fmix(h1);
    return h1;
}

#define hash MurMur

////////////////////////////////////////SUPERFASTHASH
int initial_size = 1024;
float growth_threshold = 1.0;
float growth_factor = 10;
int dups = 0, resizes = 0;
int mem_count = 0, slots=0;

typedef bool (*trieFunc)(void *key, int count, int *value, void *user);

template <typename value_t> struct jshash {
	struct keynode {

		char *key;
		short len;
		keynode *next;
		value_t value;
		
		keynode(char*k, int l) {
			len = l;
			key = new char[l];
			memcpy(key, k, l);
			next = 0;
			value = -1;
		}
		
		~keynode() {
			delete[] key;
			mem_count += sizeof(keynode) + len;
			if (next) delete next;
		}
		
		bool cmp(char*k, int l) {
			if (len != l) return false;
			return memcmp(key, k, l) == 0;
		}
	};
	
	struct entry {
		keynode *k;
		entry():k(0) {}
		~entry() {
			mem_count += sizeof(entry);
			if (k) delete k;
		}
	};
	
	entry **table;
	int length, count;
	
	jshash() {
		length = initial_size;
		count = 0;
		table = new entry*[initial_size]();
	}
	
	~jshash() {
		for (int i = 0; i < length; i++) if (table[i]) delete table[i];
		delete[] table;
		mem_count += sizeof(entry*) * length;
	}
	
	void resize(int newsize) {
	dups = 0; resizes++;
		int o = length;
		entry **old = table;
		table = new entry*[newsize]();
		length = newsize;
		for (int i = 0; i < o; i++) {
			if (old[i]) {
				keynode *k = old[i]->k;
				while (k) {
					keynode *next = k->next;
					k->next = 0;
					reinsert_when_resizing(k);
					k = next;
				}
				old[i]->k = 0;
			}
			delete old[i];
		}
		delete[] old;
	}
	
	value_t *result;
	
	bool reinsert_when_resizing(keynode *k2) {
		int h = (hash(k2->key, k2->len) >> 2);
		int n = h % length;
		if (table[n] == 0) {
			table[n] = new entry;
			table[n]->k = k2;
			result = &table[n]->k->value;
			return false;
		}
		keynode *k = table[n]->k;
		k2->next = table[n]->k;
		table[n]->k = k2;
		result = &k2->value;
		return false;
	}
	
	bool add(void *key, int keyn) {
		int h = (hash((const char*)key, keyn) >> 2);
		int n = h % length;
		if (table[n] == 0) {
			slots++;
			double f = (double)count / (double)length;
			if (f > growth_threshold) {
				resize(length * growth_factor);
				return add(key, keyn);
			}
			
			table[n] = new entry;
			table[n]->k = new keynode((char*)key, keyn);
			result = &table[n]->k->value;
			count++;
			return false;
		}
		keynode *k = table[n]->k;
		while (k) {
			if (k->cmp((char*)key, keyn)) {
				result = &k->value;
				return true;
			}
			k = k->next;
		}
		dups++;
		count++;
		keynode *k2 = new keynode((char*)key, keyn);
		k2->next = table[n]->k;
		table[n]->k = k2;
		result = &k2->value;
		return false;
	}
	
	bool find(void *key, int keyn) {
		int h = (hash((const char*)key, keyn) >> 2);
		int n = h % length;
//		__builtin_prefetch(table[n]);
		if (table[n] == 0) {
			return false;
		}
		keynode *k = table[n]->k;
		while (k) {
			if (k->cmp((char*)key, keyn)) {
				result = &k->value;
				return true;
			}
			k = k->next;
		}
		return false;
	}
	
	void forEach(trieFunc f, void *user) {
		for (int i = 0; i < length; i++) {
			if (table[i] != 0) {
				keynode *k = table[i]->k;
				while (k) {
					if (!f(k->key, k->len, &k->value, user)) return;
					k = k->next;
				}
			}
		}
	}
};

