#include "global.h"
s32 AnimData_Load(u8 *, u32);
void Sprite_Init(u8 *, u8 *, u32);
u32 ScriptProc_Start(u32, u32);
void Anim_SetSequence(u8 *, u32, u32, u32, u32);
void AnimData_SelectPaletteBank(u8 *, u32);
void sub_08114664(u8 *p)
{
    u8 *q = p + 0x44;
    if (AnimData_Load(q, 0x409C) != 0) {
        u32 r;
        u8 *t;
        Sprite_Init(p + 0x60, q, 0);
        r = ScriptProc_Start(0x922E, 0x1C22);
        *(u32 *)(p + 0x9c) = r;
        Anim_SetSequence(p + 0x8c, r, 5, 0, 0);
        AnimData_SelectPaletteBank(q, 0xe4);
    }
}
