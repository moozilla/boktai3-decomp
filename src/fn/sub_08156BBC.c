#include "global.h"
#include "player.h"

// MATCHING_CONCESSION: register-pinning
#define gPlayerDirectionByKeys gUnk_08610B50
extern const s16 gUnk_08610B50[];
static inline u16 MaskKeys(u16 keys, u16 mask)
{
    return keys & mask;
}
s32 Player_GetInputDirection(struct Player *p)
{
    s32 keys = *p->inputKeys;
    s16 direction = MaskKeys(keys, 0x40) != 0;
    if (keys & 0x80)
        direction |= 2;
    if (keys & 0x20)
        direction |= 4;
    if (0x10 & keys)
        direction |= 8;
    asm("" ::"r"(keys));
    return gUnk_08610B50[direction];
}
