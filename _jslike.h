// This is amalgamated version of jslike.h.
// you can just copy this single file to your project and include.

#ifndef __JSLIKE_H__
#define __JSLIKE_H__

namespace jslike {

#define self (*this)

int len(char *c) {
	int R = 0;
	if (c == 0) return 0;
	while (*c++) R++;
	return R;
}

int len(wchar_t *c) {
	int R = 0;
	if (c == 0) return 0;
	while (*c++) R++;
	return R;
}

char to_ascii(wchar_t w) {
	char c;
	char a = 0xE0; // rusA
	if (w >= 0x0410)
	c = (char) w - (0x0410 - a);
	else if (w == 0x0451) c = 184;
	else if (w == 0x2014) c = 0x97;
	else c = w & 0xff;
	return c;
}

wchar_t up_char(wchar_t c) {
	wchar_t A = 1040, a = 1072, YA = 1071, ya = 1103;
	if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
	if (c >= a && c <= ya) c -= a - A;
	return c;
}

wchar_t low_char(wchar_t c) {
	wchar_t A = 1040, a = 1072, YA = 1071, ya = 1103;
	if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
	if (c >= A && c <= YA) c += a - A;
	return c;
}


wchar_t UTF = 0xFEFF;

int utf_offsets[6] = { 0x0UL, 0x3080UL, 0xE2080UL, 0x3C82080UL, 0xFA082080UL, 0x82082080UL };

unsigned char utf_trail(int i)
{
	if (i < 192) return 0;
   if (i < 224) return 1;
   if (i < 240) return 2;
   if (i < 248) return 3;
   if (i < 252) return 4;
	return 5;
}

int utf2w (unsigned char* src, unsigned char* SB, wchar_t* dst, wchar_t* TB)
{
	 unsigned char* S = src;
	 wchar_t* T = dst;
	 while (S < SB)
	 {
	int ch = 0;
	wchar_t X = utf_trail(*S);
	if (S + X >= SB) break;
		  int x = X;
		  while (x-- > 0) ch += *S++, ch <<= 6;
		  ch += *S++;
	ch -= utf_offsets[X];
	if (T >= TB) { S -= (X+1); break; }
	if (ch <= 0xFFFF)
		  {
		 if (ch >= 0xD800 && ch <= 0xDFFF) *T++ = 0xFFFD;
				else *T++ = (wchar_t)ch;
	}
		  else if (ch > 0x0010FFFF) *T++ = 0xFFFD;
	else
		  {
		 if (T + 1 >= TB) { S -= (X+1); break; }
	    ch -= 0x0010000UL;
	    *T++ = (wchar_t)((ch >> 10) + 0xD800);
		 *T++ = (wchar_t)((ch & 0x3FFUL) + 0xDC00);
	}
    }
    return T - dst;
}

int w2utf(char *D, int DD, wchar_t *S, int SS)
{
  int i = 0, n = 0, c;
  if (S == 0) return 0;
  if (D != 0)
  {
    while ((i < SS) && (n < DD))
    {
      c = S[i++];
      if (c <= 0x7F) D[n++] = c;
      else if (c > 0x7FF)
      {
        if (n + 3 > DD) break;
        D[n] = 0xE0 | (c >> 12);
        D[n + 1] = 0x80 | ((c >> 6) & 0x3F);
        D[n + 2] = 0x80 | (c & 0x3F);
        n += 3;
      }
      else
      {
        if (n + 2 > DD) break;
        D[n] = 0xC0 | (c >> 6);
        D[n + 1] = 0x80 | (c & 0x3F);
        n += 2;
      }
    }
    //if (n > DD) n = DD - 1;
    D[n] = 0;
  }
  else
  {
    while (i < SS)
    {
      c = S[i++];
      if (c > 0x7F) { if (c > 0x7FF) n++; n++; }
      n++;
    }
  }
  return n;
}


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
	wchar_t *a = s + from;
	wchar_t *b = dest.s + to;
	while (count-- > 0) *b++ = *a++;
}

