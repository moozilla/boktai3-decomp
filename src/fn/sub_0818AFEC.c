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
    u8 pad2[0x274 - 0x220];
    s16 contactPower;
    u8 pad3[0xb26 - 0x276];
    s16 attack;
    s16 defense;
    u8 pad4[0xb3d - 0xb2a];
    u8 behaviorPhase;
    u8 pad5[0x1120 - 0xb3e];
    u8 mode;
    u8 triggered;
    s16 timer;
    s16 delay;
    s16 accumulated;
    s16 contactAccumulated;
    s16 contactTimer;
    s16 initialAttack;
    s16 initialDefense;
};
void ActorController_SetBehavior(struct ArmorActor *, s32);
static inline void setPartFlags(struct ArmorPart *p, u32 mask)
{
    p->flags |= mask;
}
// confidence=low: accumulated contacts drive a temporary attack and defense increase.
void Armor_UpdateContactBoost(struct ArmorActor *p, s32 amount, u32 flags)
{
    p->contactAccumulated += amount;
    if (flags & 4)
        p->contactTimer += amount;
    switch (p->mode) {
        case 0:
            if (p->contactAccumulated > 299) {
                p->mode = 1;
                p->timer = 600;
                p->delay = 180;
                p->contactAccumulated = 0;
                p->contactTimer = 0;
            } else {
                p->mode = 2;
                p->timer = 90;
                p->delay = 0;
            }
            break;
        case 1:
            p->delay = 180;
            if (p->contactTimer > 399) {
                p->mode = 3;
                p->timer = 0;
                p->delay = 600;
                p->contactAccumulated = 0;
                p->contactTimer = 0;
                {
                    s32 value, scaled;
                    value = p->initialAttack * 5120;
                    if (value >= 0)
                        scaled = value >> 12;
                    else
                        scaled = -(-value >> 12);
                    p->attack = scaled;
                }
                {
                    s32 value, scaled;
                    value = p->initialDefense * 5120;
                    if (value >= 0)
                        scaled = value >> 12;
                    else
                        scaled = -(-value >> 12);
                    p->defense = scaled;
                }
                p->contactPower = p->defense;
                setPartFlags(&p->part1, 0x200004);
                setPartFlags(&p->part0, 0x200004);
            }
            break;
        case 3:
            p->mode = 4;
            p->timer = 90;
            p->contactAccumulated = 0;
            p->contactTimer = 0;
            ActorController_SetBehavior(p, 1);
            p->behaviorPhase = 255;
            break;
    }
}
