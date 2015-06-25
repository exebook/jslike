#include "stdio.h"
#ifdef linux
#	define ロ(...) printf(__VA_ARGS__); fflush(0);
#endif
#include "jslike.h"

void testArrayLiteral() {
	var c;
	c.Arr() = 1,2,3,4,5;
	log(JSON.stringify(c));
}

void testObjectLiteral() {
	var o = obj;
	o["a"] = 1;
	o["b"] = 2;
	o["c"] = obj;
	o["c"]["d"] = 3;
	o["e"] = obj;
	o["e"]["f"] = 4;
	o["e"]["g"] = obj;
	o["e"]["g"]["h"] = 5;
	
	var a;
	a.Obj() = "a", 1, "b", 2, "c", obj, "d", 3, end, "e", obj,
		"f", 4, "g", obj, 
			"h", 5,
		end,
	end;
	log(JSON.stringify(o));
	log(JSON.stringify(a));
}

void testNumbers() {
	var a, b, c, d;
	a = 1;
	b = 2;
	c = 555;
	d = 100.100;
	log(a + b, c, d);
}

var fu (var a) {
	var R;
	return R;
}

int main(int argc, char* argv[]) {
	var a;
	fu(a);
	testNumbers();
	testArrayLiteral();
	testObjectLiteral();
}

