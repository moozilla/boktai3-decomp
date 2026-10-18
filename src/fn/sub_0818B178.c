#define Armor_UpdateAccumulatedBehavior sub_0818B178
#include "global.h"

struct ArmorPart {
    u8 pad[0x38];
    u32 flags;
};
struct ArmorActor {
    u8 pad0[0x190];
    struct ArmorPart part0;
    u8 pad1[0x1e4 - 0x1cc];
    struct ArmorPart part1;
    u8 pad2[0xb26 - 0x220];
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

static inline void setPartFlags(struct ArmorPart *p, u32 flags)
{
    p->flags |= flags;
}

void sub_0818B178(struct ArmorActor *p, s32 amount, u32 flags)
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
        setPartFlags(&p->part1, 0x400008);
        setPartFlags(&p->part0, 0x400008);
    }
}
