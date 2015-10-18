
//typedef Trie4d<int> Dict;
typedef jshash<int> Dict;

//char* shape(jschar *w, int wsize, int &rsize) {
//	char* u = new char[wsize*4];
//	rsize = w2utf(u, wsize*4, w, wsize);
//	return u;
//}

bool addToDict(Dict &dict, var key) {
	jschar *w = key._chr().s;
	int size = key._chr().size;
//	int usize;
//	char *u = shape(w, size, usize);
//	int intval = value.toInt();
	bool existed =  dict.add(w, size*2);
//	delete[] u;
	return existed;
}

bool findNode(Dict &dict, var key) {
	jschar *w = key._chr().s;
	int size = key._chr().size;

//	int usize;
//	char *u = shape(w, size, usize);

	bool found = dict.find(w, size*2);
//	delete[] u;
	return found;
}

struct keyval {
	Dict dict;
	var vals;
	int deleted;
	keyval () {
		vals = Array;
		deleted = 0;
	}
	var &get(var key) {
		key = key.toString();
		bool exist = addToDict(dict, key);
		int k;
		if (exist) {
			k = *dict.result;
		}
		else {
			// reuse deleted slots...
			if (deleted > 0) {
				for (int i = 0; i < vals.length().toInt(); i++) {
					if (vals[i].type == varDeleted) {
						vals[i].type = varNull;
						k = i;
						break;
					}
				}
				deleted--;
			} else {
				k = vals.length().toInt();
				vals.push(undefined);
			}
			*dict.result = k;
		}
		return vals[k];
	}
};

void *newObj() {
	return new keyval;
}

var & var::getObjElement(const var &n) {
	keyval *u = (keyval*) ref->data;
	var &R = u->get(n);
	return R;
}

//var decode32bitArray(var e) {
//	return e;
//	var d = Array;
//	for (var i = 0; i < e.length(); i++) {
//		var line = e[i];
//		var s = "";
//		for (var j = 0; j < line.length(); j+=2) {
//			int a = line[j].toInt(), b = line[j+1].toInt();
//			s += var::fromCharCode(a + (b << 4));
//		}
//		d.push(s);
//	}
//	return d;
//}

bool dictEnumerator(void *key, int keyLength, int *value, void *user) {
	var &data = *(var*) user;
	var str;
	str.makeStringToSet();
	str._chr().set((jschar*)key, keyLength/2);
	data["result"].push(str);
	return true; // continue iteration
}

var var::objectKeys() {
	// iterate over whole dict, performance untested
	if (type != varObj) return undefined;
	keyval *u = (keyval*) ref->data;
	Dict &dict = u->dict;
	var data = Object;
	data["result"] = Array;
	dict.forEach(dictEnumerator, (void*) &data);
//	recursiveScanTrie(&dict.root, result, curPath);
//	if (dict4d::B == 4) result = decode16bitArray(result);
//	if (dict4d::B == 5) result = decode32bitArray(result);
	return data["result"];
}

void var::deleteObj() {
	delete (keyval*)ref->data, delete ref;
}

void var::del(var key) {
	if (type != varObj) return;
	keyval *u = (keyval*) ref->data;
	key = key.toString();

	bool found = findNode(u->dict, key);

	if (found) {
		int v = *u->dict.result;
		*u->dict.result = -1;
		u->vals[v] = undefined;
		u->vals[v].type = varDeleted;
		u->deleted++; // can be reused later
	}
}
