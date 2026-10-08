#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08202A88(void *);
void sub_0821A0C0(void *);
void sub_08202730(void);
void sub_08202A4C(void);
void *sub_08202ABC(void){ void *p = sub_08219FBC(8,0x129C); if (p) { sub_0821A04C(p,sub_08202730,sub_08202A4C); if (sub_08202A88(p) >= 0) goto ok; sub_0821A0C0(p); return 0; } ok: return p; }
