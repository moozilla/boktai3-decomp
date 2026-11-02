// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"
#include "game_state.h"
s32 Div(s32, s32);
static inline u32 Bonus(struct Player *p, u32 mask)
{
    u32 *field;
    u32 bits;
    if (!p)
        return 0;
    field = &p->statBonusFlags;
    bits = mask;
    return *field & bits;
}
static inline u32 Both(struct Player *p)
{
    u32 *field;
    register u32 bits asm("r0");
    u32 value;
    if (!p)
        return 1;
    field = &p->statBonusFlags;
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
    struct GameState *save = gGameState;
    if (save->form == 6)
        return;
    if (!Both(gPlayers[0])) {
        if (Bonus(gPlayers[0], 0x8000))
            amount = (amount * 3) >> 1;
        else if (Bonus(gPlayers[0], 0x10000))
            amount = Div(amount, 2);
    }
    {
        s32 *reward = &gGameState->rewardTotal;
        s32 total = *reward + amount;
        *reward = total;
        if (total > 999998)
            gGameState->rewardTotal = 999999;
    }
}
