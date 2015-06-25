	void operator ++ (int a) {
		if (type == varNum) n++;
		else self = NaN;
	}
//- -on + 0fd
	var operator + (var a) {
		var R;
		if (type == varNum) {
			if (a.type == varNum) {
				return toDouble() + a.toDouble();
			} else {
				var R = NaN;
				return R;
			}
		} else if (type = varStr) {
			if (a.type != varStr) a = a.toString();
			int an = _chr().size, bn = a._chr().size;
			wchar_t 
				* sum = new wchar_t [an + bn],
				* c = sum,
				* A = _chr().s,
				* B = a._chr().s;
			for (int i = 0; i < an; i++) *c++ = A[i];
			for (int i = 0; i < bn; i++) *c++ = B[i];
			R.makeStringToSet();
			chr &C = R._chr();
			C.size = an + bn;
			C.s = sum;
		}
		return R;
	}
//--off +
	var& operator - (var a) {
		if (type == varNum) {
			if (a.type == varNum) {
				n -= a.n;
			} else self = NaN;
		} else if (type = varStr) {
			self = undefined;
		}
		return self;
	}
	var charAt(int n);
	var charCodeAt(int n);

	// <------- handy operations
	
