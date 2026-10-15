#include "global.h"
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
u32 ScriptProc_Start(u32, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Anim_SetSequence(void *, u32, u32, u32, u32);
void sub_081146C0(u8 *s, s32 x)
{
    u8 *p = s + 0x44;
    if (AnimData_Load(p, 0x409c) != 0) {
        u32 r;
        u8 *q;
        Sprite_Init(s + 0x60, p, 0);
        r = ScriptProc_Start(0x922e, 0x1c22);
        q = s + 0x9c;
        *(u32 *)q = r;
        if (x > 0x3c) {
            Anim_SetSequence(q - 0x10, r, 5, 2, 0);
        } else {
            Anim_SetSequence(s + 0x8c, r, 5, 0, 0);
        }
        AnimData_SelectPaletteBank(s + 0x44, 0xe4);
    }
}
