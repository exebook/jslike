
struct chr {
	int size;
	jschar *s;

chr () {
	size = 0; s = 0;
}

~chr() {
	delete[] s;
	size = 0;
}

void set(jschar *a, int length = -1) { // can be used as alloc if a=0
	if (length == -1) length = len(a);
	s = new jschar[length];
	size = length;
	if (a) for (int x = 0; x < size; x++) s[x] = *a++;
}

void set_wchar(wchar_t *a, int length = -1) {
	if (length == -1) length = len(a);
	s = new jschar[length];
	size = length;
	if (a) for (int x = 0; x < size; x++) s[x] = *a++;
}

void setUtf(char *a, int length = -1) {
	if (length == -1) length = len(a);
	jschar *d = new jschar[length];
	size = utf2w((unsigned char*)a, (unsigned char*)&a[length], d, &d[length]);
	set(d, size);
	delete[] d;
}

void setAscii(char *a, int length = -1) {
	if (length == -1) length = len(a);
	s = new jschar[length];
	size = length;
	for (int x = 0; x < size; x++) s[x] = (char)(*a++);
}

long intToStr(long i, char *s) {
	char *c = s;
	long n = 0;
	while (true) {
		unsigned long i10 = i / 10;
		*s = (i - (i10 * 10)) + '0';
		i = i10;
		s++;
		n++;
		if (i == 0) { *s-- = 0; break; }
	}
	char *b = s;
	while (b > c) {
		char tmp = *b;
		*b-- = *c;
		*c++ = tmp;
	}
	return n; 
}

void dblToStr (double d, char *s) {
	if (d < 0) d = -d, s[0] = '-', s++;
	long n = d;
	long w = intToStr(n, s);
	s += w;
	d = d - n;
	long q = d * 100000;
	d *= 100000000;
	{ // fix
		q = d - q * 1000;
		if (q == 999||q == 998) d += 0.00001;
	}
	s[0] = '.'; s++;
	w = intToStr(d, s);
	while (true) {
		if (s[w-1] != '0') {
			if (s[w-1] == '.') w--;
			break;
		}
		w--;
	}
	s[w] = 0;
}

void set(double i) {
	char s[150] = {0};
	dblToStr(i, s);
	setAscii((char*)s);
}

double toNumber () {
	bool minus = false;
	jschar *s = this->s;
	if (*s == '-') { s++; minus = true; };
	char *p = getAscii();
	double A = 0, B = 0, *C = &A, n = 0;
	while (true) {
		if (*s >= '0' && *s <= '9') {
			*C = (*C * 10) + (*s - '0');
			s++;
		} else if (*s == '.') {
			C = &B;
			n = 0;
			s++;
			continue;
		} else break;
		if (C == &B) n++;
	}
	while (n-- > 0) B /= 10;
	double R = A + B;
	if (minus) R = -R;
	return R;
}

char * getAscii(int *returnSize = 0) { // basic 0-128 range only
	char* u = new char[size+1];
	for (int i = 0; i < size; i++) u[i] = s[i];
	u[size] = 0;
	if (returnSize) *returnSize = size;
	return u;
}

char * getUtf(int *returnSize = 0) {
	char* u = new char[size*4];
	int utfSize = w2utf(u, size*4, s, size);
	u[utfSize] = 0;
	if (returnSize) *returnSize = utfSize;
	return u;
}

int cmp (const chr &other) {
	if (size == 0 && other.size == 0) return 0;
	if (size == 0) return -1;
	if (other.size == 0) return 1;
	jschar *a, *b, *ea, *eb;
	a = s; b = other.s;
	ea = a + size;
	eb = b + other.size;
	while (true) {
		jschar A = *a, B = *b;
		if (A != B) {
			if (A < B) return -1;
			return 1;
		}
		a++;
		b++;
		if (a == ea) break;
		if (b == eb) break;
	}
	if (size < other.size) return -1;
	if (size > other.size) return 1;
	return 0;
}

jschar operator [](int i) {
	if (i < 0) i = size + i;
	return s[i];
}

int find (int start, jschar *c, int subsize) {
	if (subsize == 0) return -1;
	int e = subsize;
	int l = start + e;
	int i = l - 1;
	while (i < size)
		if (s[i] != c[e - 1]) i++;
		else {
			int x = e;
			jschar    *a = &s[i], *b = &c[e - 1];
			while (x--)
			{
				if (*a != *b) { i++; break; }
				if (x > 0) { a--; b--; }
			}
			if (x < 0) return i - e + 1;
		}
	return -1;
}

chr substr(int pos, int count = -1) {
	chr R;
	if (count < 0) count = size;
	if (count > size - pos) count = size - pos;
	R.set(&s[pos], count);
	return R;
}

int _strcount(const chr &substring) {
	int i = 0;
	int result = 0;
	while (true) {
		i = find(i, substring.s, substring.size);
		if (i < 0) return result;
		result ++;
		i += substring.size;
	}
}

void _cpto(int from, const chr &dest, int to, int count) {
	jschar *a = s + from;
	jschar *b = dest.s + to;
	while (count-- > 0) *b++ = *a++;
}

chr clone() {
	chr R;
	R.set(s, size);
	return R;
}

void replace(chr &A, chr &B, chr &dest) {
//	chr dest;
	int subcount, len, si=0, di, x;
	if (size == 0) return;
	dest.size = size + _strcount(A) * (B.size - A.size);
	dest.s = new jschar [dest.size];
	di = si;
	_cpto(0, dest, 0, si);
	while (true) {
		x = find(si, A.s, A.size);
		if (x < 0) {
			len = size - si;
			if (len > 0) _cpto(si, dest, di, len);
			return;
		}
		else len = x - si;
		if (len > 0) _cpto(si, dest, di, len);
		di += len;
		if (B.size > 0) B._cpto(0, dest, di, B.size);
		si = x + A.size;
		di += B.size;
	}
}


}; // struct chr {}

