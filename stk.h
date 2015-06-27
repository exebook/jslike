struct stk {
	typedef void *T;
	T* p;
	int size, cap;
	stk() { zero(); }
  ~stk() { resize(0); }
	void zero() { p = 0; size = cap = 0; }
	
	void resize(int N) {
		if (N <= 0) { if (p) delete[] p; zero(); return; };
		if (N == size) return;
		if (N < size || N <= cap) { size = N; return; }
		if (N == cap+1) cap = N*2;
		else cap = N;
		T* o = p;
		p = new T [cap];
		for (int i = 0;  i < size;  i++) p[i] = o[i];
		delete[] o;
		size = N;
	}

	int operator ! () { return size; }
	void operator () (int i) { resize(i); }

	T pop() {
		if (size == 0) return 0;
		T item = self[-1];
		resize(size - 1);
		return item;
	}
	void push(const T &a) {
		resize(size + 1);
		self[size-1] = a;
	}

	T& operator [](int i) {
		if (i < 0) i = size + i;
		if (i >= size) resize(i + 1);
		return p[i];
	}
};
