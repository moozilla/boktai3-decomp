#include "global.h"
struct Object {u8 data[0x2c];};struct Entry {u8 data[0x60];};
void sub_08033590(void);
void sub_08214514(void*);void sub_082195E0(void*);
void sub_08213810(u8 *s)
{
 struct Object *obj=(struct Object*)(s+0x500);
 struct Entry *p;
 void *a,*b;
 int i=9;
 do {sub_08214514(obj);obj++;i--;} while(i>=0);
 sub_082195E0(s+0x364);
 a=s+0xc4;b=s+0x64;
 p=(struct Entry*)(s+0x1e4);i=3;
 do {sub_082195E0(p);p++;i--;} while(i>=0);
 sub_082195E0(s+0x184);sub_082195E0(s+0x124);
 sub_082195E0(a);sub_082195E0(b);
 sub_08033590();
}
