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
	} else if (type = varStr) {
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
