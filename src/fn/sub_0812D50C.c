#include "global.h"
#include "ram.h"

struct S {
    u8 filler[0x18];
    u8 a[0x20];
    u32 c;
};
void AnimData_Load(u8 *, u32);

s32 FragmentSpritePool_Init(struct S *s)
{
    s->c = 0;
    AnimData_Load(s->a, 0x0000F422);
    gScriptedColliderManager = s;
    return 0;
}
