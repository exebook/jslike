

char* shape(wchar_t *w, int wsize, int &rsize) {
	char* u = new char[wsize*4];
	rsize = w2utf(u, wsize*4, w, wsize);
	return u;
}

bool addToTrie(Trie16 &trie, var key, var value) {
	wchar_t *w = key._chr().s;
	int size = key._chr().size;

	int usize;
	char *u = shape(w, size, usize);
	int intval = value.toInt();
	bool ok =  trie.add(u, usize, intval);
	delete[] u;
	return ok;
}

Node *findNode(Trie16 &trie, var key) {
	wchar_t *w = key._chr().s;
	int size = key._chr().size;

	int usize;
	char *u = shape(w, size, usize);

	Node *R = trie.findNode(u, usize);
	delete[] u;
	return R;
}

struct keyval {
	Trie16 trie;
	var vals;
	int deleted;
	keyval () {
		vals = Array;
		deleted = 0;
	}
	var &get(var key) {
		key = key.toString();
		Node *n = findNode(trie, key);
		int k;
		if (n) {
			k = n->value;
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
			addToTrie(trie, key, k);
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

void recursiveScanTrie(Node *C, var result, var curPath) {
	if (C->value != -1) {
		var nul = Array;
		result.push(curPath.concat(nul));
	}
	for (int i = 0; i < 16; i++) {
		if (C->N[i]) {
			curPath.push(i);
			recursiveScanTrie(C->N[i], result, curPath);
			curPath.pop();
		}
	}
}

var decode16bitArray(var e) {
	var d = Array;
	for (var i = 0; i < e.length(); i++) {
		var line = e[i];
		var s = "";
		for (var j = 0; j < line.length(); j+=2) {
			int a = line[j].toInt(), b = line[j+1].toInt();
			s += var::fromCharCode(a + (b << 4));
		}
		d.push(s);
	}
	return d;
}

var var::objectKeys() {
	// iterate over whole trie, performance untested
	if (type != varObj) return undefined;
	keyval *u = (keyval*) ref->data;
	Trie16 &trie = u->trie;
	var result = Array;
	var curPath = Array;
	recursiveScanTrie(&trie.root, result, curPath);
	result = decode16bitArray(result);
	return result;
}

void var::deleteObj() {
	delete (keyval*)ref->data, delete ref;
}

void var::del(var key) {
	if (type != varObj) return;
	keyval *u = (keyval*) ref->data;
	key = key.toString();

	Node *n = findNode(u->trie, key);

	if (n) {
		n->value = -1;
		u->vals[n->value] = undefined;
		u->vals[n->value].type = varDeleted;
		u->deleted++; // can be reused later
	}
}
