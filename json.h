/*
	TODO: use a tokenizer here! otherwise not possible to do comments properly.
	
*/
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


var parseComment (var &s, int &i) {
	i += 2;
	while (true) {
		if (s.charAt(i) == "*" && s.charAt(i+1) == "/") {
			i += 2;
			break;
		}
		if (i >= s.length().toInt()) break;
		i++;
	}
	return undefined;
}

var parseArray(var &s, int &i) {
	var R = Array;
	int size = s.length().toInt();
	i++;
	while (i < size) {
		skipSpaces(s, i);
		if (s[i] == "]") {
			i++;
			break;
		}
		if (s.charAt(i) == "/" || s.charAt(i+1) == "*") { 
			parseComment(s, i);
			continue;
		}

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
				log("fatal: JSON parse error, expected an id.");
				exit(1);
				return undefined;
			}
		}
		if (s.charAt(i) == "/" || s.charAt(i+1) == "*") { 
			// TODO: the comment is only possble right after comma, or as the beginning of an object. So obviously, this JSON parser should be changed. Write a tokenizer first, and just skip comments on a token level.
			parseComment(s, i);
			continue;
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
//			R = Array; 
			return parseArray(s, i);
		}
		if (q == "{") {
//			R = Object;
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
		
		if (a[i] == "\r") R += "\\r";
		else if (a[i] == "\n") R += "\\n";
		else R += a[i];
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