chr clone() {
	chr R;
	R.set(s, size);
	return R;
}

chr replace(chr &A, chr &B) {
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

#define constructor

//TODO: Garbage collector (mark/sweep)
//functions

enum varType { varIgnore = -2, varNull=-1, varNum=0, varStr=1, varArr=2, varObj=3, varFunc=4, varBool=5 };
enum varSyntax { argIgnore, undefined, arr, obj, NaN, end };

struct Ref {
	int uses;
	void *data;
	Ref () {
		uses = 1;
	}
};

void *newLst();
void *newObj();

struct var;// void log(var a);
struct var {
	varType type;
	union {
		double n;
		Ref* ref;
	};
constructor var () {
	type = varNull; ref = 0;
}

~var () { unref(); }

constructor var (const var& a) {
	type = varNull; ref = 0;
	copy(a);
}

constructor var (const varSyntax syntax) {
	//type = varNull; //not needed because ref=0 means no unref()
	ref = 0;
	self = syntax;
}
void operator = (const varSyntax syntax) {
	if (ref) unref();
	if (syntax == arr) {
		type = varArr;
		ref = new Ref;
		ref->data = newLst();
	} else if (syntax == obj) {
		type = varObj;
		ref = new Ref;
		ref->data = newObj();
	} else if (syntax == argIgnore) {
		type = varIgnore;
	} else {
		type = varNull;
	}
		//TODO: other types (and wrong values like NaN, undefined)
}

void operator = (const var &a) {
	unref();
	copy(a);
}

constructor var (int a) {
	self = (double) a;
}

constructor var (bool a) {
	type = varBool;
	ref = 0;
	n = (double)a;
}

void operator = (int a) { self = (var)a;}

void operator = (bool a) { self = (var)a;}

void operator = (double a) {
	n = a;
	type = varNum;
}
constructor var (char* a) {
	ref = 0;
	self = a;
}

constructor var (wchar_t* a) {
	ref = 0;
	self = a;
}

constructor var (double a) {
	self = a;
}

void operator = (char* a) {
	if (ref) unref();
	makeStringToSet();
	_chr().setUtf(a);
	//_chr().setAscii(a);
}

void operator = (wchar_t* a) {
	if (ref) unref();
	makeStringToSet();
	_chr().set(a);
}

void copy(const var &a);

void unref() {
	if (type == varNum || type == varNull || type == varBool) return;
	else if (type == varStr) {
		ref->uses--;
		if (ref->uses == 0) {
			delete (chr*)ref->data, delete ref;
		}
	}
	else if (type == varArr) {
		ref->uses--;
		if (ref->uses == 0) {
			deleteLst();
		}
	}
	else if (type == varObj) {
		ref->uses--;
		if (ref->uses == 0) {
			deleteObj();
		}
	}
	type = varNull;
	ref = 0;
}
	

	chr & _chr() { return *((chr*)(ref->data)); }
	
	void makeStringToSet() {
		// use to set some value from wchar_t* to a new var
		ref = new Ref;
		type = varStr;
		ref->data = new chr;
	}
	
	wchar_t* getStringPointer() {
		return _chr().s;
	}

	char* getStringAllocUtf() {
		return _chr().getUtf();
	}

	char* getStringAllocAscii() {
		return _chr().getAscii();
	}
	
	double toDouble() {
		if (type == varNum) return n;
		return 0;
	}
	
	int toInt() {
		if (type == varNum) return n;
		return 0;
	}

	bool toBool() {
		if (type == varBool) return n;
		return false;
	}
	
	var toNumber() {
		return _chr().toNumber();
	}

	static var fromCharCode(var a) {
		var R = "1";
		R.getStringPointer()[0] = (wchar_t)a.toInt();
		return R;
	}

	var toString() {
		if (type == varNull) {
			return (var)"undefined";
		}
		if (type == varNum) {
			var R;
			R.type = varStr;
			R.ref = new Ref;
			chr *c = new chr;
			c->set(n);
			R.ref->data = c;
			return R;
		}
		if (type == varBool) {
			if (n) return "true";
			return "false";
		}
		if (type == varStr) {
			var R = self;
			return R;
		}
		if (type == varArr) {
			return join(", ");
		}
		if (type == varObj) {
			return "[object]";
		}
		return (var)"undefined";
	}
	
	var &operator [] (var a);

	var Push(var a);
	var Pop();

void operator ++ (int a) {
	//if (type == varNum) 
	n++;
//	else self = NaN;
}
void operator -- (int a) {
	//if (type == varNum) 
	n--;
//	else self = NaN;
}

void operator += (var a) {
	if (type == varNum && a.type == varNum) n += a.n;
	else if (type == varStr) {
		a = a.toString();
		self = self + a;
	} else
		self = NaN;
}

var operator + (var a) {
	if (type == varNum) {
		if (a.type == varNum) {
			return toDouble() + a.toDouble();
		} else if (a.type == varStr) {
			return toString() + a.toString();
		} else {
			var R = NaN;
			return R;
		}
	} else if (type = varStr) {
		if (a.type != varStr) a = a.toString();
		int an = _chr().size, bn = a._chr().size;
		wchar_t 
			* sum = new wchar_t [an + bn],
			* c = sum,
			* A = _chr().s,
			* B = a._chr().s;
		for (int i = 0; i < an; i++) *c++ = A[i];
		for (int i = 0; i < bn; i++) *c++ = B[i];
		var R;
		R.makeStringToSet();
		chr &C = R._chr();
		C.size = an + bn;
		C.s = sum;
		return R;
	}
}

#define STD_OP(X, op, Y) var operator op (var a) {\
	if (type == varNum) {\
		if (a.type == varNum) {\
			return X op Y;\
		} else return NaN;\
	}\
	return undefined;\
}
//fast version
STD_OP(n, -, a.n)
STD_OP(n, *, a.n)
STD_OP(n, /, a.n)
STD_OP((int)n, |, (int)a.n)
STD_OP((int)n, &, (int)a.n)
STD_OP(n, ||, a.n)
STD_OP(n, &&, a.n)
STD_OP((int)n, %, (int)a.n)
STD_OP(n, *=, a.n)
STD_OP(n, /=, a.n)
STD_OP(n, -=, a.n)

/*
//safe version
STD_OP(toDouble(), -, a.toDouble())
STD_OP(toDouble(), *, a.toDouble())
STD_OP(toDouble(), /, a.toDouble())
STD_OP(toInt(), |, a.toInt())
STD_OP(toInt(), &, a.toInt())
STD_OP(toDouble(), ||, a.toDouble())
STD_OP(toDouble(), &&, a.toDouble())
STD_OP(toInt(), %, a.toInt())
STD_OP(n, *=, a.toDouble())
STD_OP(n, /=, a.toDouble())
STD_OP(n, -=, a.toDouble())
*/
#undef STD_OP

var round() {
	if (type == varNum) {
		return (int) toDouble();
	}
	return undefined;
}	

	bool has(var name) {
		if (type == varObj) {
			return (self[name].type != varNull);
		}
		else return false;
	}
	
bool operator == (varSyntax b) {
	if (b == undefined) {
		return (type == varNull || type == varIgnore);
	}
	if (b == NaN) {
		return (type != varNum);
	}
	return false;
}

bool operator != (varSyntax b) {
	return ! (self == b);
}


	// decls:
	var charAt(int n);
	var charCodeAt(int n);
	var split(var separator);
	var objectKeys();
	var indexOf(var a);
	var& getArrElement(int n);
	var push(var a);
	var pop();
	var length();
	var splice(var start, var deleteCount, var item);
	var slice(var start, var end);
	var join(var separator);
	void deleteLst();
	// obj
	var& getObjElement(const var &n);
	void deleteObj();
	var typeOf();
struct Rc {
	var *ref;
	int count;
};

struct arrset {
	Rc *rc;
	var *vr;

	arrset () {
		rc = new Rc;
		rc->ref = new var;
		rc->count = 1;
	}
	arrset (var *a, int dummy) {
		//dummy is to remove "ambiguous" conflict
		rc = 0;
		vr = a;
	}
	arrset(arrset &a) {
		if (a.rc) {
			rc = a.rc;
			rc->count++;
		}
		else vr = a.vr, rc = 0;
	}
	~arrset() {
		if (rc && rc->ref && --rc->count == 0) {
			delete rc->ref;
			delete rc;
		}
	}
	var *get() {
		if (rc) return rc->ref;
		return vr;
	}
	operator var& (){
		return *get();
	}
	arrset &operator , (var b) {
		if (get()->type == varArr) get()->push(b);
		return self;
	}
	arrset &operator = (var b) {
		(*get()) = arr;
		get()->push(b);
		return self;
	}
};

arrset Arr() {
	arrset R(this, 1);
	return R;
}

struct objset {
	Rc *rc;
	var *vr;

	objset () {
		rc = new Rc;
		rc->ref = new var;
		rc->count = 1;
	}
	objset (var *a) {
		rc = 0;
		vr = a;
	}
	objset(objset &a) {
		if (a.rc) {
			rc = a.rc;
			rc->count++;
		}
		else vr = a.vr, rc = 0;
	}
	~objset() {
		if (rc && rc->ref && --rc->count == 0) {
			delete rc->ref;
			delete rc;
		}
	}
	var *get() {
		if (rc) return rc->ref;
		return vr;
	}
	void set(var *a) {
		if (rc) rc->ref = a;
		vr = a;
	}
	operator var& (){
		return *get();
	}

	chr *key;
	stk stack;

	var& __value() {
		var t;
		t.makeStringToSet();
		t._chr().set(key->s, key->size);
		return (*get())[t];
	}
	objset &operator , (varSyntax b) {
		if (b == obj) {
			__value() = obj;
			stack.push(get());
			set(& __value());
			delete key;
			key = 0;
		} else if (b == end) {
			set((var*) stack.pop());
		}
		return self;
	}
	objset &operator , (var b) {
		if (get()->type == varObj) {
			if (key) {
				__value() = b;
				delete key;
				key = 0;
			} else {
				key = new chr;
				b = b.toString();
				key->set(b._chr().s, b._chr().size);
			}
		}
		return self;
	}
	objset &operator = (var b) {
		(*get()) = obj;
		key = new chr;
		b = b.toString();
		key->set(b._chr().s, b._chr().size);
		return self;
	}
};

objset Obj() {
	objset R(this);
	return R;
}




	static objset initObj() {
		objset R;
		return R;
	}

	static arrset initArr() {
		arrset R;
		return R;
	}

	void del(var key);
//--on class 8fa
};
//--off class

