
/*
void operator = (const list &n) {
	assign(n);
}

list (const list &n) {
	basic();
	assign(n);
}
void Push(const var &item) {
	T t;
	ins(0, t);
}

void operator ++ (int) {
   resize(size + 1);
}

int operator~ () {
   return size - 1;
}


list<T> & operator < (T item)
{
   ins(0, item);
   return self;
}

list<T> & operator > (T & item)
{
   item = self[0];
   del(0, 1);
}

T operator --(int)
{
   T t = self[-1];
   resize(size - 1);
   return t;
}

T operator --()
{
   T t = self[0];
   del(0, 1);
   return t;
}

void shmasign(list &n)
{
   size = n.size;
   capacity = n.capacity;
   delta = n.delta;
   p = n.p;
   n.p = 0;
   n.size = 0;
   n.capacity = 0;
}

void assign (const list &n)
{
   resize(0);
   resize(n.size);
   for (int i = 0; i < n.size; i++) self[i] = * n.p[i];
}

void del(int x, int count)
{
   if (count == 0) return;
   int c = size - x - count;
   P a = &p[x], b = &p[x + count];
   while (c--)
   {
     if (*a) delete *a; *a++ = *b; *b++ = 0;
   };
   resize(size - count);
}

void ins_from(int x, list <T> &src, int src_x, int count = -1)
{
   if (count < 0) count = !src;
   if (count == 0) return;
   if (x == -1) x = size;
   int c = size - x;
   resize(size + count);
   int a = size - 1, b = size - 1 - count;
   while (c--)
   {
      p[a--] = p[b--];
   };
   T **P = &src.p[src_x];
   for (int i = 0; i < count; i++)
   {
      T *tmp = new T;
      *tmp = **P;
      p[x + i] = tmp;
      P++;
   }  
}
                
void ins(int x, T item)
{
   resize(size + 1);
   int c = size - x;
   P a = &p[size - 1], b = &p[size - 2];
   while (--c) { *a-- = *b; *b = 0; b--; };
   *a = new T;
   **a = item;
}

void addptr(T *item)
{
   resize(size + 1);
   p[size - 1] = item;
}

# ifdef strings_cpp
str dump()
{
  str r;
  for (int i = 0; i < !self; i++) r = r + self[i] + ",";
  return r;
}
# endif

int find(T &what)
{
   for (int i = 0; i < size; i++)
     if (what == *p[i]) return i;
   return -1;
}
*/
