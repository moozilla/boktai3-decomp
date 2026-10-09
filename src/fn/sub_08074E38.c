#include "global.h"
typedef struct { u8 pad[0x50]; u32 value; u8 pad2[8]; u8 data[0x58]; u16 a[4]; u16 b[4]; u16 c[4]; u32 d[4]; } Obj;
void sub_0821FEB4(void *, u16, u32, u32, u32, u32, u32);
void sub_0821FF58(void *, u16, u16, u32, u32, u16);
void sub_0821FF24(void *, void *, u32);
void sub_0821FF84(void *, u32, u32);
void sub_08074E38(Obj *p, void *dst, u32 a, u32 b, u32 c, s32 i, u32 d, u32 e) {
    u32 flags = 0x2001;
    u16 aa, bb, cc;
    u32 dd;
    sub_0821FEB4(dst, p->value, flags, 0, 0x10, a, b);
    aa = p->a[i];
    bb = p->b[i];
    dd = p->d[i];
    cc = p->c[i];
    sub_0821FF58(dst, aa, bb, d, dd, cc);
    sub_0821FF24(dst, p->data, 0);
    sub_0821FF84(dst, c, e);
}
