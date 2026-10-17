#include "global.h"

struct S {
    u8 f[0x50];
    s32 a;
    u32 b;
};
void SceneCommands_Dispatch(struct S *);
void sub_082260A4(u32);
u32 sub_08021360(struct S *p)
{
    SceneCommands_Dispatch(p);
    if (p->a != 0) {
        if (p->a > 0)
            p->a--;
        sub_082260A4(p->b);
    }
    return 0;
}
