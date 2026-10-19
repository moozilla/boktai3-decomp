#include "global.h"
#include "player.h"

#ifdef NONMATCHING
// fuzzy: 95.8%
// foundry draft (worker best 95.8%); what differs is mostly register allocation and instruction order
#define Player_GetInputDirection sub_08156BBC
#define gPlayerDirectionByKeys   gUnk_08610B50
extern const s16 gUnk_08610B50[];
static inline u16 MaskKeys(u16 keys, u16 mask)
{
    return keys & mask;
}
s32 sub_08156BBC(struct Player *p)
{
    s32 keys = *p->inputKeys;
    s16 direction = MaskKeys(keys, 0x40) != 0;
    if (keys & 0x80)
        direction |= 2;
    if (keys & 0x20)
        direction |= 4;
    if (0x10 & keys)
        direction |= 8;
    return gUnk_08610B50[direction];
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08156BBC);
#endif
