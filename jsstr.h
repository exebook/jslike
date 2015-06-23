
struct chr {
	int size;
	wchar_t *s;

chr () {
	size = 0; s = 0;
}

~chr() {
	delete s;
	size = 0;
}

void set(wchar_t *a, int length = -1) { // can be used as resize if a≟0
	if (length == -1) length = len(a);
	s = new wchar_t[length];
	size = length;
	if (a) for (int x = 0; x < size; x++) s[x] = *a++;
}

void setUtf(char *a, int length = -1) {
	if (length == -1) length = len(a);
	wchar_t *d = new wchar_t[length];
	size = utf2w((unsigned char*)a, (unsigned char*)&a[length], d, &d[length]);
	set(d, size);
	delete d;
}

void setAscii(char *a, int length = -1) {
	if (length == -1) length = len(a);
	s = new wchar_t[length];
	size = length;
	for (int x = 0; x < size; x++) s[x] = to_ascii(*a++);
}

int intToStr(int i, char *s) {
   char *c = s;
   int n = 0;
	while (true) {
      unsigned int i10 = i / 10;
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
	int n = d;
	int w = intToStr(n, s);
	s += w;
	d = d - n;
	int q = d * 100000;
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
	bool minus;
	wchar_t *s = this->s;
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

char * getAscii() { // basic 0-128 range only
	static char* u = new char[size];
	for (int i = 0; i < size; i++) u[i] = s[i];
	return u;
}

char * getUtf() {
	static char* u = 0;
	if (u != 0) delete u;
	u = new char[size*4];
	w2utf(u, size*4, s, size);
	return u;
}

int cmp (const chr &other, bool caseInsensetive = false) {
	if (size == 0 && other.size == 0) return 0;
	if (size == 0) return -1;
	if (other.size == 0) return 1;
	wchar_t *a, *b, *ea, *eb;
	a = s; b = other.s;
	ea = a + size;
	eb = b + other.size;
	while (true) {
		wchar_t A = *a, B = *b;
		if (caseInsensetive) {
			A = up_char(A);
			B = up_char(B);
		}
		if (A != B) {
			if (A < B) return -1;
			return 1;
		}
		a++;
		b++;
		if (a == ea) break;
		if (b == eb) break;
	}
	if (size < size) return -1;
	if (size > size) return 1;
	return 0;
}

int operator ! () {
	return size;
}

wchar_t operator [](int i) {
	if (i < 0) i = size + i;
	return s[i];
}

int find (int start, wchar_t *c, int subsize) {
	if (subsize == 0) return -1;
	int e = subsize;
	int l = start + e;
	int i = l - 1;
	while (i < size)
		if (s[i] != c[e - 1]) i++;
		else {
			int x = e;
			wchar_t    *a = &s[i], *b = &c[e - 1];
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

void upper() {
	for (int x = 0; x < size; x++) s[x] = up_char(s[x]);
}

void lower() {
	for (int x = 0; x < size; x++) s[x] = low_char(s[x]);
}

int _strcount(chr substring) {
	int i = 0;
	int result = 0;
	while (true) {
		i = find(i, substring.s, !substring);
		if (i < 0) return result;
		result ++;
		i += !substring;
	}
}

void _cpto(int from, const chr &dest, int to, int count) {
	wchar_t *a = s + from;
	wchar_t *b = dest.s + to;
	while (count-- > 0) *b++ = *a++;
}

chr clone() {
	chr R;
	R.set(s, size);
	return R;
}

chr replace(chr A, chr B) {
	chr R;
	int subcount, len, si=0, di, x;
	if (size == 0) return R;
	R.size = size + _strcount(A) * (B.size - A.size);
	R.s = new wchar_t [R.size];
	di = si;
	_cpto(0, R, 0, si);
	while (true) {
		x = find(si, A.s, A.size);
		if (x < 0) {
			len = size - si;
			if (len > 0) _cpto(si, R, di, len);
			return R;
		}
		else len = x - si;
		if (len > 0) _cpto(si, R, di, len);
		di += len;
		if (B.size > 0) B._cpto(0, R, di, B.size);
		si = x + A.size;
		di += B.size;
	}
}

}; // struct chr {}

