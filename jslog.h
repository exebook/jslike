void logVar(var a) {
	var z;
	z = a.toString();
	printf("%s ", z._chr().getUtf());
}

void __Log(var a) {
	int cnt = a.length().toInt();
	for (int i = 0; i < cnt; i++) {
		logVar(a[i]);
	}
	printf("\n");
	fflush(0);
}

#ifdef __BORLANDC__
void log(var a,
	var b = argIgnore, var c = argIgnore, var d = argIgnore,
	var e = argIgnore, var f = argIgnore, var g = argIgnore,
	var h = argIgnore, var i = argIgnore, var j = argIgnore,
	var k = argIgnore, var l = argIgnore, var m = argIgnore,
	var n = argIgnore, var o = argIgnore, var p = argIgnore,
	var q = argIgnore, var r = argIgnore, var s = argIgnore,
	var t = argIgnore, var u = argIgnore, var v = argIgnore,
	var w = argIgnore, var x = argIgnore, var y = argIgnore,
	var z = argIgnore, var A = argIgnore, var B = argIgnore
	)
{
	logVar(a);	
	if (b.type != varIgnore) logVar(b);
	if (c.type != varIgnore) logVar(c);
	if (d.type != varIgnore) logVar(d);
	if (e.type != varIgnore) logVar(e);
	if (f.type != varIgnore) logVar(f);
	if (g.type != varIgnore) logVar(g);
	if (h.type != varIgnore) logVar(h);
	if (i.type != varIgnore) logVar(i);
	if (j.type != varIgnore) logVar(j);
	if (k.type != varIgnore) logVar(k);
	if (l.type != varIgnore) logVar(l);
	if (m.type != varIgnore) logVar(m);
	if (n.type != varIgnore) logVar(n);
	if (o.type != varIgnore) logVar(o);
	if (p.type != varIgnore) logVar(p);
	if (q.type != varIgnore) logVar(q);
	if (r.type != varIgnore) logVar(r);
	if (s.type != varIgnore) logVar(s);
	if (t.type != varIgnore) logVar(t);
	if (u.type != varIgnore) logVar(u);
	if (v.type != varIgnore) logVar(v);
	if (w.type != varIgnore) logVar(w);
	if (x.type != varIgnore) logVar(x);
	if (y.type != varIgnore) logVar(y);
	if (z.type != varIgnore) logVar(z);
	if (A.type != varIgnore) logVar(A);
	if (B.type != varIgnore) logVar(B);
	printf("\n");
	fflush(0);
}


#else

#define ARR(...) (var::initArr() = __VA_ARGS__)
#define OBJ(...) (var::initObj() = __VA_ARGS__)
#define log(...) __Log((var::initArr() = __VA_ARGS__));

#endif
