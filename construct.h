constructor var () {
	type = varNull; ref = 0;
	tmp_charp = 0;
}

~var () {
	unref();
}

constructor var (const var& a) {
	type = varNull; ref = 0;
	copy(a);
}

constructor var (const varSyntax syntax) {
	//type = varNull; //not needed because ref=0 means no unref()
	ref = 0;
	self = syntax;
}
void operator = (const varSyntax syntax) {
	if (ref) unref();
	if (syntax == Array) {
		type = varArr;
		ref = new Ref;
		ref->data = newLst();
	} else if (syntax == Object) {
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
	self = (double) a;
}

constructor var (long a) {
	self = (double) a;
}

constructor var (bool a) {
	type = varBool;
	ref = 0;
	num = (double)a;
}

void operator = (int a) { self = (var)a;}

void operator = (bool a) { self = (var)a;}

void operator = (double a) {
	num = a;
	type = varNum;
}
constructor var (char* a) {
	ref = 0;
	self = a;
}
constructor var (const char* a) {
	ref = 0;
	self = (char*)a;
}

constructor var (jschar* a) {
	ref = 0;
	self = a;
}

constructor var (double a) {
	self = a;
}

void operator = (char* a) {
	if (ref) unref();
	makeStringToSet();
	_chr().setUtf(a);
//	printf("SET '%s'\n", a);
	//_chr().setAscii(a);
}

void setUtf (char* a, int size = -1) {
	if (ref) unref();
	makeStringToSet();
	_chr().setUtf(a, size);
}

void operator = (jschar* a) {
	if (ref) unref();
	makeStringToSet();
	_chr().set(a);
}

void copy(const var &a);

void unref() {
	if (type == varNum || type == varNull || type == varBool) return;
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
	