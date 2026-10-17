#include "global.h"
#include "engine.h"
struct Obj {
    u8 pad0[0x20];
    u16 a[4];
    u16 b[4];
    u32 f30;
    u8 pad34[8];
    u32 f3C;
    u8 pad40[0x18];
    u16 f58, f5A, f5C;
};
void sub_081C0AA0(struct Obj *);
u32 sub_081C3FBC(s32, s32);
s32 sub_081C06C4(struct Obj *);
static inline void fill32(void *dst, u32 *tmp, u32 ctl)
{
    *tmp = 0;
    CpuSet(tmp, dst, ctl);
}
s32 sub_081C0B24(struct Obj *p)
{
    u32 zero;
    u32 e;
    sub_081C0AA0(p);
    p->f30 = sub_081C3FBC(2, 0);
    fill32(p->a, &zero, 0x05000002);
    fill32(p->b, &zero, 0x05000002);
    p->b[0] = 0xE0;
    p->a[0] = 0x140;
    if ((e = Script_SeekToKeyword('e')) != 0)
        e = Script_GetValue();
    p->f3C = e;
    p->f58 = 0x70;
    p->f5A = 0x68;
    p->f5C = 0x64;
    return sub_081C06C4(p);
}