extern "C" void exit(int);

var & Undefined() {
	static var R;
	var none;
	R = none;
	return R;
}

void logVar(var a) {
	var z;
	z = a.toString();
	printf("%s ", z._chr().getUtf());
}

void __Log(var a) {
	int cnt = a.length().toInt();
	for (int i = 0; i < cnt; i++) {
		logVar(a[i]);
	}
	printf("\n");
	fflush(0);
}

#ifdef __BORLANDC__
// Borland C++ Buider 6.0 does not support __VA_ARGS__
void log(var a,
	var b = argIgnore, var c = argIgnore, var d = argIgnore,
	var e = argIgnore, var f = argIgnore, var g = argIgnore,
	var h = argIgnore, var i = argIgnore, var j = argIgnore,
	var k = argIgnore, var l = argIgnore, var m = argIgnore,
	var n = argIgnore, var o = argIgnore, var p = argIgnore,
	var q = argIgnore, var r = argIgnore, var s = argIgnore,
	var t = argIgnore, var u = argIgnore, var v = argIgnore,
	var w = argIgnore, var x = argIgnore, var y = argIgnore,
	var z = argIgnore, var A = argIgnore, var B = argIgnore
	)
{
	logVar(a);	
	if (b.type != varIgnore) logVar(b);
	if (c.type != varIgnore) logVar(c);
	if (d.type != varIgnore) logVar(d);
	if (e.type != varIgnore) logVar(e);
	if (f.type != varIgnore) logVar(f);
	if (g.type != varIgnore) logVar(g);
	if (h.type != varIgnore) logVar(h);
	if (i.type != varIgnore) logVar(i);
	if (j.type != varIgnore) logVar(j);
	if (k.type != varIgnore) logVar(k);
	if (l.type != varIgnore) logVar(l);
	if (m.type != varIgnore) logVar(m);
	if (n.type != varIgnore) logVar(n);
	if (o.type != varIgnore) logVar(o);
	if (p.type != varIgnore) logVar(p);
	if (q.type != varIgnore) logVar(q);
	if (r.type != varIgnore) logVar(r);
	if (s.type != varIgnore) logVar(s);
	if (t.type != varIgnore) logVar(t);
	if (u.type != varIgnore) logVar(u);
	if (v.type != varIgnore) logVar(v);
	if (w.type != varIgnore) logVar(w);
	if (x.type != varIgnore) logVar(x);
	if (y.type != varIgnore) logVar(y);
	if (z.type != varIgnore) logVar(z);
	if (A.type != varIgnore) logVar(A);
	if (B.type != varIgnore) logVar(B);
	printf("\n");
	fflush(0);
}


