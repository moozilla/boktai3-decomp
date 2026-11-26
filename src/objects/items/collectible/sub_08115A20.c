#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/engine_pause.h"
struct S {
    u8 f[0xee];
    u8 st;
};
struct T {
    u32 a;
    u32 b;
};
void GameState_AddPickupRewardCount(u32);
void Collectible_ApplyPickupReward(struct S *s, struct T *t)
{
    if (Engine_PauseBits(0x800) == 0 && t->b == 1) {
        switch (s->st) {
            case 0:
                GameState_AddPickupRewardCount(1);
                break;
            case 1:
                GameState_AddPickupRewardCount(2);
                break;
        }
    }
}
