#ifndef __JSLIKE_H__
#define __JSLIKE_H__

namespace jslike {

#include "jsutil.h"
#include "jsstr.h"
#include "stk.h"
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

int _id = 0;
struct var;// void log(var a);
struct var {
	int id;
	varType type;
	union {
		double n;
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

	char* getStringCopyUtf() {
		return _chr().getUtf();
	}

	char* getStringCopyAscii() {
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
#include "jsset.h"

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

#include "jslog.h"
#include "lst.h"
#include "jslst.h"
#include "jsbool.h"
#include "keyval.h"
#include "jsobj.h"
#include "jsix.h"
#include "jschar.h"
#include "jsjson.h"

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
	if (a.type == varNull) { ref = 0; return; }
	type = a.type;
	if (type == varNum || type == varBool) n = a.n;
	else {
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
