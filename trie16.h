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

int dummy = 0;

struct Node {
	Node *N[16];
	int value;
	Node() : N(), value(-1) { }
	~Node() {
		for (int i = 0; i < 16; i++) delete N[i];
	}
};

struct Trie16 {
	//TODO: replace pointers with numbers and store pointers in dymanic array
	//array pointers[];

	Node root;

	bool add(void *key, int count, int value) {
		Node *C = &root;
		BitScanner B(key, count);
		while (true) {
			int u = B.next();
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
//			if (C->N[0] == C->N[1]) dummy++;
			C = C->N[0+u];
			if (C == 0) return 0;
		}
	}
	
	int find(void* key, int count) {
		Node *n = findNode(key, count);
		if (n) return n->value;
		return -1;
	}
	
	void del(void *key, int count) {
		Node *n = findNode(key, count);
		if (n) n->value = -1;
	}

};

