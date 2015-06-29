#define self (*this)

int len(char *c) {
	int R = 0;
	if (c == 0) return 0;
	while (*c++) R++;
	return R;
}

int len(wchar_t *c) {
	int R = 0;
	if (c == 0) return 0;
	while (*c++) R++;
	return R;
}

wchar_t UTF = 0xFEFF;

int utf_offsets[6] = { 0x0UL, 0x3080UL, 0xE2080UL, 0x3C82080UL, 0xFA082080UL, 0x82082080UL };

unsigned char utf_trail(int i)
{
	if (i < 192) return 0;
   if (i < 224) return 1;
   if (i < 240) return 2;
   if (i < 248) return 3;
   if (i < 252) return 4;
	return 5;
}

int utf2w (unsigned char* src, unsigned char* SB, wchar_t* dst, wchar_t* TB)
{
	 unsigned char* S = src;
	 wchar_t* T = dst;
	 while (S < SB)
	 {
	int ch = 0;
	wchar_t X = utf_trail(*S);
	if (S + X >= SB) break;
		  int x = X;
		  while (x-- > 0) ch += *S++, ch <<= 6;
		  ch += *S++;
	ch -= utf_offsets[X];
	if (T >= TB) { S -= (X+1); break; }
	if (ch <= 0xFFFF)
		  {
		 if (ch >= 0xD800 && ch <= 0xDFFF) *T++ = 0xFFFD;
				else *T++ = (wchar_t)ch;
	}
		  else if (ch > 0x0010FFFF) *T++ = 0xFFFD;
	else
		  {
		 if (T + 1 >= TB) { S -= (X+1); break; }
	    ch -= 0x0010000UL;
	    *T++ = (wchar_t)((ch >> 10) + 0xD800);
		 *T++ = (wchar_t)((ch & 0x3FFUL) + 0xDC00);
	}
    }
    return T - dst;
}

int w2utf(char *D, int DD, wchar_t *S, int SS)
{
  int i = 0, n = 0, c;
  if (S == 0) return 0;
  if (D != 0)
  {
    while ((i < SS) && (n < DD))
    {
      c = S[i++];
      if (c <= 0x7F) D[n++] = c;
      else if (c > 0x7FF)
      {
        if (n + 3 > DD) break;
        D[n] = 0xE0 | (c >> 12);
        D[n + 1] = 0x80 | ((c >> 6) & 0x3F);
        D[n + 2] = 0x80 | (c & 0x3F);
        n += 3;
      }
      else
      {
        if (n + 2 > DD) break;
        D[n] = 0xC0 | (c >> 6);
        D[n + 1] = 0x80 | (c & 0x3F);
        n += 2;
      }
    }
    //if (n > DD) n = DD - 1;
    D[n] = 0;
  }
  else
  {
    while (i < SS)
    {
      c = S[i++];
      if (c > 0x7F) { if (c > 0x7FF) n++; n++; }
      n++;
    }
  }
  return n;
}
