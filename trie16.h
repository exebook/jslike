struct Node {
	Node *N[16];
	__int64_t value;
	Node() : N(), value(-1) { }
};

struct Trie16 {
	struct BitScanner {
		void *p; 
		int count, pos;
		BitScanner (void *p, int count) {
			this->p = p;
			this->count = count;
			pos = 0;
		}
		int next() {
			int bpos = pos >> 1;
			if (bpos >= count) return -1;
			unsigned char b = ((unsigned char*)p)[bpos];
			if (pos++ & 1) return (b >>= 4);
			return b & 0xf;
		}
	};
	
	Node root;

	bool add(void *key, int count, __int64_t value) {
		Node *C = &root;
		BitScanner B(key, count);
		while (true) {
			char u = B.next();
			if (u < 0) {
				if (C->value == -1) {
					C->value = value;
					return true; // value added
				}
				C->value = value;
				return false; // value replaced
			}
			Node *Q = C->N[u];
			if (Q) {
				C = Q;
			} else {
				C = C->N[u] = new Node;
			}
		}
	}

	Node* findNode(void *key, int count) {
		Node *C = &root;
		BitScanner B(key, count);
		while (true) {
			char u = B.next();
			if (u < 0) return C;
			__builtin_prefetch(C->N[u]);
			C = C->N[u];
			if (C == 0) return 0;
		}
	}
};
