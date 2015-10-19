// This is amalgamated version of jslike.h.
// you can just copy this single file to your project and include.

#ifndef __JSLIKE_H__
#define __JSLIKE_H__

namespace jslike {
typedef __uint16_t jschar;

#define self (*this)

int len(char *c) {
	int R = 0;
	if (c == 0) return 0;
	while (*c++) R++;
	return R;
}

int len(jschar *c) {
	int R = 0;
	if (c == 0) return 0;
	while (*c++) R++;
	return R;
}

jschar UTF = 0xFEFF;

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

int utf2w (unsigned char* src, unsigned char* SB, jschar* dst, jschar* TB)
{
	 unsigned char* S = src;
	 jschar* T = dst;
	 while (S < SB)
	 {
	int ch = 0;
	jschar X = utf_trail(*S);
	if (S + X >= SB) break;
		  int x = X;
		  while (x-- > 0) ch += *S++, ch <<= 6;
		  ch += *S++;
	ch -= utf_offsets[X];
	if (T >= TB) { S -= (X+1); break; }
	if (ch <= 0xFFFF)
		  {
		 if (ch >= 0xD800 && ch <= 0xDFFF) *T++ = 0xFFFD;
				else *T++ = (jschar)ch;
	}
		  else if (ch > 0x0010FFFF) *T++ = 0xFFFD;
	else
		  {
		 if (T + 1 >= TB) { S -= (X+1); break; }
	    ch -= 0x0010000UL;
	    *T++ = (jschar)((ch >> 10) + 0xD800);
		 *T++ = (jschar)((ch & 0x3FFUL) + 0xDC00);
	}
    }
    return T - dst;
}

int w2utf(char *D, int DD, jschar *S, int SS)
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
	jschar *s;

chr () {
	size = 0; s = 0;
}

~chr() {
	delete[] s;
	size = 0;
}

void set(jschar *a, int length = -1) { // can be used as resize if a≟0
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

enum varType { varIgnore = -2, varNull=-1, varNum=0, varStr=1, varArr=2, varObj=3, varFunc=4, varBool=5, varDeleted=6 };
enum varSyntax { argIgnore, undefined, Array, Object, NaN, end };

void *newLst();
void *newObj();

struct var;// void log(var a);
struct arrset;
struct objset;

struct Ref {
	int uses;
	void *data;
	Ref () {
		uses = 1;
	}
};

struct var {
	varType type;
	union {
		double num;
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
	if (syntax == Array) {
		type = varArr;
		ref = new Ref;
		ref->data = newLst();
	} else if (syntax == Object) {
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

constructor var (long a) {
	self = (double) a;
}

constructor var (bool a) {
	type = varBool;
	ref = 0;
	num = (double)a;
}

void operator = (int a) { self = (var)a;}

void operator = (bool a) { self = (var)a;}

void operator = (double a) {
	num = a;
	type = varNum;
}
constructor var (char* a) {
	ref = 0;
	self = a;
}
constructor var (const char* a) {
	ref = 0;
	self = (char*)a;
}

constructor var (jschar* a) {
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
//	printf("SET '%s'\n", a);
	//_chr().setAscii(a);
}

void setUtf (char* a, int size = -1) {
	if (ref) unref();
	makeStringToSet();
	_chr().setUtf(a, size);
}

void operator = (jschar* a) {
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
		// use to set some value from jschar* to a new var
		ref = new Ref;
		type = varStr;
		ref->data = new chr;
	}

	jschar* getStringPointer() {
		return _chr().s;
	}

	char* getStringAllocUtf(int *utfSize = 0) {
		if (type == varStr) {
			char *c =_chr().getUtf(utfSize);
			return c;
		}
		else return 0;
	}

	char* getStringAllocAscii(int *returnSize = 0) {
		if (type == varStr)
			return _chr().getAscii(returnSize);
		else return 0;
	}

	double toDouble() {
		return num; // returns garbage if not varNum
	}

	int toInt() {
		return num;
	}

	bool toBool() {
		if (type == varBool) return num;
		return false;
	}

	var toNumber() {
		return _chr().toNumber();
	}

	static var fromCharCode(var a) {
		var R = "1";
		R.getStringPointer()[0] = (jschar)a.toInt();
		return R;
	}

	var toString() {
		if (type == varStr) return self;
		if (type == varNull) {
			return (var)"undefined";
		}
		if (type == varNum) {
			var R;
			R.type = varStr;
			R.ref = new Ref;
			chr *c = new chr;
			c->set(num);
			R.ref->data = c;
			return R;
		}
		if (type == varBool) {
			if (num) return "true";
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
	if (type == varNum) num++;
}
void operator -- (int a) {
	if (type == varNum) num--;
}

void operator += (var a) {
	if (type == varNum && a.type == varNum) num += a.num;
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
	} else if (type == varStr) {
		if (a.type != varStr) a = a.toString();
		int an = _chr().size, bn = a._chr().size;
		jschar 
			* sum = new jschar [an + bn],
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
	return undefined;
}

#define STD_OP(X, op, Y) var operator op (var a) {\
	if (type == varNum) {\
		if (a.type == varNum) {\
			return X op Y;\
		} else return NaN;\
	}\
	return undefined;\
}

STD_OP(toDouble(), -, a.toDouble())
STD_OP(toDouble(), *, a.toDouble())
STD_OP(toDouble(), /, a.toDouble())
STD_OP(toInt(), |, a.toInt())
STD_OP(toInt(), &, a.toInt())
STD_OP(toDouble(), ||, a.toDouble())
STD_OP(toDouble(), &&, a.toDouble())
STD_OP(toInt(), %, a.toInt())
STD_OP(num, *=, a.toDouble())
STD_OP(num, /=, a.toDouble())
STD_OP(num, -=, a.toDouble())

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

void setAscii (char* a) {
	if (ref) unref();
	makeStringToSet();
	_chr().setAscii(a);
}

	// decls:
	var replace(var find, var repl);
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
	var concat(var a);
	var concatAll();

//	arrset Arr();
//	objset Obj();
	static objset obj();
	static arrset arr();

	void del(var key);
//--on class 8fa
};
//--off class

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
		(*get()) = Array;
		get()->push(b);
		return self;
	}
};

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
		if (b == Object) {
			__value() = Object;
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
		(*get()) = Object;
		key = new chr;
		b = b.toString();
		key->set(b._chr().s, b._chr().size);
		return self;
	}
};

objset var::obj() {
	objset R;
	return R;
}

arrset var::arr() {
	arrset R;
	return R;
}

//arrset var::Arr() {
//	arrset R(this, 1);
//	return R;
//}
//
//objset var::Obj() {
//	objset R(this);
//	return R;
//}

//objset (varSyntax) {
	
//}


//maybe that would be better than var::initObj() = 1,2,3,4
//                              var a = lit(arr) = 1,2,3,4;
//var a = lit(obj) = "a",1,"b",2,"c",3,"d",4;


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
	char *c = z._chr().getUtf();
	printf("%s ", c);
	delete[] c;
}

