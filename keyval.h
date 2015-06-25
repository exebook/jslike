struct keyval {
	var keys, vals;
	keyval () {
		keys = arr;
		vals = arr;
	}
	void set(var key, var val) {
		key = key.toString();
		int k = keys.indexOf(key).toDouble();
		if (k < 0) {
			keys.push(key);
			vals.push(val);
		} else {
			vals[k] = val;
		}
	}
	var &get(var key) {
		key = key.toString();
		int k = keys.indexOf(key).toDouble();
		if (k < 0) {
			k = keys.length().toDouble();
			keys[k] = key;
		}
		return vals[k];
	}
};

