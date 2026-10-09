#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_080447D4(void *);
void sub_0821A0C0(void *);
void sub_08044594(void);
void sub_080447AC(void);
void *sub_08044814(void){ void *p = sub_08219FBC(8,0x1590); if (p) { sub_0821A04C(p,sub_08044594,sub_080447AC); if (sub_080447D4(p) >= 0) goto ok; sub_0821A0C0(p); return 0; } ok: return p; }
