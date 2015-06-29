bool operator == (var a, var b) {
	if (a.type == varNum && b.type == varNum) {
		return a.num == b.num;
	}
	if (a.type == varStr && b.type == varStr) {
		chr &A = a._chr();
		chr &B = b._chr();
		return a._chr().cmp(b._chr()) == 0;
	}
	return false;
}

bool operator != (var a, var b) {
	return !(a == b);
}

bool operator < (var a, var b) {
	if (a.type == varNum && b.type == varNum) {
		return a.num < b.num;
	}
	if (a.type == varStr && b.type == varStr) {
		chr &A = a._chr();
		chr &B = b._chr();
		return a._chr().cmp(b._chr()) < 0;
	}
	return false;
}

bool operator > (var a, var b) {
	if (a == b) return false;
	if (a < b) return false;
	return true;
}

