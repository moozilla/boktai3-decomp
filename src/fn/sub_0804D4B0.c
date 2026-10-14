#include "global.h"
extern u32 gUnk_02000488;
void sub_08072C40(u8 *);
void sub_08225938(u8 *);
void DisplayAnimation_Destroy(u8 *);
void Collider_Remove(u8 *);
void TetherActor_RemoveSegments(u8 *);
void TetherActor_RemoveAuxiliarySprite(u8 *);
u32 sub_0804D4B0(u8 *p)
{
    if (p[0x254] != 0)
        sub_08072C40(p + 0x244);
    sub_08225938(p + 0x44);
    DisplayAnimation_Destroy(p + 0x8C);
    Collider_Remove(p + 0x130);
    TetherActor_RemoveSegments(p);
    TetherActor_RemoveAuxiliarySprite(p);
    return gUnk_02000488 = 0;
}
