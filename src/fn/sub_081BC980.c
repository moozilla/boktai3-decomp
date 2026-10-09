#include "global.h"
void sub_082164AC(int,void *,int);
void sub_0821656C(int,int,int,int,int);
void sub_081BC664(void *);
void sub_0822B2F8(int);
void sub_08220D78(void *,void *,int,int,int);
void sub_08030DBC(int);
void sub_081BD084(void);
void sub_081BC694(void *,void (*)(void));
void sub_081BC980(u8 *p) {
 int z;
 sub_082164AC(0,*(void **)(p+0x8e0),5);
 z=0; sub_0821656C(0,0,0,0,z); sub_081BC664(p); sub_0822B2F8(0x2c1);
 *(u32 *)(p+0x60)&=~1;
 sub_08220D78(p+0x58,p+0x18,6,2,z); sub_08030DBC(3); sub_081BC694(p,sub_081BD084);
}
