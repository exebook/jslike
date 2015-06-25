var var::charAt(int n) {
	if (type != varStr) {
		static var R;
		return R;
	}
	chr &C = _chr();
	if (n >= C.size) return Undefined();
	wchar_t c = C.s[n];
	var R = "1";
	R._chr().s[0] = c;
	return R;
}

var var::charCodeAt(int n) {
	if (type != varStr) {
		static var R;
		return R;
	}
	chr &C = _chr();
	if (n >= C.size) return Undefined();
	wchar_t c = C.s[n];
	var R = (int)c;
	return R;
}

