void *newObj() {
	return new keyval;
}

var & var::getObjElement(const var &n) {
	keyval *u = (keyval*) ref->data;
	var &R = u->get(n);
	return R;
}

var var::objectKeys() {
	if (type != varObj) return undefined;
	keyval *u = (keyval*) ref->data;
	return u->keys;
}

void var::deleteObj() {
	delete (keyval*)ref->data, delete ref;
}

void var::del(var key) {
	if (type != varObj) return;
	keyval *u = (keyval*) ref->data;
	key = key.toString();
	int k = u->keys.indexOf(key).toDouble();
	if (k >= 0) {
		u->keys.splice(k, 1);
		u->vals.splice(k, 1);
	}
}
