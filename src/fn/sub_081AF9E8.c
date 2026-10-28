#include "global.h"
#include "render.h"

struct A {
    u8 f[0x2c];
    u8 g[0x86];
    s16 h;
};
void TransientSprite_Spawn(void *, void *, s32, s32, s32, s32);

void Vanargand_EmitPartTrail(struct A *p)
{
    if ((*(s32 *)&gEnginePauseFlags) <= 0 && p->h != 0)
        TransientSprite_Spawn(p, (u8 *)p + 0x2c, 8, 0x200, -1, 0);
}
