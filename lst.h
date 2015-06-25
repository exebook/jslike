struct lst {

	typedef var** P;
	P p;
	int size, capacity, delta;

	lst() {
		basic();
	}

	~lst() {
		resize(0);
	}

	void basic() {
		p = 0;
		size = capacity = 0;
	}
	
	void resize(int newsize) {
		if (newsize <= 0) {
			for (int i = 0; i < size; i++) if (p[i]) delete p[i];
			if (p) delete[] p;
			basic();
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
//		int oldcap = capacity;
		if (newsize == capacity+1) capacity = newsize*2;
		else capacity = newsize;
		P o = p;
		p = new var* [capacity];
		for (int i = 0;  i < size;  i++) p[i] = o[i];
		for (int i = size; i < capacity; i++) p[i] = 0;
		delete[] o;
		size = newsize;
	}

	int operator ! () {
		return size;
	}

	void operator () (int i) {
		resize(i);
	}

	var pop() {
		if (size == 0) return undefined;
		var item = self[-1];
		resize(size - 1);
		return item;
	}
	void push(const var &a) {
		resize(size + 1);
		self[size-1] = a;
	}

	var& operator [](int i) { //TMPVAR? callback if op= ?
		if (i < 0) i = size + i;
		if (i >= size) resize(i + 1);
		if (p[i] == 0) p[i] = new var();
		return (*p[i]);
	}

	void delIns(int pos, int delCount, var *item, int insCount) {
		P o = p;
		int newSize = size - delCount + insCount;
		p = new var* [newSize];
		for (int i = 0; i < pos; i++) p[i] = o[i];
		for (int i = pos; i < pos + delCount; i++) delete o[i];
		for (int i = 0; i < insCount; i++) {
			p[pos+i] = new var;
			*p[pos+i] = item[i];//val|ref?
		}
		int remainCount = size - (pos + delCount);
		int a = pos + insCount, b = pos + delCount;
		for (int i = 0; i < remainCount; i++) p[a+i] = o[b+i];
		delete[] o;
		size = newSize;
	}

};
