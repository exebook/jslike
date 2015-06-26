#include "stdio.h"
#ifdef linux
#	include "wchar.h"
#	define ロ(...) printf(__VA_ARGS__); fflush(0);
#endif

#include "jslike.h"
using namespace jslike;

void testArrayLiteral() {
	var c;
	c.Arr() = 1,2,3,4,5;
	log(c.pop());
	log(c.pop());
	c.push(6);
	c.Push(222);
	c.Push(333);
	log(JSON.stringify(c));
	log("Pop():", c.Pop() + ",", "Arr:", c);
	log("Pop():", c.Pop() + ",", "Arr:", c);
}

void testObjectLiteral() {
	// both examples create an object like this:
	// { a: 1, b: 2, c: { d: 3 }, e: { f: 4, g: { h: 5 } } } 

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
	log(a + b, c - d);
}

var chainNext(var a) {
	return a["next"];
}

var chainPrev(var a) {
	return a["prev"];
}

var chainBegin() {
	var R = obj;
	R["next"] = undefined;
	R["prev"] = undefined;
	return R;
}

var chainAdd(var afterWhat, var what) {
	what["next"] = afterWhat["next"];
	afterWhat["next"] = what;
	what["prev"] = afterWhat;
	if (what["next"] != undefined) what["next"]["prev"] = what;
	return what;
}

var chainRemove(var a) {
	if (a["prev"].has("next")) a["prev"]["next"] = a["next"];
	if (a["next"].has("prev")) a["next"]["prev"] = a["prev"];
	return a;
}

void showSolarObjects(var start, var title, var isMoon) {
	var i = start;
	log(title);
	while (i != undefined) {
		if (!isMoon.toDouble() || i.has("moonOf")) {
			var s = "";
			if (i["moonOf"] != undefined) s = (var)
				", a moon of " + i["moonOf"]["name"];
			log(i["name"] + ",", i["radius"] + s);
		}
		i = chainNext(i);
	}
}

void testChain() {
	var earth = chainBegin();
	earth["name"] = "Earth";
	earth["radius"] = 1;
	var last = earth, moon, sun;
	last = chainAdd(last, (var::initObj() = "name", "Moon", "radius", 0.27, "moonOf", earth));
	moon = last;
	last = chainAdd(last, (var::initObj() = "name", "Venus", "radius", 0.95));
	last = chainAdd(last, (var::initObj() = "name", "Sun", "radius", 109));
	sun = last;
	last = chainAdd(last, (var::initObj() = "name", "Mars", "radius", 0.5));
	last = chainAdd(last, (var::initObj() = "name", "Mercury", "radius", 0.4));

	last = chainAdd(last, (var::initObj() = "name", "Jupiter", "radius", 10.9));
	last = chainAdd(last, (var::initObj() = "name", "Ganymede", "radius", 0.4, "moonOf", last));

	last = chainAdd(last, (var::initObj() = "name", "Saturn", "radius", 9.1));
	last = chainAdd(last, (var::initObj() = "name", "Titan", "radius", 0.4, "moonOf", last));

	last = chainAdd(last, (var::initObj() = "name", "Uranus", "radius", 3.9));
	last = chainAdd(last, (var::initObj() = "name", "Neptune", "radius", 3.8));
//	last = chainAdd(last, (var::initObj() = "name", "", "radius", ));
	showSolarObjects(earth, "Object\t radius(earths)", false);
	showSolarObjects(earth, "\nMoons only:", true);
}

void testIndexOf() {
	var A;
	A.Arr() = 0,1,2,3,4,5,6;
	var j = A.indexOf(3);
	log(A);
	log(j);
	var a = "hello world";
	log(a.indexOf("wo"));
}

