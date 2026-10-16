#include "global.h"

extern const u8 gUnk_0824F67C[];
void sub_0824923C(u8 *, u32);
s32 Collider_Refresh(u8 *);
void Collider_SetPosition(u8 *, u8 *, u32);

void sub_08198B08(u8 *p)
{
    const u32 *tbl = (const u32 *)gUnk_0824F67C;
    sub_0824923C(p, tbl[*(p + 0x157)]);
    Collider_Refresh(p + 0xF4);
    Collider_SetPosition(p + 0xF4, p + 0x48, 0);
}