#else

#define ARR(...) (var::initArr() = __VA_ARGS__)
#define OBJ(...) (var::initObj() = __VA_ARGS__)
#define log(...) __Log((var::initArr() = __VA_ARGS__));

#endif

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

void *newLst() {
	return new lst;
}

void var::deleteLst() {
	delete (lst*)ref->data, delete ref;
}

var var::splice(var start, var deleteCount, var item = argIgnore) {
	var R = arr;
	lst &L = *(lst*) ref->data;
	int
		x = start.toDouble(),
		d = deleteCount.toDouble(),
		size = length().toDouble();
	if (d < 0) d = 0;
	if (x >= size) return R;
	if (x + d > size) d = size - x;
	for (int i = x; i < x + d; i++) R.push(L[i]);
	int N = 1;
	if (item.type == varIgnore) N = 0;
	L.delIns(x, d, &item, N);
	return R;
}

var var::join(var separator) {
	var R;
	lst L;
	int cnt = length().toDouble();
	if (cnt == 0) {
		return (var)"";
	}
	int all = 0;
	for (int i = 0; i < cnt; i++) {
		var t = self[i].toString();
		all += t.length().toDouble();
		L.push(t);
	}
	int seplen = (int)separator.length().toDouble();
	int Rsize = all + (cnt-1) * seplen;

	R.makeStringToSet();
	R._chr().set(0, Rsize);
	int x = 0;
	for (int i = 0; i < cnt; i++) {
		int subcnt = (int)L[i].length().toDouble();
		L[i]._chr()._cpto(0, R._chr(), x, subcnt);
		x += subcnt;
		if (i == cnt - 1) break;
		separator._chr()._cpto(0, R._chr(), x, seplen);
		x += seplen;
	}
	return R;
}

