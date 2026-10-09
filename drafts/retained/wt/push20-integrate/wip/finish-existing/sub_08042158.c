#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08042068(void *);
void sub_0821A0C0(void *);
void sub_08041F3C(void);
void sub_08042044(void);
void *sub_08042158(void){ void *p = sub_08219FBC(4,0x1088); if (p) { sub_0821A04C(p,sub_08041F3C,sub_08042044); if (sub_08042068(p) >= 0) goto ok; sub_0821A0C0(p); return 0; } ok: return p; }
