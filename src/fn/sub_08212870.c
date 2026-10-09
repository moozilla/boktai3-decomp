#include "global.h"
struct Entry {u8 data[0x60];};
void sub_08214514(void*);void sub_0821A0C0(void*);void sub_082195E0(void*);
void sub_08212870(u8 *s)
{
 struct Entry *p;
 void *a,*b;
 int i;
 sub_08214514(s+0xb10);
 sub_0821A0C0(*(void**)(s+0xb5c));
 sub_0821A0C0(*(void**)(s+0xb58));
 sub_082195E0(s+0xa88);
 p=(struct Entry*)(s+0x3a8); i=17;
 do {sub_082195E0(p);p++;i--;} while(i>=0);
 a=s+0xc4;b=s+0x64;
 p=(struct Entry*)(s+0x1e4);i=3;
 do {sub_082195E0(p);p++;i--;} while(i>=0);
 sub_082195E0(s+0x184);sub_082195E0(s+0x124);
 sub_082195E0(a);sub_082195E0(b);
}