var var::slice(var start, var end = undefined) {
	// needs more testing
	int 
		size = length().toDouble(),
		a = start.toDouble(),
		b = end.toDouble();

	if (end == undefined) b = size;
		else if (b < 0) b += size;
		else if (b > size) b = size;
		
	if (a < 0) a += size;

	if (type == varStr) {
		var R;
		R.makeStringToSet();
		R._chr().set(&_chr().s[a], b-a);
		return R;
	}
	if (type != varArr) return undefined;
	var R = arr;
	if (b <= a) { return R; }
	lst &L = * (lst*) R.ref->data;
	
	L.resize(b - a);
	for (int i = 0; i < L.size; i++) L[i] = self[a + i];
	return R;
}

var var::push(var a) {
	lst *L = (lst*) ref->data;
	L->push(a);
	return a;
}

var var::pop() {
	lst *L = (lst*) ref->data;
	return L->pop();
}

var var::Push(var a) {
	lst *L = (lst*) ref->data;
	L->delIns(0, 0, &a, 1);
	return a;
}

var var::Pop() {
	lst *L = (lst*) ref->data;
	var R = (*L)[0];
	L->delIns(0, 1, 0, 0);
	return R;
}

var var::length() {
	if (type == varArr) {
		lst *L = (lst*) ref->data;
		return L->size;
	}
	if (type == varStr) {
		return _chr().size;
	}
	return undefined;
}

