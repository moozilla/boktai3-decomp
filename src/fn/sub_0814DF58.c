// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"
#include "game_state.h"

#define EN_OFF   ((u32) & ((struct Player *)0)->statModifierContext.statBOverrideEnabled)
#define MODE_OFF ((u32) & ((struct Player *)0)->statModifierContext.statBOverrideMode)
#define LOADB(var, reg, off, p)                                                                    \
    {                                                                                              \
        register u32 o asm(reg) = off;                                                             \
        register u8 *q asm("r0");                                                                  \
        asm("" : "+r"(o));                                                                         \
        q = (u8 *)(p) + o;                                                                         \
        var = *q;                                                                                  \
    }
void Player_SubtractStatB(struct Player *p, s32 amount)
{
    s32 value;
    u32 en, mode;
    LOADB(en, "r2", EN_OFF, p);
    if (en) {
        LOADB(mode, "r2", MODE_OFF, p);
        if (mode == 7) {
            value = gGameState->unk77C;
            goto have;
        }
    }
    value = p->currentStatB;
have:
    if (value < amount)
        value = 0;
    else
        value -= amount;
    LOADB(en, "r1", EN_OFF, p);
    if (en) {
        LOADB(mode, "r1", MODE_OFF, p);
        if (mode == 7) {
            gGameState->unk77C = value;
            return;
        }
    }
    p->currentStatB = value;
}
