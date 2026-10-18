#include "global.h"

struct S {
    u8 pad[0x4d];
    u8 f4d;
    u8 pad2[0x68 - 0x4e];
    s32 f68;
};
void Registry6A37_UpdateBackgroundScroll(struct S *);
void sub_081B95E4(u8 *);
void SceneTransition_BeginVectorTween(struct S *);

void Registry6A37_UpdateFirstScrollTransition(struct S *p)
{
    s32 r;
    Registry6A37_UpdateBackgroundScroll(p);
    sub_081B95E4((u8 *)p + 0x24);
    r = 0;
    if (p->f4d == 2)
        r = 1;
    if (r)
        SceneTransition_BeginVectorTween(p);
    p->f68++;
}
