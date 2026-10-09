#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_0803A434(void *);
void sub_0821A0C0(void *);
void sub_0803A3C8(void);
void sub_0803A408(void);
void *sub_0803A4D4(void){ void *p = sub_08219FBC(11,0xb5c); if (p) { sub_0821A04C(p,sub_0803A3C8,sub_0803A408); if (sub_0803A434(p) >= 0) goto ok; sub_0821A0C0(p); return 0; } ok: return p; }
