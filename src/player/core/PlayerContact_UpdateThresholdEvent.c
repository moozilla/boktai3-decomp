#include "global.h"
#include "player/player.h"
#include "system/game_state/game_state.h"
#include "engine/engine.h"
#include "engine/ram.h"

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
    if (gEngineFlags & 0x1000)
        return;
    if (p->actionMode == 1) {
        if ((s32)gGameState->unk600 >= gGameState->contactThreshold) {
            if (p->contactThresholdReady) {
                if (p->tetherMode <= 1) {
                    gGameState->unk600 = 0;
                    gGameState->contactThreshold = Mod(nextRandom(), 100) + 200;
                    p->contactThresholdReady = 0;
                    Player_CallEventScript(p, 2);
                    PlayerBurst_Create(p);
                }
            } else if (p->contactThresholdPending) {
                p->contactThresholdReady = 1;
                p->contactThresholdPending = 0;
            }
        } else {
            p->contactThresholdPending = 0;
        }
    } else {
        p->contactThresholdReady = 0;
        p->contactThresholdPending = 0;
    }
}
