#include "stdio.h"
#ifdef linux
#	include "wchar.h"
#	define ロ(...) printf(__VA_ARGS__); fflush(0);
#endif
#include "jsutil.h"
#include "jsstr.h"
#define constructor
//#ifdef NONE
//Итак, мы почти доделали класс Chr который будет обеспечивать String.
//Теперь надо к нему ещё slice и прочие няшки.
//Потом надо для Array и Object сделать функционал.
//А потом всё это собрать в кучку под крышей `var`.
//Подумать над рефренс-контином и как это вообще синтаксически будет.
//Потом когда всё заработает, подумать над сборщиком-мусора.
//Да, и подумать над функциями, возможно их легко сделать!

//struct var;void logVar(var &a);
enum varType { varIgnore = -2, varNull=-1, varNum=0, varStr=1, varArr=2, varObj=3, varFunc=4 };
enum varSyntax { argIgnore, undefined, arr, obj, NaN };

struct Ref {
	int uses;
	void *data;
	Ref () {
		uses = 1;
	}
};

void *newLst();
void *newObj();

int _id = 0;
struct var;// void log(var a);
struct var {
	int id;
	varType type;
	union {
		double n;
		Ref* ref;
	};
	constructor var () {
		id=_id++; type = varNull; ref = 0;
	}
  ~var () { unref(); }
	constructor var (const var& a) {
		id=_id++; 
		type = varNull; ref = 0;
		copy(a);
	}

	constructor var (const varSyntax syntax) {
		id=_id++; 
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
		id=_id++; 
		self = (double) a;
	}
	void operator = (int a) { self = (double)a;}
	
	void operator = (double a) {
		n = a;
		type = varNum;
	}
	constructor var (char* a) {
		id=_id++; 
		ref = 0;
		self = a;
	}
	chr & _chr() { return *((chr*)(ref->data)); }
	constructor var (double a) {
		id = _id++;
		self = a;
	}
	
	void makeStringToSet() {
		// use to set some value from wchar_t* to a new var
		ref = new Ref;
		type = varStr;
		ref->data = new chr;
	}
	
	void operator = (char* a) {
		if (ref) unref();
		makeStringToSet();
		_chr().setUtf(a);
		//_chr().setAscii(a);
	}
	
	void copy(const var &a);
	
	void unref() {
		if (type == varNull || type == varNum) return;
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
	}
	
	double toDouble() {
		if (type == varNum) return n;
		return NaN;
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
	
	var &operator [] (var a) {
		if (type == varNull || type == varNum) { 
			static var R; 
			return R;
		}
		if (type == varStr) {
			static var R;
			R = "1";
			int n = a.toDouble();
			wchar_t c = _chr().s[n];
			R._chr().s[0] = c;
			return R;
		}
		if (type == varArr) {
			int n = a.toDouble();
			return getArrElement(n);
		}
		if (type == varObj) {
			return getObjElement(a);
		}
		static var R;
		R = undefined;
		return R;
	}
	var indexOf(var a);
	// decls:
	var &getArrElement(int n);
	var push(var a);
	var pop();
	var length();
	var splice(var start, var deleteCount, var item);
	var slice(var start, var end);
	var join(var separator);
	void deleteLst();
	// obj
	var &getObjElement(const var &n);
	void deleteObj();
};

bool operator == (var a, varSyntax b) {
	if (b == undefined) {
		return (a.type == varNull || a.type == varIgnore);
	}
	if (b == NaN) {
		return (a.type != varNum);
	}
	return false;
}

#include "jslst.h"
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
		int oldcap = capacity;
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

//////////// VAR LST  /////////////////

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
	if (type != varArr) return undefined;
	int 
		size = length().toDouble(),
		a = start.toDouble(),
		b = end.toDouble();
	if (end == undefined) b = size;
	else if (b < 0) b += size;
	else if (b > size) b = size;
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

///////////////////////////////////////

void logVar(var a) {
	var z;
	z = a.toString();
	printf("%s ", z._chr().getUtf());
}

void log(var a, 
	var b = argIgnore, var c = argIgnore, var d = argIgnore,
	var e = argIgnore, var f = argIgnore, var g = argIgnore,
	var h = argIgnore, var i = argIgnore, var j = argIgnore,
	var k = argIgnore, var l = argIgnore)
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
	printf("\n");
}


struct tset {
	var *ref;
	tset (var &a) {
		ref = &a;
	}
	tset &operator , (var b) {
		if (ref->type == varArr) ref->push(b);
		return self;
	}
	tset &operator = (var b) {
		(*ref) = arr;
		ref->push(b);
		return self;
	}
};

tset &set(var &a) {
	static tset R(a);
	return R;
}

var typeName(varType a) {
	if (a == varNum) return "number";
	if (a == varStr) return "string";
	if (a == varArr) return "array";
	if (a == varNull) return "undefined";
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

var var::indexOf(var a) {
	if (type == varArr) {
		int cnt = length().toDouble();
		for (int i = 0; i < cnt; i++) {
			if (self[i] == a) return i;
		}
	}
	return -1;
}
//-- deo.bg.on j #ffe --
	void var::copy(const var &a) {
		// at this point this object is empty
		if (a.type == varNull) { ref = 0; return; }
		type = a.type;
		if (type == varNum) n = a.n;
		else {
			ref = a.ref;
			chr &c = (*(var*)&a)._chr();
			if (ref) ref->uses++;
		}
	}
//-- deo.bg.off j

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

/////////// VAR OBJECT FUCNTIONS

void *newObj() {
	return new keyval;
}

var & var::getObjElement(const var &n) {
	keyval *u = (keyval*) ref->data;
	var &R = u->get(n);
	return R;
}

void var::deleteObj() {
	delete (keyval*)ref->data, delete ref;
}

/////////// --------------------

int main(int argc, char* argv[]) {
	var a = obj;
	a["hello"] = "world";
	a["1"] = "one";
	a["2"] = "two";
	log("a[\"hello\"]", a["hello"]);
	log("result must be \"one\":", a[1]);
	log("a[\"1\"]", a["1"]);
	log("a[2]:", a[2]);
}
