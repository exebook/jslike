struct Rc {
	var *ref;
	int count;
};

struct arrset {
	Rc *rc;
	var *vr;

	arrset () {
		rc = new Rc;
		rc->ref = new var;
		rc->count = 1;
	}
	arrset (var *a, int dummy) {
		//dummy is to remove "ambiguous" conflict
		rc = 0;
		vr = a;
	}
	arrset(arrset &a) {
		if (a.rc) {
			rc = a.rc;
			rc->count++;
		}
		else vr = a.vr, rc = 0;
	}
	~arrset() {
		if (rc && rc->ref && --rc->count == 0) {
			delete rc->ref;
			delete rc;
		}
	}
	var *get() {
		if (rc) return rc->ref;
		return vr;
	}
	operator var& (){
		return *get();
	}
	arrset &operator , (var b) {
		if (get()->type == varArr) get()->push(b);
		return self;
	}
	arrset &operator = (var b) {
		(*get()) = arr;
		get()->push(b);
		return self;
	}
};

arrset Arr() {
	arrset R(this, 1);
	return R;
}

struct objset {
	Rc *rc;
	var *vr;

	objset () {
		rc = new Rc;
		rc->ref = new var;
		rc->count = 1;
	}
	objset (var *a) {
		rc = 0;
		vr = a;
	}
	objset(objset &a) {
		if (a.rc) {
			rc = a.rc;
			rc->count++;
		}
		else vr = a.vr, rc = 0;
	}
	~objset() {
		if (rc && rc->ref && --rc->count == 0) {
			delete rc->ref;
			delete rc;
		}
	}
	var *get() {
		if (rc) return rc->ref;
		return vr;
	}
	void set(var *a) {
		if (rc) rc->ref = a;
		vr = a;
	}
	operator var& (){
		return *get();
	}

	chr *key;
	stk <void*> stack;

	var& __value() {
		var t;
		t.makeStringToSet();
		t._chr().set(key->s, key->size);
		return (*get())[t];
	}
	objset &operator , (varSyntax b) {
		if (b == obj) {
			__value() = obj;
			stack.push(get());
			set(& __value());
			delete key;
			key = 0;
		} else if (b == end) {
			set((var*) stack.pop());
		}
		return self;
	}
	objset &operator , (var b) {
		if (get()->type == varObj) {
			if (key) {
				__value() = b;
				delete key;
				key = 0;
			} else {
				key = new chr;
				b = b.toString();
				key->set(b._chr().s, b._chr().size);
			}
		}
		return self;
	}
	objset &operator = (var b) {
		(*get()) = obj;
		key = new chr;
		b = b.toString();
		key->set(b._chr().s, b._chr().size);
		return self;
	}
};

objset Obj() {
	objset R(this);
	return R;
}


