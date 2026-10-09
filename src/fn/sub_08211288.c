#include "global.h"
struct Object {u8 data[0x2c];};struct Entry {u8 data[0x60];};
void sub_08214514(void*);void sub_082195E0(void*);
void sub_08211288(u8 *s)
{
 struct Object *obj=(struct Object*)(s+0x65c);
 struct Entry *p;
 void *a,*b;
 int i=26;
 do {sub_08214514(obj);obj++;i--;} while(i>=0);
 a=s+0xa4;b=s+0x44;
 p=(struct Entry*)(s+0x1c4);i=3;
 do {sub_082195E0(p);p++;i--;} while(i>=0);
 sub_082195E0(s+0x164);sub_082195E0(s+0x104);
 sub_082195E0(a);sub_082195E0(b);
}
