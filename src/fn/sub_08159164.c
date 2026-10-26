// MATCHING_CONCESSION: register-pinning
#include "global.h"
extern u8 *gGameState;
extern u8 *gUnk_02000580[];
s32 Div(s32, s32);
static inline u32 Bonus(u8 *p, u32 mask)
{
    u32 offset;
    u32 *field;
    u32 bits;
    if (!p)
        return 0;
    offset = 0x444;
    field = (u32 *)(p + offset);
    bits = mask;
    return *field & bits;
}
static inline u32 Both(u8 *p)
{
    u32 *field;
    register u32 bits asm("r0");
    u32 value;
    if (!p)
        return 1;
    bits = 0x444;
    field = (u32 *)(p + bits);
    bits = 0x8000;
    value = *field;
    bits &= value;
    if (!bits)
        return 0;
    bits = 0x10000;
    if (!(value & bits))
        return 0;
    return 1;
}
// confidence=low: enemy and script rewards add capped experience with opposing player bonuses.
#define Player_AddExperience sub_08159164
void Player_AddExperience(s32 input)
{
    s32 amount = input;
    if (*(s16 *)(gGameState + 0x5A8) == 6)
        return;
    if (!Both(gUnk_02000580[0])) {
        if (Bonus(gUnk_02000580[0], 0x8000))
            amount = (amount * 3) >> 1;
        else if (Bonus(gUnk_02000580[0], 0x10000))
            amount = Div(amount, 2);
    }
    {
        s32 total = *(s32 *)(gGameState + 0x48) + amount;
        *(s32 *)(gGameState + 0x48) = total;
        if (total > 999998)
            *(s32 *)(gGameState + 0x48) = 999999;
    }
}