var& var::getArrElement(int n) {
	lst *L = (lst*) ref->data;
	return (*L)[n];
}

bool operator == (var a, var b) {
	if (a.type == varNum && b.type == varNum) {
		return a.n == b.n;
	}
	if (a.type == varStr && b.type == varStr) {
		chr &A = a._chr();
		chr &B = b._chr();
		return a._chr().cmp(b._chr()) == 0;
	}
	return false;
}

bool operator != (var a, var b) {
	return !(a == b);
}

bool operator < (var a, var b) {
	if (a.type == varNum && b.type == varNum) {
		return a.n < b.n;
	}
	if (a.type == varStr && b.type == varStr) {
		chr &A = a._chr();
		chr &B = b._chr();
		return a._chr().cmp(b._chr()) < 0;
	}
	return false;
}

bool operator > (var a, var b) {
	if (a == b) return false;
	if (a < b) return false;
	return true;
}


struct keyval {
	var keys, vals;
	keyval () {
		keys = arr;
		vals = arr;
	}
	void set(var key, var val) {
		key = key.toString();
		int k = keys.indexOf(key).toDouble();
		if (k < 0) {
			keys.push(key);
			vals.push(val);
		} else {
			vals[k] = val;
		}
	}
	var &get(var key) {
		key = key.toString();
		int k = keys.indexOf(key).toDouble();
		if (k < 0) {
			k = keys.length().toDouble();
			keys[k] = key;
		}
		return vals[k];
	}
};


void *newObj() {
	return new keyval;
}

var & var::getObjElement(const var &n) {
	keyval *u = (keyval*) ref->data;
	var &R = u->get(n);
	return R;
}

var var::objectKeys() {
	if (type != varObj) return undefined;
	keyval *u = (keyval*) ref->data;
	return u->keys;
}

void var::deleteObj() {
	delete (keyval*)ref->data, delete ref;
}

void var::del(var key) {
	if (type != varObj) return;
	keyval *u = (keyval*) ref->data;
	key = key.toString();
	int k = u->keys.indexOf(key).toDouble();
	if (k >= 0) {
		u->keys.splice(k, 1);
		u->vals.splice(k, 1);
	}
}

//--on [] eef
var &var::operator [] (var a) {
	if (type == varNull || type == varNum) { 
		log((var)"FATAL ERROR: Cannot access property \""
			+ a +"\" of non-Array \"" + self + "\"");
//		log("The program will now terminate.");
		exit(0);
		static var R; 
		return R;
	}
	else if (type == varStr) {
		int n = a.toDouble();
		chr &C = _chr();
		if (n >= C.size) {
			return Undefined();
		}
		wchar_t c = C.s[n];
		static var R = "1";
		R._chr().s[0] = c;
		return R;
	}
	else if (type == varArr) {
		int n = a.toDouble();
		return getArrElement(n);
	}
	else if (type == varObj) {
		return getObjElement(a);
	} else {
		printf("unknown type\n");
	}
	static var R;
	R = undefined;
	return R;
}
//--off []

