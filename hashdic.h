#include "string.h" // memcpy/memcmp

typedef __uint32_t u32;
typedef __int8_t i8;
typedef __uint8_t u8;

u32 MurMur(const char* key, int count) {
	#define muR(x, r) (((x) << r) | ((x) >> (32 - r)))
	u32 seed = 0x8e96a8f2;
	int n = count >> 2;
	u32 k, mur = seed, c1 = 0xcc9e2d51, c2 = 0x1b873593,
	*b = (u32*) (key + n * 4);
	for(int i = -n; i != 0; i++) {
		k = (muR(b[i] * c1, 15) * c2);
		mur = muR(mur^k, 13)*5+0xe6546b64;
	}
	u8 * tail = (u8*)(key + (n<<2));
	k = 0;
	switch(count & 3) {
		case 3: k ^= tail[2] << 16;
		case 2: k ^= tail[1] << 8;
		case 1: k ^= tail[0];
		k = mur ^ muR(k*c1,15)*c2;
	}
	
	mur ^= count;
	mur ^= (mur ^ mur >> 16) * 0x85ebca6b >> 13;
	return mur ^ (mur * 0xc2b2ae35 >> 16);
	#undef muR
}

#define hash MurMur

int initial_size = 1024;
float growth_threshold = 1.0; //when to resize, for example 0.5 means "if number of inserted keys is half of table length then resize". My experiments on english dictionary shows best performance/memory savings with 1.0.
float growth_factor = 10; // increase the size of hash table N times
int dups = 0, resizes = 0;
int mem_count = 0, slots=0;

typedef bool (*enumFunc)(void *key, int count, int *value, void *user);

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
	
	void forEach(enumFunc f, void *user) {
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

