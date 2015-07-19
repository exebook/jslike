//--on [] eef
var &var::operator [] (var a) {
	if (type == varNull || type == varNum) { 
		log((var)"FATAL ERROR: Cannot access property \""
			+ a +"\" of non-Array \"" + self + "\"");
//		log("The program will now terminate.");
		exit(0);
		static var R; 
		return R;
	}
	else if (type == varStr) {
		int n = a.toDouble();
		chr &C = _chr();
		if (n >= C.size) {
			return Undefined();
		}
		jschar c = C.s[n];
		static var R = "1";
		R._chr().s[0] = c;
		return R;
	}
	else if (type == varArr) {
		int n = a.toDouble();
		return getArrElement(n);
	}
	else if (type == varObj) {
		return getObjElement(a);
	} else {
		printf("unknown type\n");
	}
	static var R;
	R = undefined;
	return R;
}
//--off []
