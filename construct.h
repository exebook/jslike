constructor var () {
	id=_id++; type = varNull; ref = 0;
}
  ~var () { unref(); }
constructor var (const var& a) {
	id=_id++; 
	type = varNull; ref = 0;
	copy(a);
}

constructor var (const varSyntax syntax) {
	id=_id++; 
	//type = varNull; //not needed because ref=0 means no unref()
	ref = 0;
	self = syntax;
}
void operator = (const varSyntax syntax) {
	if (ref) unref();
	if (syntax == arr) {
		type = varArr;
		ref = new Ref;
		ref->data = newLst();
	} else if (syntax == obj) {
		type = varObj;
		ref = new Ref;
		ref->data = newObj();
	} else if (syntax == argIgnore) {
		type = varIgnore;
	} else {
		type = varNull;
	}
		//TODO: other types (and wrong values like NaN, undefined)
}

void operator = (const var &a) {
	unref();
	copy(a);
}

constructor var (int a) {
	id=_id++; 
	self = (double) a;
}
void operator = (int a) { self = (double)a;}

void operator = (double a) {
	n = a;
	type = varNum;
}
constructor var (char* a) {
	id=_id++; 
	ref = 0;
	self = a;
}

constructor var (double a) {
	id = _id++;
	self = a;
}

void operator = (char* a) {
	if (ref) unref();
	makeStringToSet();
	_chr().setUtf(a);
	//_chr().setAscii(a);
}

void copy(const var &a);

void unref() {
	if (type == varNull || type == varNum) return;
	else if (type == varStr) {
		ref->uses--;
		if (ref->uses == 0) {
			delete (chr*)ref->data, delete ref;
		}
	}
	else if (type == varArr) {
		ref->uses--;
		if (ref->uses == 0) {
			deleteLst();
		}
	}
	else if (type == varObj) {
		ref->uses--;
		if (ref->uses == 0) {
			deleteObj();
		}
	}
	type = varNull;
	ref = 0;
}
	