#jslike - JavaScript-like var/Array/Object/function for C++

C++ has it's own powerful ways of doing things, but when it comes to quick prototyping it sucks. jslike let's you create objects, assign properties, use arrays and strings a JavaScript way right in your C++ code by exposing the single `var` class.


```cpp
	// The library is included by source, so you simply add these two lines:
	#include "jslike.h"
	using namespace jslike;
	// jslike tries not to pollute your namespace as much as possible, so
	// everything is inside the "namespace jslike"
	// But it exposes two macros ARR and OBJ. If you think those macros
	// could cause you trouble, please undefine them or redefine them.
```cpp

	
	/* Undefined variables and numbers */

```cpp
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
		stars = ARR("Vega", "Aldebaran", "Altair", "Decrux");
		log("Stars:", stars);
		//This syntax is achieved with overloading operator= and operator, for a
		// temporary class "arrset".
	}
	
	/* Object variables */
	// Object literal notation:
	{
		var o = OBJ("x", 100, "y", 400); // -> { x: 100, y: 400 } 
		log(JSON.stringify(o));
		o["x"]++;
		o["y"] = o["x"] * 2;
		log(JSON.stringify(o)); // -> { x: 101, y: 202 }
		// unfortunately there is no way to enable "o.x" style notation in C++ 
		o.del("x"); // delete existing propery specified by the key name
		log(JSON.stringify(o)); // -> { y: 202 }
	}
	
```

