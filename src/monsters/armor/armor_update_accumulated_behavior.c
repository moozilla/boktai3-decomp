#include "engine/shared/global.h"
#include "engine/shared/contact_collider.h"

struct ArmorActor {
    u8 pad0[0x190];
    struct ContactCollider part0;
    struct ContactCollider part1;
    u8 pad238[0xB26 - 0x238];
    s16 actionValue;
    u8 pad3[0xb32 - 0xb28];
    u16 flags;
    u8 pad4[0xb3d - 0xb34];
    u8 behaviorPhase;
    u8 pad5[0x1120 - 0xb3e];
    u8 mode;
    u8 triggered;
    s16 modeTimer;
    s16 delay;
    s16 accumulated;
    s16 triggerAccumulated;
    s16 triggerTimer;
    s16 initialActionValue;
};
void ActorController_SetBehavior(struct ArmorActor *, s32);

void Armor_UpdateAccumulatedBehavior(struct ArmorActor *p, s32 amount, u32 flags)
{
    p->accumulated += amount;
    if (flags & 8)
        p->triggerAccumulated += amount;
    p->delay = 420;
    if (p->flags & 1) {
        if (p->mode != 3) {
            if (p->accumulated > 149) {
                p->mode = 3;
                p->modeTimer = 120;
                p->accumulated = 0;
            } else {
                p->mode = 2;
            }
        } else {
            p->modeTimer = 120;
        }
    } else {
        p->mode = 2;
    }
    if (p->triggered) {
        ActorController_SetBehavior(p, 1);
        p->behaviorPhase = 255;
    } else if (p->triggerAccumulated > 249) {
        s32 value;
        s32 scaled;
        p->triggered = 1;
        p->triggerTimer = 600;
        p->triggerAccumulated = 0;
        value = p->initialActionValue * 5120;
        if (value >= 0)
            scaled = value >> 12;
        else
            scaled = -(-value >> 12);
        p->actionValue = scaled;
        ContactCollider_SetReactionFlags(&p->part1, 0x400008);
        ContactCollider_SetReactionFlags(&p->part0, 0x400008);
    }
}
