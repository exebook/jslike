#jslike - JavaScript-like "var" (Number/String/Array/Object/function) for C++

jslike let's you create objects, assign properties, use arrays and strings a JavaScript way right in your C++ code by exposing the single `var` class.


###Currently implemented

- Number, String, Array, Object, Boolean.
- Pass by value for Numbers/Boolean, pass by reference for Array/Object.

####Functions and properties:

- `var = var, int, double, char*, wchar_t*, bool` -- construct var from basic types
- `var.toString()`
- `var[#]` -- Array index access
- `var[key]` -- Object property access
- `var[#]` -- String character access
- `var + var` -- Arithmetic/logic operators `++` `--` `+` `-` `/` `*` `%` `&` `|` `&&` `||` `==` `!=` `<` `>`
- `var + var` -- String concatenation, comparison `<` `>` `==`
- `var.round()`
- `var.charAt()`
- `var.charCodeAt()`
- `var.toNumber()`
- `var.fromCharCode()`
- `var.split()`
- `var.objectKeys()` -- JavaScript's `Object.keys()`
- `var.indexOf()` -- Array/String
- `var.push()`
- `var.pop()`
- `var.Push()` -- JavaScript `unshift()` (**shift** is a reserved word in C++).
- `var.Pop()` -- JavaScript `shift()`
- `var.length()`
- `var.splice()`
- `var.slice()`
- `var.join()`
- `var.typeOf()`
- `var.del()` -- delete key/value pairs from Object
- `var.has()` -- check if key is set in the Object
- `undefined` -- set and compare vars with Undefined(), undefined
- `JSON.stringify()`
- `JSON.parse()`

Conversion to basic C++ types:

- `wchar_t* getStringPointer()`
- `char* getStringAllocUtf()`
- `char* getStringAllocAscii()`
- `int toDouble()`
- `int toInt()`

Experimental complex literals:

- `var::arr() = a,b,c,d,....`
- `var::obj() = a,b,c,d,....`
- `ARR(1,2,3,4)`
- `OBJ("a", 2, "b", 4)`
- `log(...)` -- function to conveniently output to console

###Not yet implemented
- mark&sweep GC
- functions, lambdas, closures
- map, sort, filter etc...

##Examples

The library is included by source, so you simply add these two lines:
```js
	#include "jslike.h"
	using namespace jslike;
```
**jslike** tries not to pollute your namespace as much as possible, so
 everything is inside the "namespace jslike".

	
###Undefined variables and numbers

```js
var x;
var y = 42;
log(x, x.typeOf());
log(y, y.typeOf());
x = 3, y = 4;
log(x + y);
log(x - 4, x + 5, x / y, x * y);
```

```console
undefined undefined
42 number
7
-1 8 0.75 12
```

###Strings
```js
var hello = "hello";
var world;
world = "world";
log(hello + "-" + world + "!"); // string concatenation
log(hello, world); // log can accept unlimited number or arguments
log(hello.indexOf("ll"));
```

```console
hello-world!
hello world
2
```

```js
var s = "Nuke the entire site from orbit";
log("Sentence length:", s.length());
log(s.slice(0, 4), s.slice(-10));

for (int i = 0; i < 4; i++) log(s.charAt(i), s.charCodeAt(i));
```

```console
Sentence length: 31
Nuke from orbit
N 78
u 117
k 107
e 101
```

###Mix numbers and strings
```js	
var a = "A", b = 1; // 'string + number' implies conversion to string
log(a + b);
log((var)"Received #" + 12 + " from " + 2 + " planets");
log((var)"2 + 2 = " + (2 + 2)); // explicit var constructor
```


```console
A1
Received #12 from 2 planets
2 + 2 = 4
```

###Arrays

Declare empty array and push something into it.

```js
var A = Array;
A.push("planet");
A.push("LV");
A.push(426);
log(A);
```

Array `toString()` is called before output.

```console
planet, LV, 426
```

### log()
log() is console.log() of JavaScript.

log can print any var or anything that a var constructor accepts.

```js
var a = 1;
var b = "hello";
var c = Array;
c.push(100);
c.push(200);
log(a, b, c, 500, "world");
```

```console
1 hello 100, 200 500 world
```
	
###Array.slice()
```js
var fruits;
fruits = (Arr "Banana", "Orange", "Lemon", "Apple", "Mango");
var citrus = fruits.slice(1, 3);
log(citrus);
```

The contents of citrus will be:

```console
Orange, Lemon
```

###Array splice()

Unlike JavaScript you cannot insert more than one item in splice().

```js
var fruits = (Arr "Banana", "Orange", "Apple", "Mango");
fruits.splice(2, 0, "Kiwi");
fruits.splice(2, 0, "Lemon");
log(fruits);
```

```console
Banana, Orange, Lemon, Kiwi, Apple, Mango 
```

###Array indexOf() and String indexOf()
```js
var q;
q = (Arr 0,1,2,3,4,5,6);
var j = q.indexOf(3);
log(q);
log(j);
var a = "hello world";
log(a.indexOf("wo"));
```	

```console
0, 1, 2, 3, 4, 5, 6 
3 
6 
```

###Array literal notation

The closest syntax we could achieve in C++ to cozy JavaScript Array literal notation is this: `var x = (Arr list-of-items)`.
This syntax is achieved with overloading `operator=` and `operator,` for a temporary class `arrset`. And then hiding it under the macro `Arr`.

`Arr` is a `#define` macro that expands to `var::arr() =`.

```js
var planets = (Arr "Earth", "Venus", "Mars", "Jupiter");
var stars = (Arr "Vega", "Aldebaran", "Altair", "Decrux");
log("Stars:", stars);
```

###Object variables

```js
var xy = obj; // Declare empty object
xy["x"] = 100;
xy["y"] = 200;
log(JSON.stringify(xy));
```

```console
{ x: 100, y: 200 } 
```

Object literal notation:

```js
var o = OBJ("x", 100, "y", 400);
log(JSON.stringify(o)); // -> { x: 100, y: 400 } 

//Property access
o["x"]++;
o["y"] = o["x"] * 2;
log(JSON.stringify(o)); // -> { x: 101, y: 202 }

// unfortunately there is no way to enable "o.x" style notation in C++ 
o.del("x"); // delete existing propery specified by the key name
log(JSON.stringify(o)); // -> { y: 202 }

```


##Converting between var and basic types
```js
var a = 123;
int i = a.toInt();
double d = a.toDouble();
printf("Basic types: %i %f\n", i, d);
var hello = "world";

char *str = hello.getStringAllocUtf(); // ..Alloc() means you are
// responsible for deallocation of the returned data. Use `delete`.
printf("char* = %s\n", str);
delete str; // you must delete what you got with ...Alloc().

str = hello.getStringAllocAscii(); // Ascii is faster that Utf.
printf("char* = %s\n", str);
delete str;

// Internally strings are stored as UTF16/32 AKA wchar_t.
wchar_t *w = hello.getStringPointer();
for (int i = 0; i < hello.length().toInt(); i++) {
	printf("char code: %i, char: %c\n", w[i], w[i]);
}
// getStringPointer returns a pointer to an internal data, it 
// is valid until the var exists. Do not delete the returned pointer.
```

###Passing var as an argument

```js
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

```
