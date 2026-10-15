#include "global.h"

struct P {
    u32 a, b;
};
void AnimData_Load(u8 *, u32);
void Sprite_Init(u8 *, u8 *, u32);
void sub_08225B28(u8 *, u8 *);
void AnimData_SelectPaletteBank(u8 *, u32);
u32 ScriptProc_Start(u32, u32);
void RoamingObjectB_UpdateAnimation(u8 *);

void sub_08064460(u8 *p)
{
    u8 *a = p + 0x90;
    u8 *b;
    AnimData_Load(a, 0x9247);
    b = p + 0x64;
    Sprite_Init(b, a, 0);
    sub_08225B28(p + 0x1c, b);
    AnimData_SelectPaletteBank(a, 0x193);
    *(struct P *)(p + 0x80) = *(struct P *)(p + 0x28);
    *(u32 *)(p + 0x120) = ScriptProc_Start(0x922E, 0x1DF8);
    *(u16 *)(p + 0x138) = 7;
    RoamingObjectB_UpdateAnimation(p);
}
