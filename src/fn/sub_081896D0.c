// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "game_state.h"

struct ArmorPart {
    u8 pad[0x38];
    u32 flags;
};
static inline void Armor_ClearPart(struct ArmorPart *part, u32 bits)
{
    u32 mask = ~bits;
    part->flags &= mask;
}

void ActorController_SetBehavior(void *, s32);
s32 Math_Atan2(s32, s32);

struct ArmorFacingActor {
    u8 unknown0[0x48];
    s16 x;
    u8 unknown4A[2];
    s16 z;
    u8 unknown4E[0x190 - 0x4E];
    struct ArmorPart body;
    u8 unknown1CC[0x1E4 - 0x1CC];
    struct ArmorPart attack;
    u8 unknown220[0xB26 - 0x220];
    u16 attackPower;
    u8 unknownB28[0xB32 - 0xB28];
    u16 flags;
    u8 unknownB34[0xB38 - 0xB34];
    u8 type;
    u8 facing;
    u8 unknownB3A[0xB3C - 0xB3A];
    u8 behavior;
    u8 phase;
    u8 unknownB3E[0x1120 - 0xB3E];
    u8 decision;
    u8 boost;
    s16 timer;
    s16 delay;
    u16 accumulated;
    u16 contacts;
    s16 boostTimer;
    u16 initialAttack;
};

struct ArmorTargetPosition {
    u8 unknown0[0x30];
    s16 x;
    u8 unknown32[2];
    s16 z;
};

/* Evaluate each position once; round the relative offset to whole tiles and take its magnitude. */
#define ARMOR_GET_RELATIVE_GRID(deltaX, deltaZ, absX, absZ, targetX, actorX, targetZ, actorZ)      \
    do {                                                                                           \
        (deltaX) = (targetX) - (actorX);                                                           \
        (deltaZ) = (targetZ) - (actorZ);                                                           \
        (deltaX) = ((deltaX) + 0x80) >> 8;                                                         \
        (deltaZ) = ((deltaZ) + 0x80) >> 8;                                                         \
        (absX) = (deltaX);                                                                         \
        (absZ) = (deltaZ);                                                                         \
        if ((deltaX) < 0)                                                                          \
            (absX) = -(deltaX);                                                                    \
        if ((deltaZ) < 0)                                                                          \
            (absZ) = -(deltaZ);                                                                    \
    } while (0)

