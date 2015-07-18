

char* shape(wchar_t *w, int wsize, int &rsize) {
	char* u = new char[wsize*4];
	rsize = w2utf(u, wsize*4, w, wsize);
	return u;
}

bool addToTrie(trie4d::Trie4d<int> &trie, var key) {
	wchar_t *w = key._chr().s;
	int size = key._chr().size;

	int usize;
	char *u = shape(w, size, usize);
//	int intval = value.toInt();
	bool existed =  trie.add(u, usize);
	delete[] u;
	return existed;
}

bool findNode(trie4d::Trie4d<int> &trie, var key) {
	wchar_t *w = key._chr().s;
	int size = key._chr().size;

	int usize;
	char *u = shape(w, size, usize);

	bool found = trie.find(u, usize);
	delete[] u;
	return found;
}

struct keyval {
	trie4d::Trie4d<int> trie;
	var vals;
	int deleted;
	keyval () {
		vals = Array;
		deleted = 0;
	}
	var &get(var key) {
		key = key.toString();
		bool exist = addToTrie(trie, key);
		int k;
		if (exist) {
			k = *trie.result;
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
			*trie.result = k;
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

bool trieEnumerator(void *key, int count, int *value, void *user) {
	var &data = *(var*) user;
	var str; str.setUtf((char*)key, count);
	data["result"].push(str);
	return true; // continue iteration
}

var var::objectKeys() {
	// iterate over whole trie, performance untested
	if (type != varObj) return undefined;
	keyval *u = (keyval*) ref->data;
	trie4d::Trie4d<int> &trie = u->trie;
	var data = Object;
	data["result"] = Array;
	trie.forEach(trieEnumerator, (void*) &data);
//	recursiveScanTrie(&trie.root, result, curPath);
//	if (trie4d::B == 4) result = decode16bitArray(result);
//	if (trie4d::B == 5) result = decode32bitArray(result);
	return data["result"];
}

void var::deleteObj() {
	delete (keyval*)ref->data, delete ref;
}

void var::del(var key) {
	if (type != varObj) return;
	keyval *u = (keyval*) ref->data;
	key = key.toString();

	bool found = findNode(u->trie, key);

	if (found) {
		int v = *u->trie.result;
		*u->trie.result = -1;
		u->vals[v] = undefined;
		u->vals[v].type = varDeleted;
		u->deleted++; // can be reused later
	}
}