var var::charAt(int n) {
	if (type != varStr) {
		static var R;
		return R;
	}
	chr &C = _chr();
	if (n >= C.size) return Undefined();
	wchar_t c = C.s[n];
	var R = "1";
	R._chr().s[0] = c;
	return R;
}

var var::charCodeAt(int n) {
	if (type != varStr) {
		static var R;
		return R;
	}
	chr &C = _chr();
	if (n >= C.size) return Undefined();
	wchar_t c = C.s[n];
	var R = (int)c;
	return R;
}



bool startNum(var s) {
	wchar_t C = s.getStringPointer()[0];
	if ((C >= '0' && C <= '9') || C == '-' || C == '.') return true;
	return false;
}

var parseNum(var &s, int &i) {
	int size = s.length().toInt();
	var R = "";
	bool minus = false;
	bool dot = false;
	wchar_t *CH = s.getStringPointer();
	if (CH[i] == '-') minus = true, i++;
	while (i < size) {
		wchar_t C = CH[i];
		if (C >= '0' && C <= '9') {
			R += var::fromCharCode(C);
		} else if (C == '.') {
			if (dot) break;
			R += var::fromCharCode(C);
			dot = true;
		} else break;
		i++;
	}
	return R.toNumber();
}

var parseString(var &s, int &i) {
	int size = s.length().toInt();
	var q = s.charAt(i);
	if (q != "'" && q != "\"") return undefined;
	i++;
	var str = "";
	while (i < size) {
		if (s[i] == q) break;
		str += s[i];
		i++;
	}
	i++;
	return str;
}

bool isAlpha(wchar_t c) {
	if (c >= 'a' && c <= 'z') return true;
	if (c >= 'A' && c <= 'Z') return true;
	if (c > 128) return true; // really dirty hack
	return false;
}

var parseId(var &s, int &i) {
	int size = s.length().toInt();
	var str = "";
	wchar_t *C = s.getStringPointer();
	while (i < size) {
		if (!isAlpha(C[i])) break;
		str += s[i];
		i++;
	}
	return str;
}

void skipSpaces(var &s, int &i) {
	int size = s.length().toInt();
	wchar_t *S = s.getStringPointer();
	while (i < size && S[i] <= 32) {
		i++;
	}
}

bool parseSingleCharOp(wchar_t op, var &s, int &i) {
	int size = s.length().toInt();
	wchar_t *S = s.getStringPointer();
	while (i < size) {
		wchar_t C = S[i];
		if (C == op) { i++; return true; }
		if (C == ' ' || C == '\t' || C == '\r' || C == '\n') {
			i++;
			continue;
		}
		break;
	}
	return false;
}

var parseJsonObject(var &s, int &i);

var parseArray(var &s, int &i) {
	var R = arr;
	int size = s.length().toInt();
	i++;
	while (i < size) {
		skipSpaces(s, i);
		if (s[i] == "]") break;
		var O = parseJsonObject(s, i);
		R.push(O);
		bool comma = parseSingleCharOp(',', s, i);
		if (!comma) {
			bool end = parseSingleCharOp(']', s, i);
			if (end) break;
			log("fatal: JSON parse Array, comma expected", s.slice(i-10, i), "^");
			exit(1);
			break;
		}
	}
	return R;
}

var parseObject(var &s, int &i) {
	var R = obj;
	int size = s.length().toInt();
	i++;
	int max = 10;
	while (i < size) {
		skipSpaces(s, i);
		if (s[i] == "}") break;
		var K = parseString(s, i);
		if (K == undefined) {
			K = parseId(s, i);
			if (K == undefined) {
				log("fatal: JSON parse error");
				exit(1);
				return undefined;
			}
		}
		bool colon = parseSingleCharOp(':', s, i);
		if (!colon) break;
		skipSpaces(s, i);
		var V = parseJsonObject(s, i);
		R[K] = V;
		bool comma = parseSingleCharOp(',', s, i);
		if (!comma) break;
		if (max-- <= 0) break;
	}
	return R;
}

