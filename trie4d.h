//namespace trie4d {
typedef bool (*trieFunc)(void *key, int count, int *value, void *user);
const char * trieType = "trie4d, c++, value_t, prefetch"
#ifdef TRACE_MEM
", mem_count";
int mem_count;
#endif
;

template <typename value_t> class Trie4d {
	struct BitScanner { // separate a key in 4 bit chunks
		void *p; 
		int count, pos;
		BitScanner (void *p, int count): p(p), count(count), pos(0) {}
		int next() {
			int bpos = pos >> 1;
			if (bpos >= count) return -1;
			unsigned char b = ((unsigned char*)p)[bpos];
			if (pos++ & 1) return (b >>= 4);
			return b & 0xf;
		}
	};
	
	struct Node {
		char item;
		value_t value;
		Node **N;
		
		Node() : N(0), value(-1), item(-1) {}
		~Node() {
			if (item == -2) {
				for (int i = 0; i < 16; i++) delete N[i];
				delete[] N;
				#ifdef TRACE_MEM
				mem_count += sizeof(Node) * 16;
				#endif
			}
			else if (item >= 0) delete (Node*)N;
			#ifdef TRACE_MEM
			mem_count += sizeof(Node);
			#endif
		}
	};
	
	//structures for get keys iteration
	
	struct cnode {
		char c;
		cnode *p;
		cnode(char c, cnode *p):c(c), p(p) {}
	};
	
	struct iter_t {
		trieFunc f;
		void *user;
	};

	void iterate(Node *C, iter_t *iter, int size, cnode *p) {
		if (C->value != -1) {
			char *c = new char[size/2], *w = &c[size/2-1];
			do { *w-- = p->c << 4 | p->p->c; } while (p = p->p->p);
			iter->f(c, size/2, &C->value, iter->user);
			delete[] c;
		}
		if (C->item == -1) return;
		if (C->item >= 0) {
			cnode n(C->item, p);
			iterate((Node*)C->N, iter, size+1, &n);
			return;
		}
		for (int i = 0; i < 16; i++) {
			if (C->N[i]) {
				cnode n(i, p);
				iterate(C->N[i], iter, size+1, &n);
			}
		}
	}

	Node root;
public:
	value_t *result;
	
	bool add(void *key, int count) {//, value_t value) {
		Node *C = &root;
		BitScanner B(key, count);
		while (true) {
			char u = B.next();
			if (u < 0) {
				result = &C->value;
				if (C->value == (value_t)-1) return false; // new node added
				return true; // existing node found
			}
			if (C->item == -1) {
				C->item = u;
				C->N = (Node**) new Node();
				C = (Node*)C->N;
			}
			else if (C->item >= 0) {
				if (C->item == u) C = (Node*)C->N;
				else {
					Node *tmp = (Node*)C->N;
					C->N = new Node*[16]();
					C->N[C->item] = tmp;
					C->item = -2;
					C = C->N[u] = new Node();
				}
			} else {
				__builtin_prefetch(C->N[u]);// <-- optional
				if (!C->N[u]) C->N[u] = new Node();
				C = C->N[u];
			}
		}
	}
	
	bool find(void *key, int count) {
		Node *C = &root;
		BitScanner B(key, count);
		while (true) {
			char u = B.next();
			if (u < 0) {
				result = &C->value;
				return true;
			}
			if (C->item == -1) return false;
			else if (C->item >= 0) {
				if (u == C->item) C = (Node*)C->N;
				else return false;
			} else {
				__builtin_prefetch(C->N[u]);// <-- optional
				if (!(C = C->N[u])) return false;
			}
		}
	}

	void forEach(trieFunc f, void *user) {
		iter_t t;
		t.f = f;
		t.user = user;
		iterate(&root, &t, 0, 0);
	}
};

//} // namespace trie4d