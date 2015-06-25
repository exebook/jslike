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

