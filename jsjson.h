
bool startNum(var s) {
	wchar_t C = s.getStringPointer()[0];
	if ((C >= '0' && C <= '9') || C == '-' || C == '.') return true;
	return false;
}

var parseNum(var &s, int &i) {
	int size = s.length().toInt();
	var R = "";
	bool minus = false;
	bool dot = false;
	wchar_t *CH = s.getStringPointer();
	if (CH[i] == '-') minus = true, i++;
	while (i < size) {
		wchar_t C = CH[i];
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

bool isAlpha(wchar_t c) {
	if (c >= 'a' && c <= 'z') return true;
	if (c >= 'A' && c <= 'Z') return true;
	if (c > 128) return true; // really dirty hack
	return false;
}

bool isNum(wchar_t c) {
	return (c >= '0' && c <= '9');
}

var parseId(var &s, int &i) {
	int size = s.length().toInt();
	var str = "";
	wchar_t *C = s.getStringPointer();
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
	wchar_t *S = s.getStringPointer();
	while (i < size && S[i] <= 32) {
		i++;
	}
}

bool parseSingleCharOp(wchar_t op, var &s, int &i) {
	int size = s.length().toInt();
	wchar_t *S = s.getStringPointer();
	while (i < size) {
		wchar_t C = S[i];
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
	var R = arr;
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
	var R = obj;
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
			R = arr; 
			return parseArray(s, i);
		}
		if (q == "{") {
			R = obj;
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
}

var quoteIfNeeded(var a) {
	wchar_t *s = a.getStringPointer();
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
		var R = arr;
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
		var R = arr;
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