void __Log(var a) {
	int cnt = a.length().toInt();
	for (int i = 0; i < cnt; i++) {
		logVar(a[i]);
	}
	printf("\n");
	fflush(0);
}

void __Log1(var a) {
	__Log(a);
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

#define ARR(...) (jslike::var::arr() = __VA_ARGS__)
#define OBJ(...) (jslike::var::obj() = __VA_ARGS__)
#define log(...) __Log((jslike::var::arr() = __VA_ARGS__));

#endif

#define Arr jslike::var::arr() =
#define Obj jslike::var::obj() =

struct __console {
	void __Log(var a) {
		__Log1(a);
	}
} console;

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

typedef lst<var> vars;
void *newLst() {
	return new vars;
}

void var::deleteLst() {
	delete (vars*)ref->data, delete ref;
}

var var::splice(var start, var deleteCount, var item = argIgnore) {
	var R = Array;
	vars &L = *(vars*) ref->data;
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
	vars L;
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
	var R = Array;
	if (b <= a) { return R; }
	vars &L = * (vars*) R.ref->data;
	
	L.resize(b - a);
	for (int i = 0; i < L.length(); i++) L[i] = self[a + i];
	return R;
}

var var::push(var a) {
	if (type != varArr) {
		log("FATAL ERROR: push() into non-Array\n");
		exit(1);
		//return undefined;
	}
	vars *L = (vars*) ref->data;
	L->push(a);
	return a;
}

var var::pop() {
	vars *L = (vars*) ref->data;
	return L->pop();
}

var var::Push(var a) {
	vars *L = (vars*) ref->data;
	L->delIns(0, 0, &a, 1);
	return a;
}

var var::Pop() {
	vars *L = (vars*) ref->data;
	var R = (*L)[0];
	L->delIns(0, 1, 0, 0);
	return R;
}

var var::length() {
	if (type == varArr) {
		vars *L = (vars*) ref->data;
		return L->length();
	}
	if (type == varStr) {
		return _chr().size;
	}
	return undefined;
}

var& var::getArrElement(int n) {
	vars *L = (vars*) ref->data;
	return (*L)[n];
}

bool operator == (var a, var b) {
	if (a.type == varNum && b.type == varNum) {
		return a.num == b.num;
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
		return a.num < b.num;
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


//#include "keyval.h"
//#include "trie4d.h"
//#include "trie5d.h"
#include "string.h" // memcpy/memcmp

typedef __uint32_t u32;
typedef __int8_t i8;
typedef __uint8_t u8;

u32 MurMur(const char* key, int count) {
	#define muR(x, r) (((x) << r) | ((x) >> (32 - r)))
	u32 seed = 0x8e96a8f2;
	int n = count >> 2;
	u32 k, mur = seed, c1 = 0xcc9e2d51, c2 = 0x1b873593,
	*b = (u32*) (key + n * 4);
	for(int i = -n; i != 0; i++) {
		k = (muR(b[i] * c1, 15) * c2);
		mur = muR(mur^k, 13)*5+0xe6546b64;
	}
	u8 * tail = (u8*)(key + (n<<2));
	k = 0;
	switch(count & 3) {
		case 3: k ^= tail[2] << 16;
		case 2: k ^= tail[1] << 8;
		case 1: k ^= tail[0];
		k = mur ^ muR(k*c1,15)*c2;
	}
	
	mur ^= count;
	mur ^= (mur ^ mur >> 16) * 0x85ebca6b >> 13;
	return mur ^ (mur * 0xc2b2ae35 >> 16);
	#undef muR
}

#define hash MurMur

int initial_size = 1024;
float growth_threshold = 2.0; //when to resize, for example 0.5 means "if number of inserted keys is half of table length then resize". My experiments on english dictionary shows balanced performance/memory savings with 1.0.
float growth_factor = 10; // grow the size of hash table by N, suggested number is between 2 (conserve memory) and 10 (faster insertions).

int dups = 0, resizes = 0, mem_count = 0, slots=0; // count some stats

typedef bool (*enumFunc)(void *key, int count, int *value, void *user);

template <typename value_t> struct jshash {
	struct keynode {

		char *key;
		short len;
		keynode *next;
		value_t value;
		
		keynode(char*k, int l) {
			len = l;
			key = new char[l];
			memcpy(key, k, l);
			next = 0;
			value = -1;
		}
		
		~keynode() {
			delete[] key;
			mem_count += sizeof(keynode) + len;
			if (next) delete next;
		}
		
		bool cmp(char*k, int l) {
			if (len != l) return false;
			return memcmp(key, k, l) == 0;
		}
	};
	
//	struct entry {
//		keynode *k;
//		entry():k(0) {}
//		~entry() {
//			mem_count += sizeof(entry);
//			if (k) delete k;
//		}
//	};
	
	keynode **table;
	int length, count;
	
	jshash() {
		length = initial_size;
		count = 0;
		table = new keynode*[initial_size]();
	}
	
	~jshash() {
		for (int i = 0; i < length; i++) if (table[i]) delete table[i];
		delete[] table;
		mem_count += sizeof(keynode*) * length;
	}
	
	void resize(int newsize) {
	dups = 0; resizes++; slots = 0;
		int o = length;
		keynode **old = table;
		table = new keynode*[newsize]();
		length = newsize;
		for (int i = 0; i < o; i++) {
			keynode *k = old[i];
			while (k) {
				keynode *next = k->next;
				k->next = 0;
				reinsert_when_resizing(k);
				k = next;
			}
			old[i] = 0;
			delete old[i];
		}
		delete[] old;
	}
	
	value_t *result;
	
	bool reinsert_when_resizing(keynode *k2) {
		int n = hash(k2->key, k2->len) % length;
		if (table[n] == 0) {
			slots++;
			table[n] = k2;
			result = &table[n]->value;
			return false;
		}
		keynode *k = table[n];
		k2->next = k;
		table[n] = k2;
		result = &k2->value;
		return false;
	}
	
	bool add(void *key, int keyn) {
		int n = hash((const char*)key, keyn) % length;
		if (table[n] == 0) {
			slots++;
			double f = (double)count / (double)length;
			if (f > growth_threshold) {
				resize(length * growth_factor);
				return add(key, keyn);
			}
			
			table[n] = new keynode((char*)key, keyn);
			result = &table[n]->value;
			count++;
			return false;
		}
		keynode *k = table[n];
		while (k) {
			if (k->cmp((char*)key, keyn)) {
				result = &k->value;
				return true;
			}
			k = k->next;
		}
		dups++;
		count++;
		keynode *k2 = new keynode((char*)key, keyn);
		k2->next = table[n];
		table[n] = k2;
		result = &k2->value;
		return false;
	}
	
	bool find(void *key, int keyn) {
		int n = hash((const char*)key, keyn) % length;
		if (table[n] == 0) {
			return false;
		}
		keynode *k = table[n];
		while (k) {
			//__builtin_prefetch(k->key);
			if (k->cmp((char*)key, keyn)) {
				result = &k->value;
				return true;
			}
			k = k->next;
		}
		return false;
	}
	
	void forEach(enumFunc f, void *user) {
		for (int i = 0; i < length; i++) {
			if (table[i] != 0) {
				keynode *k = table[i];
				while (k) {
					if (!f(k->key, k->len, &k->value, user)) return;
					k = k->next;
				}
			}
		}
	}
};



//typedef Trie4d<int> Dict;
typedef jshash<int> Dict;

//char* shape(jschar *w, int wsize, int &rsize) {
//	char* u = new char[wsize*4];
//	rsize = w2utf(u, wsize*4, w, wsize);
//	return u;
//}

bool addToDict(Dict &dict, var key) {
	jschar *w = key._chr().s;
	int size = key._chr().size;
//	int usize;
//	char *u = shape(w, size, usize);
//	int intval = value.toInt();
	bool existed =  dict.add(w, size*2);
//	delete[] u;
	return existed;
}

bool findNode(Dict &dict, var key) {
	jschar *w = key._chr().s;
	int size = key._chr().size;

//	int usize;
//	char *u = shape(w, size, usize);

	bool found = dict.find(w, size*2);
//	delete[] u;
	return found;
}

struct keyval {
	Dict dict;
	var vals;
	int deleted;
	keyval () {
		vals = Array;
		deleted = 0;
	}
	var &get(var key) {
		key = key.toString();
		bool exist = addToDict(dict, key);
		int k;
		if (exist) {
			k = *dict.result;
		}
		else {
			// reuse deleted slots...
			if (deleted > 0) {
				for (int i = 0; i < vals.length().toInt(); i++) {
					if (vals[i].type == varDeleted) {
						vals[i].type = varNull;
						k = i;
						break;
					}
				}
				deleted--;
			} else {
				k = vals.length().toInt();
				vals.push(undefined);
			}
			*dict.result = k;
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

//var decode32bitArray(var e) {
//	return e;
//	var d = Array;
//	for (var i = 0; i < e.length(); i++) {
//		var line = e[i];
//		var s = "";
//		for (var j = 0; j < line.length(); j+=2) {
//			int a = line[j].toInt(), b = line[j+1].toInt();
//			s += var::fromCharCode(a + (b << 4));
//		}
//		d.push(s);
//	}
//	return d;
//}

bool dictEnumerator(void *key, int keyLength, int *value, void *user) {
	var &data = *(var*) user;
	var str;
	str.makeStringToSet();
	str._chr().set((jschar*)key, keyLength/2);
	data["result"].push(str);
	return true; // continue iteration
}

var var::objectKeys() {
	// iterate over whole dict, performance untested
	if (type != varObj) return undefined;
	keyval *u = (keyval*) ref->data;
	Dict &dict = u->dict;
	var data = Object;
	data["result"] = Array;
	dict.forEach(dictEnumerator, (void*) &data);
//	recursiveScanTrie(&dict.root, result, curPath);
//	if (dict4d::B == 4) result = decode16bitArray(result);
//	if (dict4d::B == 5) result = decode32bitArray(result);
	return data["result"];
}

void var::deleteObj() {
	delete (keyval*)ref->data, delete ref;
}

void var::del(var key) {
	if (type != varObj) return;
	keyval *u = (keyval*) ref->data;
	key = key.toString();

	bool found = findNode(u->dict, key);

	if (found) {
		int v = *u->dict.result;
		*u->dict.result = -1;
		u->vals[v] = undefined;
		u->vals[v].type = varDeleted;
		u->deleted++; // can be reused later
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
		jschar c = C.s[n];
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
	jschar c = C.s[n];
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
	jschar c = C.s[n];
	var R = (int)c;
	return R;
}



bool startNum(var s) {
	jschar C = s.getStringPointer()[0];
	if ((C >= '0' && C <= '9') || C == '-' || C == '.') return true;
	return false;
}

var parseNum(var &s, int &i) {
	int size = s.length().toInt();
	var R = "";
	bool minus = false;
	bool dot = false;
	jschar *CH = s.getStringPointer();
	if (CH[i] == '-') minus = true, i++;
	while (i < size) {
		jschar C = CH[i];
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
		if (s[i] == "\\") { i++; } else 
		if (s[i] == q) break;
		str += s[i];
		i++;
	}
	i++;
	return str;
}

bool isAlpha(jschar c) {
	if (c >= 'a' && c <= 'z') return true;
	if (c >= 'A' && c <= 'Z') return true;
	if (c > 128) return true; // really dirty hack
	return false;
}

bool isNum(jschar c) {
	return (c >= '0' && c <= '9');
}

var parseId(var &s, int &i) {
	int size = s.length().toInt();
	var str = "";
	jschar *C = s.getStringPointer();
	if (!isAlpha(C[i])) return undefined;
	while (i < size) {
		if (!isAlpha(C[i]) && !isNum(C[i])) break;
		str += s[i];
		i++;
	}
	return str;
}

void skipSpaces(var &s, int &i) {
	int size = s.length().toInt();
	jschar *S = s.getStringPointer();
	while (i < size && S[i] <= 32) {
		i++;
	}
}

bool parseSingleCharOp(jschar op, var &s, int &i) {
	int size = s.length().toInt();
	jschar *S = s.getStringPointer();
	while (i < size) {
		jschar C = S[i];
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
	var R = Array;
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
	var R = Object;
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
	i++;
	return R;
}

var parseBool(var &s, int &i) {
	if (s.slice(i, i+4) == "true") { i += 4; return true; }
	if (s.slice(i, i+5) == "false") { i += 5; return false; }
	return undefined;
}

bool parseNull(var &s, int &i) {
	if (s.slice(i, i+4) == "null") { i += 4; return true; }
	if (s.slice(i, i+9) == "undefined") { i += 9; return true; }
	return false;
}

var parseJsonObject(var &s, int &i) {
	var R;
	var *C = &R;
	int size = s.length().toInt();
	if (i < size) {
		var q = s.charAt(i);
		if (startNum(q)) {
			return parseNum(s, i);
		}
		if (q == "[") { 
			R = Array; 
			return parseArray(s, i);
		}
		if (q == "{") {
			R = Object;
			return parseObject(s, i);
		}
		if (q == "\"" || q == "'") { 
			return parseString(s, i);
		}
		var boo = parseBool(s, i);
		if (boo != undefined) return boo;
		bool nul = parseNull(s, i);
		if (nul) return undefined;
		return undefined;
	}
	return undefined;
}

var quoteIfNeeded(var a) {
	jschar *s = a.getStringPointer();
	int size = a.length().toInt();
	for (int i = 0; i < size; i++) {
		if (!isAlpha(s[i])) return (var)"\"" + a + "\"";
	}
	return a;
}

var escapeStr(var a) {
	var R = "";
	for (int i = 0; i < a.length(); i++) {
		if (a[i] == "\"") R += "\\";
		else if (a[i] == "\'") R += "\\";
		else if (a[i] == "\\") R += "\\";
		R += a[i];
	}
	return (var)"\""+R+"\"";
}

struct classJSON {

	var stringifyObj(var a) {
		var keys = a.objectKeys();
		var R = Array;
		for (var i = 0; i < keys.length(); i++) {
			var &ref = a[keys[i]];
			var q = "";
			var s;
			s = stringify(ref);
			R.push( quoteIfNeeded(keys[i]) + ":" + s);
		}
		return (var) "{" + R.join(",") + "}";
	}
	
	var stringifyArr(var a) {
		int cnt = a.length().toDouble();
		var R = Array;
		for (var i = 0; i < cnt; i++) {
			var &ref = a[i];
			var q = "";
			var s;
			s = stringify(ref);
			R.push(s);
		}
		return (var) "[" + R.join(",") + "]";
	}
	
	var stringify(var a) {
		if (a.type == varNull) return "null";
		if (a.type == varStr) return escapeStr(a);
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


#include <stdio.h>

namespace jsfile { // low-level internal use functions

void deleteFile(char *fileName) {
	FILE *f;
	f = fopen(fileName, "wb");
	fclose(f);
}

bool fileExists(char *fileName) {
	FILE *f = fopen(fileName, "r+b");
	bool b = f != NULL;
	if (b) fclose(f);
	return b;
}

void appendFile(char *fileName, void *data, int size) {
	FILE *f;
	if (fileExists(fileName)) {
		f = fopen(fileName, "r+b");
	}
	else {
		f = fopen(fileName, "wb");
	}
	fseek(f, 0, SEEK_END);
	fwrite(data, 1, size, f);
	fclose(f);
}

int fileSize(char *n) {
	FILE *f;
	f = fopen(n, "rb");
	if (f) {
		fseek(f, 0, 2);
		int t;
		t = ftell(f);
		fseek(f, 0, 0);
		fclose(f);
		return t;
	}
	else return -1;
}

char* load(char *fileName) { // you must 'delete' returned pointer
//TODO: rewrite to use size and set into var immediately
	FILE *f = fopen(fileName, "r");
	if (f >= 0) {
		int size = fileSize(fileName);
		if (size == 0) {
			return 0;
		}
		char *s = new char[size];
		int l = fread(s, 1, size, f);
		fclose(f);
		return s;
	}
	else return 0;
}

}

var readFile(var fileName, var encoding = "utf8") {
	var t = fileName;
	char *fn = t.getStringAllocUtf();
	char *c = jsfile::load(fn);
	if (c == 0) return "";
	var R;
	if (encoding == "binary") R.setAscii(c);
	else R = c;
	delete[] fn;
	delete[] c;
	return R;
}

var writeFile(var fileName, var data, var encoding = "utf8") {
	using namespace jsfile;
	int dataSize;
	char *fn, *d;
	fn = fileName.getStringAllocUtf();
	if (encoding == "utf8") {
		d = data.getStringAllocUtf(&dataSize);
	}
	else {
		d = data.getStringAllocAscii(&dataSize);
	}
	if (fileExists(fn)) {
		deleteFile(fn);
	}
	appendFile(fn, d, dataSize);
	delete[] fn;
	delete[] d;
	return undefined;
}



var typeName(varType a) {
	if (a == varNum) return "number";
	if (a == varStr) return "string";
	if (a == varArr) return "array";
	if (a == varNull) return "undefined";
	if (a == varObj) return "object";
	if (a == varBool) return "boolean";
	return "unsure";
}

var var::concat(var a) {
	var R = Array;
	vars &L = *(vars*) R.ref->data;
	int count2 = a.length().toInt();
	int count = length().toInt(), x = 0;
	L.resize(count + count2);
	for (int i = 0; i < count; i++) L[x++] = self[i];
	for (int i = 0; i < count2; i++) L[x++] = a[i];
	return R;
}

var var::concatAll() {
	var R = Array;
	int count = length().toInt(), x = 0;
	for (int i = 0; i < count; i++) R = R.concat(self[i]);
	return R;
}

var var::replace(var find, var repl) {
	find = find.toString();
	repl = repl.toString();
	var R ;
	R.makeStringToSet();
	_chr().replace(find._chr(), repl._chr(), R._chr());
	return R;
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
	if (type == varNum || type == varBool) num = a.num;
	else {
		if (a.type == varNull) { 
			//ref = 0;
			return;
		}
		ref = a.ref;
		if (ref) ref->uses++;
	}
}

var splitEveryChar(var a) {
	var R = Array;
	int len = a.length().toDouble();
	for (int i = 0; i < len; i++) R.push(a.charAt(i));
	return R;
}

var var::split(var separator) {
	separator = separator.toString();
	self = self.toString();
	if (separator.length() == 0) return splitEveryChar(self);
	if (self.length() == 0) return Array;
	chr
		&V = self._chr(),
		&D = separator._chr();
	int
		start = 0,
		i = 0,
		count = V._strcount(D) + 1;
	var R = Array;
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
