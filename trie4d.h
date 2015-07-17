namespace trie4d {
#ifdef TRACE_MEM
int mem_count;
#endif

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
	
	Node root;
public:
	value_t *result;
	
	bool add(void *key, int count, value_t value) {
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
};

} // namespace trie4d