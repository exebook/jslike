struct arrset {
	var *ref, *tmp;
	arrset (var &a) {
		ref = &a;
		tmp = 0;
	}
	~arrset() {
		if (tmp) delete tmp;
	}
	arrset &operator , (var b) {
		if (ref->type == varArr) ref->push(b);
		return self;
	}
	arrset &operator = (var b) {
		(*ref) = arr;
		ref->push(b);
		return self;
	}
	operator var& (){
		return *ref;
	}
};

arrset Arr() {
	arrset R(self);
	return R;
}

struct objset {
	var *ref, *tmp;
	chr *key;
	stk <void*> stack;
	objset (var &a) {
		ref = &a;
		tmp = 0;
		key = 0;
	}
	~objset() {
		if (tmp) delete tmp;
	}
	objset &operator , (varSyntax b) {
		if (b == obj) {
			value() = obj;
			stack.push(ref);
			ref = & value();
			delete key;
			key = 0;
		} else if (b == end) {
			ref = (var*) stack.pop();
		}
		return self;
	}
	var& value() {
		var t;
		t.makeStringToSet();
		t._chr().set(key->s, key->size);
		return (*ref)[t];
	}
	objset &operator , (var b) {
		if (ref->type == varObj) {
			if (key) {
				value() = b;
				delete key;
				key = 0;
			} else {
				key - &b._chr();
				key = new chr;
				key->set(b._chr().s, b._chr().size);
			}
		}
		return self;
	}
	objset &operator = (var b) {
		(*ref) = obj;
		key = new chr;
		key->set(b._chr().s, b._chr().size);
		return self;
	}
	operator var& (){
		return *ref;
	}
};

objset Obj() {
	objset R(self);
	return R;
}


