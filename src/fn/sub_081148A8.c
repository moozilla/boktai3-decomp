#include "global.h"
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
u32 ScriptProc_Start(u32, u32);
void AnimData_SelectTiles(void *, u32);
void Anim_SetSequence(void *, u32, u32, u32, u32);
void sub_081148A8(u8 *s, s32 x)
{
    u8 *p = s + 0x44;
    if (AnimData_Load(p, 0x409c) != 0) {
        u32 r;
        u8 *q;
        Sprite_Init(s + 0x60, p, 0);
        r = ScriptProc_Start(0x922e, 0x1c22);
        q = s + 0x9c;
        *(u32 *)q = r;
        Anim_SetSequence(q - 0x10, r, 8, 0, 0);
        AnimData_SelectTiles(p, 0xe4);
        if (x == 0) {
            *(u8 *)(s + 0xca) = 0xc0;
            *(s32 *)(s + 0xd0) = 1;
        } else {
            u8 *b = s + 0xca;
            s32 c = 0x60;
            *b = c;
            *(s32 *)(b + 6) = c - 0x61;
        }
    }
}
