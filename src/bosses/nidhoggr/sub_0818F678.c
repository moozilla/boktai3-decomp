#include "global.h"

void PlayerStats_AddContactHundredths(u32, u32, s32);

void SceneAnimation_OnEffectContact(u32 a, u32 b, u8 *p)
{
    u8 *q = *(u8 **)(p + 0x184);
    *(p + 0x180) = 0xff;
    PlayerStats_AddContactHundredths(a, b, *(s16 *)(q + 0x7274));
}
