#include "global.h"

#include "engine.h"
#include "game_state.h"

/* Non-zero when the actor already faces the player along one axis. */

// confidence=low: per-frame chase state machine that steers the actor toward the player's position.
void ActorController_SetBehavior(void *actor, u32 behavior);
void ActorController_SetBehaviorIfFinished(void *actor, u32 behavior);
s32 Armor_IsSequenceIncomplete(void *armor);

struct Sub081879A8Actor {
    u8 unk0[0x48];
    s16 x48, y4A, z4C;
    u8 unk4E[0x292 - 0x4E];
    u16 flags292;
    u8 unk294[0xB32 - 0x294];
    u16 flagsB32;
    u8 unkB34[0xB39 - 0xB34];
    u8 dirB39;
    u8 unkB3A[2];
    u8 kindB3C;
    u8 kindB3D;
    u8 unkB3E[0xB58 - 0xB3E];
    u8 armorB58[0x1120 - 0xB58];
    u8 mode1120;
    u8 flag1121;
    u16 timer1122;
    u8 unk1124[2];
    u16 timer1126;
    u16 timer1128;
};

static inline s32 sub081879A8_Abs(s32 v)
{
    if (v < 0) {
        return -v;
    }
    return v;
}

static inline s32 sub081879A8_Facing(s32 dx, s32 dz)
{
    return (Math_Atan2(dx, dz) + 0x20) & 0xC0;
}

/* Non-zero when the actor already faces the player along one axis. */
static inline int sub081879A8_Aligned(struct Sub081879A8Actor *actor, s32 dxs, s32 dzs)
{
    if (dxs == 0) {
        if (dzs < 0) {
            return actor->dirB39 == 0xC0;
        }
        return actor->dirB39 == 0x40;
    }
    if (dzs != 0) {
        return 0;
    }
    if (dxs >= 0) {
        return actor->dirB39 == 0;
    }
    return actor->dirB39 == 0x80;
}

static inline int sub081879A8_TryAlign(struct Sub081879A8Actor *actor, s32 dxs, s32 dzs)
{
    if ((s16)actor->timer1122 > 0) {
        return 0;
    }
    if (!sub081879A8_Aligned(actor, dxs, dzs)) {
        return 0;
    }
    ActorController_SetBehavior(actor, 0x18);
    actor->timer1122 = 0xB4;
    return 1;
}

