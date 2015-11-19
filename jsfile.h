#include <stdio.h>

namespace jsfile { // low-level internal use functions

void deleteFile(char *fileName) {
	FILE *f;
	f = fopen(fileName, "wb");
	fclose(f);
}

bool existsFile(char *fileName) {
	FILE *f = fopen(fileName, "r+b");
	bool b = f != NULL;
	if (b) fclose(f);
	return b;
}

void appendFile(char *fileName, void *data, int size) {
	FILE *f;
	if (existsFile(fileName)) {
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
		if (size < 0) {
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
	if (c == 0) return undefined;
	var R;
	if (encoding == "binary") R.setAscii(c);
	else R = c;
	delete[] fn;
	delete[] c;
	return R;
}

var writeFile(var fileName, var data, var encoding = "utf8") {
	using namespace jsfile;
	int dataSize;
	char *fn, *d;
	fn = fileName.getStringAllocUtf();
	if (encoding == "utf8") {
		d = data.getStringAllocUtf(&dataSize);
	}
	else {
		d = data.getStringAllocAscii(&dataSize);
	}
	if (existsFile(fn)) {
		deleteFile(fn);
	}
	appendFile(fn, d, dataSize);
	delete[] fn;
	delete[] d;
	return undefined;
}

var appendFile(var fileName, var data, var encoding = "utf8") {
	using namespace jsfile;
	int dataSize;
	char *fn, *d;
	fn = fileName.getStringAllocUtf();
	if (encoding == "utf8") {
		d = data.getStringAllocUtf(&dataSize);
	}
	else {
		d = data.getStringAllocAscii(&dataSize);
	}
	appendFile(fn, d, dataSize);
	delete[] fn;
	delete[] d;
	return undefined;
}

var fileExists(var fileName) {
	using namespace jsfile;
	char *fn, *d;
	fn = fileName.getStringAllocUtf();
	bool result = existsFile(fn);
	delete[] fn;
	return result;
}
