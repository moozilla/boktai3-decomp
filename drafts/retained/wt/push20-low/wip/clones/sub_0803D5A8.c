#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_0803D514(void *);
void sub_0821A0C0(void *);
void sub_0803D430(void);
void sub_0803D4D0(void);
void *sub_0803D5A8(void){ void *p = sub_08219FBC(8,0xB1C); if (p) { sub_0821A04C(p,sub_0803D430,sub_0803D4D0); if (sub_0803D514(p) >= 0) goto ok; sub_0821A0C0(p); return 0; } ok: return p; }
