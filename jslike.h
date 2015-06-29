#ifndef __JSLIKE_H__
#define __JSLIKE_H__

namespace jslike {

#include "chrutil.h"
#include "chr.h"
#include "stk.h"
#define constructor

//TODO: Garbage collector (mark/sweep)
//functions

enum varType { varIgnore = -2, varNull=-1, varNum=0, varStr=1, varArr=2, varObj=3, varFunc=4, varBool=5 };
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
#include "construct.h"

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

#include "jsexpr.h"
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

#include "jsset.h"

extern "C" void exit(int);

var & Undefined() {
	static var R;
	var none;
	R = none;
	return R;
}

#include "jslog.h"
#include "lst.h"
#include "jslst.h"
#include "jsbool.h"
#include "keyval.h"
#include "jsobj.h"
#include "jsix.h"
#include "jschar.h"
#include "json.h"

var typeName(varType a) {
	if (a == varNum) return "number";
	if (a == varStr) return "string";
	if (a == varArr) return "array";
	if (a == varNull) return "undefined";
	if (a == varObj) return "object";
	return "unsure";
}

var var::concat(var a) {
	var R = Array;
	lst &L = *(lst*) R.ref->data;
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
		if (a.type == varNull) { ref = 0; return; }
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
	if (self.length() == 0) return Undefined();
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
