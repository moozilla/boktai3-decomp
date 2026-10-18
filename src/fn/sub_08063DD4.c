#include "global.h"

struct P {
    u32 a, b;
};
void AnimData_Load(u8 *, u32);
void Sprite_Init(u8 *, u8 *, u32);
void SceneBody_SetSprite(u8 *, u8 *);
void AnimData_SelectPaletteBank(u8 *, u32);
u32 ScriptProc_Start(u32, u32);
void RoamingObject_UpdateAnimation(u8 *);

void RoamingObject_InitGraphics(u8 *p)
{
    u8 *a = p + 0x90;
    u8 *b;
    AnimData_Load(a, 0x9247);
    b = p + 0x64;
    Sprite_Init(b, a, 0);
    SceneBody_SetSprite(p + 0x1c, b);
    AnimData_SelectPaletteBank(a, 0x193);
    *(struct P *)(p + 0x80) = *(struct P *)(p + 0x28);
    *(u32 *)(p + 0x120) = ScriptProc_Start(0x922E, 0x1DF8);
    *(u16 *)(p + 0x138) = 7;
    RoamingObject_UpdateAnimation(p);
}
