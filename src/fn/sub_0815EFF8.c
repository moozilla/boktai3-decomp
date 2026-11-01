// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"
#include "game_state.h"
#include "engine.h"
#include "ram.h"

#define gRandomTableIndex gRandomIndex
s32 Mod(s32, s32);
void Player_CallEventScript(struct Player *, u32);
void *PlayerBurst_Create(void *);
static inline u16 nextRandom(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}
void PlayerContact_UpdateThresholdEvent(struct Player *p)
{
    u32 zero = gEngineFlags & 0x1000;
    if (zero)
        return;
    if (p->actionMode != 1)
        goto clearReady;
    {
        if ((s32)gGameState->unk600 < gGameState->contactThreshold) {
            goto clearPending;
        } else if (p->contactThresholdReady) {
            if (p->tetherMode <= 1) {
                gGameState->unk600 = zero;
                gGameState->contactThreshold = Mod(nextRandom(), 100) + 200;
                p->contactThresholdReady = zero;
                Player_CallEventScript(p, 2);
                PlayerBurst_Create(p);
            }
        } else if (p->contactThresholdPending) {
            p->contactThresholdReady = 1;
            p->contactThresholdPending = zero;
        }
    }
    return;
clearReady: {
    register u32 off asm("r2") = 0x53C;
    register u8 *q asm("r0");
    asm("" : "+r"(off));
    q = (u8 *)p + off;
    *q = zero;
}
clearPending: {
    u32 off = 0x53B;
    u8 *q;
    q = (u8 *)p + off;
    *q = zero;
}
}