// Tentative field meanings: armor pursuit and close-range attack decisions.
void Armor_UpdateFacingDecision(struct ArmorFacingActor *actor)
{
    struct ArmorTargetPosition *player;
    s32 playerX, playerZ, actorX, actorZ;
    s32 deltaX, deltaZ, absX, absZ;

    if (actor->timer > 0) {
        actor->timer--;
    }
    if (actor->boostTimer > 0) {
        actor->boostTimer--;
        if (actor->boostTimer <= 0) {
            actor->decision = 4;
            actor->boost = 0;
            actor->timer = 0x12C;
            actor->attackPower = actor->initialAttack;
            actor->contacts = 0;
            Armor_ClearPart(&actor->attack, 0x400008);
            Armor_ClearPart(&actor->body, 0x400008);
        }
    }
    if (actor->delay > 0) {
        actor->delay--;
    }

    if (actor->phase != 0xFF && actor->behavior != 2 && actor->behavior != 0x26 &&
        actor->behavior != 0x25 && actor->behavior != 0x27) {
        return;
    }

    player = (struct ArmorTargetPosition *)gGameState;
    ARMOR_GET_RELATIVE_GRID(deltaX, deltaZ, absX, absZ, playerX = player->x, actorX = actor->x,
                            playerZ = player->z, actorZ = actor->z);

    if (actor->flags & 1) {
        switch (actor->decision) {
            case 0:
                if (absX + absZ <= 4) {
                    if (actor->boost)
                        ActorController_SetBehavior(actor, 0x27);
                    else
                        ActorController_SetBehavior(actor, 0x25);
                    actor->decision = 1;
                    actor->timer = 0x168;
                } else {
                    if (actor->boost != 0)
                        goto approach;
                    if (actor->delay <= 0)
                        goto approach;
                    goto idle;
                }
                return;
            case 1:
                if (actor->behavior == 3) {
                    ActorController_SetBehavior(actor, 0x1e);
                    return;
                }

                {
                    s32 dir;
                    struct ArmorTargetPosition *target = (struct ArmorTargetPosition *)gGameState;
                    dir = Math_Atan2(target->x - actor->x, target->z - actor->z);
                    dir = (dir + 0x20) & 0xC0;
                    if (actor->facing == dir) {
                        s32 total = absX + absZ;
                        if (total > 5) {
                            ActorController_SetBehavior(actor, 9);
                            actor->decision = 0;
                            return;
                        }
                        if (total <= 1) {
                            ActorController_SetBehavior(actor, 3);
                            return;
                        }
                    }
                    if (actor->timer <= 0 && (deltaX == 0 || deltaZ == 0)) {
                        if (actor->boost != 0) {
                            ActorController_SetBehavior(actor, 0x24);
                        } else {
                            ActorController_SetBehavior(actor, 0x18);
                        }
                        actor->timer = 0x168;
                    } else {
                        if (actor->boost != 0)
                            goto close;
                        if (actor->delay <= 0)
                            goto close;
                        goto hold;
                    }
                }
                return;
            case 2:
                ActorController_SetBehavior(actor, 3);
                actor->decision = 1;
                actor->timer = 0x168;
                return;
            case 3:
                if ((u8)(actor->behavior - 10) > 1 && actor->behavior != 0xd) {
                    ActorController_SetBehavior(actor, 0xb);
                    return;
                }
                if (actor->timer <= 0) {
                    ActorController_SetBehavior(actor, 0xc);
                    actor->decision = 0;
                    actor->accumulated = 0;
                } else {
                    if (actor->boost != 0) {
                        ActorController_SetBehavior(actor, 0xc);
                        actor->decision = 0;
                        actor->accumulated = 0;
                    }
                }
                return;
            case 4:
                if (actor->timer <= 0) {
                    ActorController_SetBehavior(actor, 2);
                    actor->decision = 0;
                    actor->boost = 0;
                    actor->contacts = 0;
                    return;
                } else {
                    ActorController_SetBehavior(actor, 0x1d);
                    return;
                }
        }
        return;
    }

    if (actor->type == 0xF) {
        ActorController_SetBehavior(actor, 0x29);
        return;
    }
    switch (actor->decision) {
        case 0:
            if (absX + absZ <= 4) {
                if (actor->boost)
                    ActorController_SetBehavior(actor, 0x27);
                else
                    ActorController_SetBehavior(actor, 0x25);
                actor->decision = 1;
                actor->timer = 0x168;
            } else {
                if (actor->boost == 0 && actor->delay > 0)
                    goto idle;
            approach:
                ActorController_SetBehavior(actor, 0x26);
                return;
            idle:
                ActorController_SetBehavior(actor, 2);
            }
            return;
        case 1:
            if (actor->behavior == 3) {
                ActorController_SetBehavior(actor, 0x1e);
                return;
            }

            {
                s32 dir;
                dir = Math_Atan2(playerX - actorX, playerZ - actorZ);
                dir = (dir + 0x20) & 0xC0;
                if (actor->facing == dir) {
                    s32 total = absX + absZ;
                    if (total > 5) {
                        ActorController_SetBehavior(actor, 9);
                        actor->decision = 0;
                        return;
                    }
                    if (total <= 1) {
                        ActorController_SetBehavior(actor, 3);
                        return;
                    }
                }
                if (actor->timer <= 0 && (deltaX == 0 || deltaZ == 0)) {
                    if (actor->boost != 0) {
                        ActorController_SetBehavior(actor, 0x24);
                    } else {
                        ActorController_SetBehavior(actor, 0x18);
                    }
                    actor->timer = 0x168;
                } else {
                    if (actor->boost == 0 && actor->delay > 0)
                        goto hold;
                close:
                    ActorController_SetBehavior(actor, 0x27);
                    return;
                hold:
                    ActorController_SetBehavior(actor, 0x25);
                }
            }
            return;
        case 2:
            ActorController_SetBehavior(actor, 3);
            actor->decision = 1;
            actor->timer = 0x168;
            return;
        case 4:
            if (actor->timer <= 0) {
                ActorController_SetBehavior(actor, 2);
                actor->decision = 0;
                actor->boost = 0;
                actor->contacts = 0;
                return;
            } else {
                ActorController_SetBehavior(actor, 0x1d);
                return;
            }
    }
}
