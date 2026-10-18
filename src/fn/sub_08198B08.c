#include "global.h"

extern const u8 gUnk_0824F67C[];
void Callback_InvokeActor(u8 *, u32);
s32 Collider_Refresh(u8 *);
void Collider_SetPosition(u8 *, u8 *, u32);

void SceneEffect_UpdateActor(u8 *p)
{
    const u32 *tbl = (const u32 *)gUnk_0824F67C;
    Callback_InvokeActor(p, tbl[*(p + 0x157)]);
    Collider_Refresh(p + 0xF4);
    Collider_SetPosition(p + 0xF4, p + 0x48, 0);
}
