#include "global.h"
#include "player.h"
#include "game_state.h"

// MATCHING_CONCESSION: register-pinning
// confidence=low: refreshes collider flags and response category from player state.
#define Player_RefreshBodyCollision sub_0813B7C4
void Collider_SetBodyProperties(void *, u32, u32, u32);

static inline s32 IsSpecialScene(struct Player *p)
{
    u32 active = p->active;
    s32 result = 0;
    if (active) {
        if (p->sceneMode == 8)
            result = 1;
    }
    return result;
}

void Player_RefreshBodyCollision(struct Player *p)
{
    u32 flags;
    u32 category;
    /* The level-derived body amount is zero in every mode. */
    u32 amount;
    if (p->actionMode == 6) {
        s32 level = gGameState->level + 10;
        if (level > 99)
            level = 99;
        amount = level * 0;
        flags = *(u32 *)((u8 *)p + 0x338) | 2;
        category = 1;
    } else if (p->actionMode == 0) {
        amount = 0;
        flags = *(u32 *)((u8 *)p + 0x338) | 1;
        category = 2;
    } else {
        amount = 0;
        flags = *(u32 *)((u8 *)p + 0x338) | 2;
        category = 1;
    }
    if ((p && IsSpecialScene(p)) || *(u32 *)((u8 *)p + 0x338) == 0x40)
        flags = 0x40;
    asm("" : : "r"(amount));
    Collider_SetBodyProperties(&p->collider, 0, flags, category);
}