#ifdef NONMATCHING
// fuzzy: 35.6%
// scope shape 369 (differences once low registers are set aside)
// confidence=low: per-frame chase state machine that steers the actor toward the player's position.
void sub_081879A8(struct Sub081879A8Actor *actor)
{
    struct GameState *player;
    u16 t;
    s32 px, py, pz, ax, ay, az;
    s32 dxRaw, dyRaw, dzRaw, dxs, dzs, adx, ady, adz, sum, dir;

    t = actor->timer1122;
    if ((s16)t > 0) {
        actor->timer1122 = t - 1;
        if (actor->flag1121) {
            actor->timer1122 = t - 2;
        }
    }
    t = actor->timer1126;
    if ((s16)t > 0) {
        actor->timer1126 = t - 1;
        if (actor->flag1121) {
            actor->timer1126 = t - 2;
        }
    }
    if ((s16)actor->timer1126 > 0 || (actor->flagsB32 & 1)) {
        actor->flags292 |= 4;
    } else {
        actor->flags292 &= ~4;
    }
    actor->timer1128 = actor->timer1128 - 1;
    if (!((s16)actor->timer1128 > 0) && !(actor->flagsB32 & 1)) {
        actor->flag1121 = (actor->flag1121 == 0);
        actor->timer1128 = 0x12C;
    }

    if (actor->kindB3D == 0xFF || actor->kindB3C == 0x13 || actor->kindB3C == 0x21 ||
        actor->kindB3C == 0x20 || actor->kindB3C == 0x22) {
        player = gUnk_02000710;
        px = player->positionX;
        py = player->positionY;
        pz = player->positionZ;
        ax = actor->x48;
        ay = actor->y4A;
        az = actor->z4C;
        dxRaw = px - ax;
        dyRaw = py - ay;
        dzRaw = pz - az;
        dxs = (dxRaw + 0x80) >> 8;
        dzs = (dzRaw + 0x80) >> 8;
        adx = sub081879A8_Abs(dxs);
        ady = sub081879A8_Abs(dyRaw);
        adz = sub081879A8_Abs(dzs);

        if (actor->flagsB32 & 1) {
            if (actor->mode1120 == 1) {
                if (actor->kindB3C == 3) {
                    ActorController_SetBehavior(actor, 0x1E);
                } else {
                    sum = adx + adz;
                    if (sum > 4) {
                        ActorController_SetBehavior(actor, 0x13);
                        actor->mode1120 = 0;
                    } else {
                        dir = sub081879A8_Facing(dxRaw, dzRaw);
                        if (dir == actor->dirB39 && sum <= 1) {
                            ActorController_SetBehavior(actor, 3);
                        } else if (!sub081879A8_TryAlign(actor, dxs, dzs)) {
                            ActorController_SetBehaviorIfFinished(actor, 0x20);
                        }
                    }
                }
            } else if (actor->mode1120 > 1) {
                if (actor->mode1120 == 2) {
                    if (actor->kindB3C == 0xF) {
                        ActorController_SetBehavior(actor, 3);
                    } else {
                        ActorController_SetBehavior(actor, 0x20);
                        actor->mode1120 = 1;
                        actor->timer1122 = 0xB4;
                    }
                } else if (actor->mode1120 == 3) {
                    if (!(((u8)(actor->kindB3C - 10) <= 1) || actor->kindB3C == 0xD)) {
                        ActorController_SetBehavior(actor, 0xA);
                    }
                    if (!((s16)actor->timer1122 > 0)) {
                        if (sub081879A8_Aligned(actor, dxs, dzs) && ady <= 0x7F &&
                            (dxs == 0 ? adz : adx) <= 4) {
                            ActorController_SetBehavior(actor, 0x24);
                        } else {
                            ActorController_SetBehavior(actor, 0x13);
                        }
                        actor->mode1120 = 0;
                    }
                }
            } else if (actor->mode1120 == 0) {
                sum = adx + adz;
                if (sum <= 3) {
                    ActorController_SetBehavior(actor, 0x20);
                    actor->mode1120 = 1;
                    actor->timer1122 = 0xB4;
                } else {
                    ActorController_SetBehaviorIfFinished(actor, 0x13);
                }
            }
        } else {
            if (actor->mode1120 == 1) {
                if (actor->kindB3C == 3) {
                    ActorController_SetBehavior(actor, 0x1E);
                } else {
                    sum = adx + adz;
                    if (sum > 4) {
                        if (actor->flag1121) {
                            ActorController_SetBehavior(actor, 0x21);
                        } else {
                            ActorController_SetBehavior(actor, 0x13);
                        }
                        actor->mode1120 = 0;
                    } else {
                        dir = sub081879A8_Facing(dxRaw, dzRaw);
                        if (dir == actor->dirB39 && sum <= 1) {
                            ActorController_SetBehavior(actor, 3);
                        } else if (!sub081879A8_TryAlign(actor, dxs, dzs)) {
                            if (actor->flag1121) {
                                ActorController_SetBehavior(actor, 0x22);
                            } else {
                                ActorController_SetBehavior(actor, 0x20);
                            }
                        }
                    }
                }
            } else if (actor->mode1120 == 2) {
                if (actor->kindB3C == 0xF) {
                    ActorController_SetBehavior(actor, 3);
                } else {
                    if (actor->flag1121) {
                        ActorController_SetBehavior(actor, 0x22);
                    } else {
                        ActorController_SetBehavior(actor, 0x20);
                    }
                    actor->mode1120 = 1;
                    actor->timer1122 = 0xB4;
                }
            } else if (actor->mode1120 == 0) {
                sum = adx + adz;
                if (sum <= 3) {
                    if (actor->flag1121) {
                        ActorController_SetBehavior(actor, 0x22);
                    } else {
                        ActorController_SetBehavior(actor, 0x20);
                    }
                    actor->mode1120 = 1;
                    actor->timer1122 = 0xB4;
                } else if (Armor_IsSequenceIncomplete(actor->armorB58)) {
                    ActorController_SetBehavior(actor, 8);
                } else {
                    if (actor->flag1121) {
                        ActorController_SetBehavior(actor, 0x21);
                    } else {
                        ActorController_SetBehavior(actor, 0x13);
                    }
                }
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081879A8);
#endif
