#include "global.h"
#include "ram.h"

struct S {
    u8 filler[0x18];
    u8 a[0x1c];
    u32 b;
    u32 c;
};
void AnimData_Load(u8 *, u32);
u32 ScriptProc_Start(u32, u32);

s32 DirectionalProjectile_InitPool(struct S *s)
{
    AnimData_Load(s->a, 0x0000D45E);
    s->b = ScriptProc_Start(0x0000922E, 0x00001DC3);
    s->c = 0;
    gMovingSpritePool = s;
    return 0;
}
