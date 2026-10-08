#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_0820D5E4(void *);
void sub_0821A0C0(void *);
void sub_0820D2C4(void);
void sub_0820D5AC(void);
void *sub_0820D614(void){ void *p = sub_08219FBC(8,0xB1C); if (p) { sub_0821A04C(p,sub_0820D2C4,sub_0820D5AC); if (sub_0820D5E4(p) >= 0) goto ok; sub_0821A0C0(p); return 0; } ok: return p; }
