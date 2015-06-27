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
		(*get()) = Array;
		get()->push(b);
		return self;
	}
};

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
	stk stack;

	var& __value() {
		var t;
		t.makeStringToSet();
		t._chr().set(key->s, key->size);
		return (*get())[t];
	}
	objset &operator , (varSyntax b) {
		if (b == Object) {
			__value() = Object;
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
		(*get()) = Object;
		key = new chr;
		b = b.toString();
		key->set(b._chr().s, b._chr().size);
		return self;
	}
};

objset var::obj() {
	objset R;
	return R;
}

arrset var::arr() {
	arrset R;
	return R;
}

//arrset var::Arr() {
//	arrset R(this, 1);
//	return R;
//}
//
//objset var::Obj() {
//	objset R(this);
//	return R;
//}

//objset (varSyntax) {
	
//}


//maybe that would be better than var::initObj() = 1,2,3,4
//                              var a = lit(arr) = 1,2,3,4;
//var a = lit(obj) = "a",1,"b",2,"c",3,"d",4;
