#include "global.h"
#include "player/player.h"

extern const s16 gUnk_08610B50[];

static inline u16 MaskKeys(u16 keys, u16 mask)
{
    return keys & mask;
}

s32 Player_GetInputDirection(struct Player *p)
{
    u32 right;
    s32 keys = *p->inputKeys;
    s16 direction = MaskKeys(keys, 0x40) != 0;
    if (keys & 0x80)
        direction |= 2;
    if (keys & 0x20)
        direction |= 4;
    right = 0x10;
    right &= keys;
    if (right)
        direction |= 8;
    return gUnk_08610B50[direction];
}
