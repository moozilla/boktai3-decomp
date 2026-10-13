#include "global.h"
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
u32 ScriptProc_Start(u32, u32);
void AnimData_SelectTiles(void *, u32);
void sub_081147BC(u8 *s)
{
    u8 *p = s + 0x44;
    if (AnimData_Load(p, 0x409c) != 0) {
        Sprite_Init(s + 0x60, p, 0);
        *(u32 *)(s + 0x9c) = ScriptProc_Start(0x922e, 0x1c22);
        AnimData_SelectTiles(p, 0xe4);
    }
}
