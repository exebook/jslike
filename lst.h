template <typename T> class lst {
	typedef T** P;
	P p;
	int capacity, size;
	void zeroInit() {
		p = 0;
		size = capacity = 0;
	}
	
public:
	lst() {
		zeroInit();
	}

	~lst() {
		resize(0);
	}
	
	int length() {
		return size;
	}

	void resize(int newsize) {
		if (newsize <= 0) {
			for (int i = 0; i < size; i++) if (p[i]) delete p[i];
			if (p) delete[] p;
			zeroInit();
			return;
		};
		if (newsize == size) return;
		if (newsize < size) {
			for (int i = newsize; i < size; i++) {
				if (p[i]) delete p[i];
				p[i] = 0;
			}
			size = newsize;
			return;
		}
		if (newsize <= capacity) {
			size = newsize;
			return;
		}
		// capacity growth
		if (newsize == capacity+1) capacity = newsize*2;
		else capacity = newsize;
		P o = p;
		p = new T* [capacity];
		for (int i = 0;  i < size;  i++) p[i] = o[i];
		for (int i = size; i < capacity; i++) p[i] = 0;
		delete[] o;
		size = newsize;
	}
	
	void prealloc(int count) {
		capacity = count;
		p = new T* [capacity];
	}

	T pop() {
		if (size == 0) return undefined;
		T item = self[-1];
		resize(size - 1);
		return item;
	}
	void push(const T &a) {
		resize(size + 1);
		self[size-1] = a;
	}

	T& operator [](int i) { //TMPVAR? callback if op= ?
		if (i < 0) i = size + i;
		if (i >= size) resize(i + 1);
		if (p[i] == 0) p[i] = new T();
		return (*p[i]);
	}

	void delIns(int pos, int delCount, T *item, int insCount) {
		P o = p;
		int newSize = size - delCount + insCount;
		p = new T* [newSize];
		for (int i = 0; i < pos; i++) p[i] = o[i];
		for (int i = pos; i < pos + delCount; i++) delete o[i];
		for (int i = 0; i < insCount; i++) {
			p[pos+i] = new T;
			*p[pos+i] = item[i];//val|ref?
		}
		int remainCount = size - (pos + delCount);
		int a = pos + insCount, b = pos + delCount;
		for (int i = 0; i < remainCount; i++) p[a+i] = o[b+i];
		delete[] o;
		size = newSize;
	}

};

//typedef lstvar<var> lst;