void readme() {
	/* Undefined variables and numbers */

	var x; // define "empty" variable (x == undefined)
	var y = 2; // define a number variable
	log(x, x.typeOf()); // -> undefined undefined
	log(y, y.typeOf()); // -> 2 number
	x = 3, y = 4;
	log(x + y);
	// Basic math operations:
	log(x - 4, x + 5, x / y, x * y); // -> -1 8 0.75 12
	
	/* Strings */

	var hello = "hello"; // define a string variable
	var world;
	world = "world";
	log(hello + " " + world); // string concatenation
	log(hello, world); // log can accept up to 28 arguments
	log(hello.indexOf("ll")); // -> 2

	var s = "Nuke the entire site from orbit";
	log("Sentence length:", s.length()); // -> 31
	log(s.slice(0, 4), s.slice(-10)); // -> Nuke from orbit
	
	for (int i = 0; i < 4; i++) log(s.charAt(i), s.charCodeAt(i));
	/* output:
		N 78 
		u 117 
		k 107 
		e 101 
	*/
	
	/* Mix numbers and strings */
	var a = "A", b = 1; // 'string + number' implies conversion to string
	log(a + b); // -> A1
	log((var)"Received #" + 12 + " from " + 2 + " planets");
	// -> Received #12 from 2 planets
	
	/* Arrays */
	var A = arr; // declare empty array
	A.push("planet");
	A.push("LV");
	A.push(426);
	log(A); // Array toString() is called before output
	// -> 
	
	/* log() analog of console.log() in JavaScript */
	// log can print any var or anything that a var constructor accepts.
	{
		var a = 1;
		var b = "hello";
		var c = arr;
		c.push(100);
		c.push(200);
		log(a, b, c, 500, "world");
	}
	
	/* w3shools Array.slice() example */	
	var fruits;
	fruits.Arr() = "Banana", "Orange", "Lemon", "Apple", "Mango";
	var citrus = fruits.slice(1, 3);
	//The result of citrus will be:
	log(citrus);
	//Orange,Lemon

	/* Array splice() */
	fruits.Arr() = "Banana", "Orange", "Apple", "Mango";
	fruits.splice(2, 0, "Kiwi"); // unlike JavaScript you cannot insert more than one item here.
	fruits.splice(2, 0, "Lemon");
	log(fruits);
	
	/* Array indexOf() and String indexOf() */
	{
		var q;
		q.Arr() = 0,1,2,3,4,5,6; // Array literal notation
		var j = q.indexOf(3);
		log(q);
		log(j);
		var a = "hello world";
		log(a.indexOf("wo"));
	}
	
	/* Array literal notation */
	{
		var planets;
		// literal setter:
		planets.Arr() = "Earth", "Venus", "Mars", "Jupiter";
		// static literal returner, useful for passing as an argument.
		var stars = (var::initArr() = "Vega", "Aldebaran", "Altair", "Decrux");
		// if you are OK with using macros, there is a macro-shortcut for the above:
//		stars = ARR("Vega", "Aldebaran", "Altair", "Decrux");
		log("Stars:", stars);
		//This syntax is achieved with overloading operator= and operator, for a
		// temporary class "arrset".
	}
	
	/* Object variables */
	// Object literal notation:
	{
		var o = (var::initObj() = "x", 100, "y", 400); // -> { x: 100, y: 400 } 
//		var o = OBJ("x", 100, "y", 400); // -> { x: 100, y: 400 }
		log(JSON.stringify(o));
		o["x"]++;
		o["y"] = o["x"] * 2;
		log(JSON.stringify(o)); // -> { x: 101, y: 202 }
		// unfortunately there is no way to enable "o.x" style notation in C++ 
		o.del("x"); // delete existing propery specified by the key name
		log(JSON.stringify(o)); // -> { y: 202 }
	}
	
	/* Converting between var and basic types */
	{
		var a = 123;
		int i = a.toInt();
		double d = a.toDouble();
		printf("Basic types: %i %f\n", i, d);
		var hello = "world";
		char *str = hello.getStringCopyUtf(); // ..Copy() means you are responsible for deallocation of the returned data. Use `delete`.
		printf("char* = %s\n", str);
		delete str; // you must delete what you got with ...Copy().

		str = hello.getStringCopyAscii(); // Ascii is faster that Utf.
		printf("char* = %s\n", str);
		delete str;

		// Internally strings are stored as UTF16/32 AKA wchar_t.
		wchar_t *w = hello.getStringPointer();
		for (int i = 0; i < hello.length().toInt(); i++) {
			printf("char code: %i, char: %c\n", w[i], w[i]);
		}
	}
	
}

/* Passing var as an argument */

var sum10(var a, var b) {
	a = a * 10;
	b = b * 10;
	return a + b;
}

void popAndPrint(var a) {
	log(a.pop());
}

void _main() {
	var x = 400, y = 500;
	log(sum10(x, y)); // 9000
	log(x, y); // 400, 500 are left unmodified, because x,y were passed by values
	var b;
	b.Arr() = 100,200,300;
	popAndPrint(b);
	popAndPrint(b);
	log("What remains of b:", b); // prints 100
	// because arrays and objects are passed by reference
}

var fu (var a) {
	var R;
	return R;
}

var f(var b) {
	b = b.toString();
	return b;
}

int main(int argc, char* argv[]) {
//	testNumbers();
//	testArrayLiteral();
//	testObjectLiteral();
//	testChain();
//	testIndexOf();
//	readme();
_main(); 
//return 0;
//for (int i = 0; i < 1000000; i++) {
	var stars = (var::initArr() = "Vega", "Aldebaran", "Altair", "Decrux");
	log(stars);
	var o = (var::initObj() = "x", 100, "y", 400); // -> { x: 100, y: 400 }
	log(JSON.stringify(o));
	var a;
	a = (var::initObj() = "a", 1, "b", 2, "c", obj, "d", 3, end, "e", obj,
		"f", 4, "g", obj,
			"h", 5,
		end,
	end);
	log(JSON.stringify(a));

	var b;
	b.Arr() = 100,200,300;
	log(b);
	b.Obj() = 100,200,300,400;
	log(JSON.stringify(b));
//}
}

