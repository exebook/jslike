
void logVar(var a) {
	var z;
	z = a.toString();
	printf("%s ", z._chr().getUtf());
}

void log(var a, 
	var b = argIgnore, var c = argIgnore, var d = argIgnore,
	var e = argIgnore, var f = argIgnore, var g = argIgnore,
	var h = argIgnore, var i = argIgnore, var j = argIgnore,
	var k = argIgnore, var l = argIgnore)
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
	printf("\n");
	fflush(0);
}