var parseJsonObject(var &s, int &i) {
	var R;
	var *C = &R;
	int size = s.length().toInt();
	while (i < size) {
		var q = s.charAt(i);
		if (startNum(q)) {
			return parseNum(s, i);
		}
		if (q == "[") { 
			R = arr; 
			log("parseArray");
			return parseArray(s, i);
		}
		if (q == "{") {
			R = obj;
			return parseObject(s, i);
		}
		if (q == "\"" || q == "'") { 
			return parseString(s, i);
		}
		return undefined;
		i++;
	}
}

var quoteIfNeeded(var a) {
	wchar_t *s = a.getStringPointer();
	int size = a.length().toInt();
	for (int i = 0; i < size; i++) {
		if (!isAlpha(s[i])) return (var)"\"" + a + "\"";
	}
	return a;
}

struct classJSON {

	var stringifyObj(var a) {
		var keys = a.objectKeys();
		var R = arr;
		for (var i = 0; i < keys.length(); i++) {
			var &ref = a[keys[i]];
			var q = "";
			if (ref.type == varStr) q = "\"";
			var s;
			s = stringify(ref);
			R.push( quoteIfNeeded(keys[i]) + ": " + q + s + q );
		}
		return (var) "{ " + R.join(", ") + " }";
	}
	
	var stringifyArr(var a) {
		int cnt = a.length().toDouble();
		var R = arr;
		for (var i = 0; i < cnt; i++) {
			var &ref = a[i];
			var q = "";
			if (ref.type == varStr) q = "\"";
			var s;
			s = stringify(ref);
			R.push( q + s + q );
		}
		return (var) "[ " + R.join(", ") + " ]";
	}
	
	var stringify(var a) {
		if (a.type == varArr) return stringifyArr(a);
		if (a.type == varObj) return stringifyObj(a);
		return a.toString();
	}
	
	var parse(var a) {
		int i = 0;
		var R = parseJsonObject(a, i);
		return R;
	}
	
} JSON;



var typeName(varType a) {
	if (a == varNum) return "number";
	if (a == varStr) return "string";
	if (a == varArr) return "array";
	if (a == varNull) return "undefined";
	if (a == varObj) return "object";
	return "unsure";
}

var var::typeOf() {
	var R;
	R = typeName(type);
	return R;
}

var var::indexOf(var a) {
	if (type == varArr) {
		int cnt = length().toDouble();
		for (int i = 0; i < cnt; i++) {
			if (self[i] == a) return i;
		}
	}
	else if (type == varStr) {
		chr &C = _chr();
		a = a.toString();
		chr &A = a._chr();
		return C.find(0, A.s, A.size);
	}
	return -1;
}

void var::copy(const var &a) {
	// at this point this object is empty
	type = a.type;
	if (type == varNum || type == varBool) n = a.n;
	else {
		if (a.type == varNull) { ref = 0; return; }
		ref = a.ref;
		chr &c = (*(var*)&a)._chr();
		if (ref) ref->uses++;
	}
}

var splitEveryChar(var a) {
	var R = arr;
	int len = a.length().toDouble();
	for (int i = 0; i < len; i++) R.push(a.charAt(i));
	return R;
}

var var::split(var separator) {
	separator = separator.toString();
	self = self.toString();
	if (separator.length() == 0) return splitEveryChar(self);
	if (self.length() == 0) return Undefined();
	chr
		&V = self._chr(),
		&D = separator._chr();
	int
		start = 0,
		i = 0,
		count = V._strcount(D) + 1;
	var R = arr;
	while (i < count) {
		int end = V.find(start, D.s, D.size);
		if (end < 0) end = V.size;
		var O;
		O.type = varStr;
		O.ref = new Ref;
		O.ref->data = new chr;
		O._chr().set(&V.s[start], end - start);
		R.push(O);
		start = end + D.size;
		i++;
	}
	return R;
}

}
#endif // __JSLIKE_H__
