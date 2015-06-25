struct classJSON {

	var stringifyObj(var a) {
		var keys = a.objectKeys();
		var R = arr;
		for (var i = 0; i < keys.length(); i++) {
			var &ref = a[keys[i]];
			var q = "";
			if (ref.type == varStr) q = "\"";
			var s;
			s = stringify(ref);
			R.push( keys[i] + ": " + q + s + q );
		}
		return (var) "{ " + R.join(", ") + " }";
	}
	
	var stringifyArr(var a) {
		int cnt = a.length().toDouble();
		var R = arr;
		for (var i = 0; i < cnt; i++) {
			var &ref = a[i];
			var q = "";
			if (ref.type == varStr) q = "\"";
			var s;
			s = stringify(ref);
			R.push( q + s + q );
		}
		return (var) "[ " + R.join(", ") + " ]";
	}
	
	var stringify(var a) {
		if (a.type == varArr) return stringifyArr(a);
		if (a.type == varObj) return stringifyObj(a);
		return a.toString();
	}
	
} JSON;
