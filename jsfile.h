#include <stdio.h>

namespace jsfile { // low-level internal use functions

void deleteFile(char *fileName) {
	FILE *f;
	f = fopen(fileName, "wb");
	fclose(f);
}

bool fileExists(char *fileName) {
	FILE *f = fopen(fileName, "r+b");
	bool b = f != NULL;
	if (b) fclose(f);
	return b;
}

void appendFile(char *fileName, void *data, int size) {
	FILE *f;
	if (fileExists(fileName)) {
		f = fopen(fileName, "r+b");
	}
	else {
		f = fopen(fileName, "wb");
	}
	fseek(f, 0, SEEK_END);
	fwrite(data, 1, size, f);
	fclose(f);
}

int fileSize(char *n) {
	FILE *f;
	f = fopen(n, "rb");
	if (f) {
		fseek(f, 0, 2);
		int t;
		t = ftell(f);
		fseek(f, 0, 0);
		fclose(f);
		return t;
	}
	else return -1;
}

char* load(char *fileName) { // you must 'delete' returned pointer
//TODO: rewrite to use size and set into var immediately
	FILE *f = fopen(fileName, "r");
	if (f >= 0) {
		int size = fileSize(fileName);
		if (size == 0) {
			return 0;
		}
		char *s = new char[size];
		int l = fread(s, 1, size, f);
		fclose(f);
		return s;
	}
	else return 0;
}

}

var readFile(var fileName, var encoding = "utf8") {
	var t = fileName;
	char *fn = t.getStringAllocUtf();
	char *c = jsfile::load(fn);
	if (c == 0) return "";
	var R;
	if (encoding == "binary") R.setAscii(c);
	else R = c;
	delete[] fn;
	delete[] c;
	return R;
}
