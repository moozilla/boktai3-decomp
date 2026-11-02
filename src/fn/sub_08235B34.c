#include "global.h"
#include "game_state.h"
#include "player.h"
#include "health_burst.h"
struct BurstVector {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void sub_08235B34(struct HealthBurstTask *p)
{
    struct BurstVector size, offset;
    void *collider = p->collider;
    s32 amount;
    u32 flags, attackFlags;
    size.x = 30000;
    size.y = 30000;
    size.z = 30000;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    if (!p->kind) {
        flags = 1u << p->owner->movementRequestBlocked;
        amount = p->owner->maximumStatA - p->owner->currentStatA;
        attackFlags = 0x1000;
    } else {
        s32 value;
        if (p->owner->actionMode == 6) {
            value = gGameState->level + 10;
            if (value > 99)
                value = 99;
        } else
            value = gGameState->level;
        flags = 0;
        amount = value * 8;
        attackFlags = 0x1000;
    }
    Collider_Init(collider, 0, 0x3001, 0, (u16)flags, &size, &offset);
    Collider_SetAttackProperties(collider, amount, 10, 0, attackFlags, 30);
    Collider_SetCallback(collider, 0, p);
    Collider_SetPosition(collider, &p->owner->position, 0);
}
