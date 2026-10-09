#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_080456C8(void *);
void sub_0821A0C0(void *);
void sub_08045574(void);
void sub_08045688(void);
void *sub_08045704(void){ void *p = sub_08219FBC(8,0xB1C); if (p) { sub_0821A04C(p,sub_08045574,sub_08045688); if (sub_080456C8(p) >= 0) goto ok; sub_0821A0C0(p); return 0; } ok: return p; }
