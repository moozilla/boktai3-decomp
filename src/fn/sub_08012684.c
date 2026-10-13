#include "global.h"

struct E {
    u8 act;
    u8 f[0x9f];
};
struct S {
    u8 f[0x18];
    u8 a[0x20];
    struct E e[8];
};
extern struct S *gUnk_02000050;
u32 ScriptProc_Start(u32, u32);
s32 AnimData_Load(void *, u32);
void AnimatedHazard_Initialize(void *, void *);

s32 sub_08012684(struct S *s)
{
    struct E *e;
    s32 i;
    gUnk_02000050 = s;
    *(u32 *)((u8 *)s + 0x34) = ScriptProc_Start(0x922E, 0xE92A);
    if (AnimData_Load(s->a - 0, 0xCD6C) == 0)
        return -1;
    e = s->e;
    for (i = 7; i >= 0; i--, e++) {
        AnimatedHazard_Initialize(s, e);
    }
    return 0;
}
