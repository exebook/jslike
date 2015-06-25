void *newLst() {
	return new lst;
}

void var::deleteLst() {
	delete (lst*)ref->data, delete ref;
}

var var::splice(var start, var deleteCount, var item = argIgnore) {
	var R = arr;
	lst &L = *(lst*) ref->data;
	int
		x = start.toDouble(),
		d = deleteCount.toDouble(),
		size = length().toDouble();
	if (d < 0) d = 0;
	if (x >= size) return R;
	if (x + d > size) d = size - x;
	for (int i = x; i < x + d; i++) R.push(L[i]);
	int N = 1;
	if (item.type == varIgnore) N = 0;
	L.delIns(x, d, &item, N);
	return R;
}

var var::join(var separator) {
	var R;
	lst L;
	int cnt = length().toDouble();
	if (cnt == 0) {
		return (var)"";
	}
	int all = 0;
	for (int i = 0; i < cnt; i++) {
		var t = self[i].toString();
		all += t.length().toDouble();
		L.push(t);
	}
	int seplen = (int)separator.length().toDouble();
	int Rsize = all + (cnt-1) * seplen;

	R.makeStringToSet();
	R._chr().set(0, Rsize);
	int x = 0;
	for (int i = 0; i < cnt; i++) {
		int subcnt = (int)L[i].length().toDouble();
		L[i]._chr()._cpto(0, R._chr(), x, subcnt);
		x += subcnt;
		if (i == cnt - 1) break;
		separator._chr()._cpto(0, R._chr(), x, seplen);
		x += seplen;
	}
	return R;
}

var var::slice(var start, var end = undefined) {
	// needs more testing
	int 
		size = length().toDouble(),
		a = start.toDouble(),
		b = end.toDouble();

	if (end == undefined) b = size;
		else if (b < 0) b += size;
		else if (b > size) b = size;
		
	if (a < 0) a += size;

	if (type == varStr) {
		var R;
		R.makeStringToSet();
		R._chr().set(&_chr().s[a], b-a);
		return R;
	}
	if (type != varArr) return undefined;
	var R = arr;
	if (b <= a) { return R; }
	lst &L = * (lst*) R.ref->data;
	
	L.resize(b - a);
	for (int i = 0; i < L.size; i++) L[i] = self[a + i];
	return R;
}

var var::push(var a) {
	lst *L = (lst*) ref->data;
	L->push(a);
	return a;
}

var var::pop() {
	lst *L = (lst*) ref->data;
	return L->pop();
}

var var::Push(var a) {
	lst *L = (lst*) ref->data;
	L->delIns(0, 0, &a, 1);
	return a;
}

var var::Pop() {
	lst *L = (lst*) ref->data;
	var R = (*L)[0];
	L->delIns(0, 1, 0, 0);
	return R;
}

var var::length() {
	if (type == varArr) {
		lst *L = (lst*) ref->data;
		return L->size;
	}
	if (type == varStr) {
		return _chr().size;
	}
	return undefined;
}

var& var::getArrElement(int n) {
	lst *L = (lst*) ref->data;
	return (*L)[n];
}
