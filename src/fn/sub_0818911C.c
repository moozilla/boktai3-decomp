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

struct ArmorTimedActor {
    u8 unknown0[0x48];
    s16 x;
    u8 unknown4A[2];
    s16 z;
    u8 unknown4E[0x190 - 0x4E];
    struct ArmorPart body;
    u8 unknown1CC[0x1E4 - 0x1CC];
    struct ArmorPart attack;
    u8 unknown220[0xB26 - 0x220];
    u16 savedB26;
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
    u8 fastMode;
    s16 attackCooldown;
    s16 secondaryCooldown;
    u16 collisionCooldown;
    u16 modeTimer;
    s16 transitionTimer;
    u16 field112C;
};

struct ArmorTarget {
    u8 unknown0[0x30];
    s16 x;
    u8 unknown32[2];
    s16 z;
};

// MATCHING_CONCESSION: register-pinning
// confidence=low: switches timed armor attack modes and selects aligned attacks.
void sub_0818911C(void *arg0)
{
    struct ArmorTimedActor *a = arg0;
    struct ArmorTarget *player;
    s32 targetX, targetZ, actorX, actorZ;
    s32 gridX, gridZ, absoluteX, distance;
    register s32 absoluteXValue asm("r6");
    u32 mode;
    u8 *decision;
    register u32 stateAddress asm("r3");

    if (a->attackCooldown > 0) {
        a->attackCooldown--;
    }
    if (a->transitionTimer > 0) {
        a->transitionTimer--;
        if (a->transitionTimer <= 0) {
            a->decision = 4;
            a->fastMode = 0;
            a->attackCooldown = 0x12C;
            a->savedB26 = a->field112C;
            a->modeTimer = 0;
            Armor_ClearPart(&a->attack, 0x400008);
            Armor_ClearPart(&a->body, 0x400008);
        }
    }
    if (a->secondaryCooldown > 0) {
        a->secondaryCooldown--;
    }

    if (a->phase != 0xFF && a->behavior != 2 && a->behavior != 0x26 && a->behavior != 0x25 &&
        a->behavior != 0x27) {
        return;
    }

    player = (struct ArmorTarget *)gGameState;
    targetX = player->x;
    actorX = a->x;
    gridX = targetX - actorX;
    targetZ = player->z;
    actorZ = a->z;
    gridZ = targetZ - actorZ;
    gridX = (gridX + 0x80) >> 8;
    gridZ = (gridZ + 0x80) >> 8;
    absoluteXValue = gridX;
    distance = gridZ;
    if (gridX < 0)
        absoluteXValue = -gridX;
    if (gridZ < 0)
        distance = -gridZ;

    absoluteX = absoluteXValue;
    mode = a->flags & 1;
    if (mode) {
        switch (a->decision) {
            case 0:
                if (absoluteX + distance <= 4) {
                    if (a->fastMode)
                        ActorController_SetBehavior(a, 0x27);
                    else
                        ActorController_SetBehavior(a, 0x25);
                    a->decision = 1;
                    a->attackCooldown = 0x168;
                } else {
                    if (a->fastMode != 0)
                        goto idle38;
                    if (a->secondaryCooldown <= 0)
                        goto idle38;
                    goto idle2;
                }
                return;
            case 1:
                if (a->behavior == 3) {
                    ActorController_SetBehavior(a, 0x1e);
                    return;
                }
                {
                    s32 total = absoluteX + distance;
                    distance = total;
                }
                if (distance > 5) {
                    if (a->fastMode != 0)
                        goto reset38;
                    if (a->secondaryCooldown <= 0)
                        goto reset38;
                    goto reset2;
                }
                {
                    s32 dir;
                    struct ArmorTarget *target = (struct ArmorTarget *)gGameState;
                    asm("" : : "r"(absoluteX));
                    dir = Math_Atan2(target->x - a->x, target->z - a->z);
                    dir = (dir + 0x20) & 0xC0;
                    if (a->facing == dir && distance <= 1) {
                        ActorController_SetBehavior(a, 3);
                        return;
                    }
                    if (a->attackCooldown <= 0 && (gridX == 0 || gridZ == 0)) {
                        if (a->fastMode != 0) {
                            ActorController_SetBehavior(a, 0x24);
                        } else {
                            ActorController_SetBehavior(a, 0x18);
                        }
                        a->attackCooldown = 0x168;
                    } else {
                        if (a->fastMode != 0)
                            goto wait39;
                        if (a->secondaryCooldown <= 0)
                            goto wait39;
                        goto wait37;
                    }
                }
                return;
            case 2:
                ActorController_SetBehavior(a, 3);
                a->decision = 1;
                a->attackCooldown = 0x168;
                return;
            case 3:
                if ((u8)(a->behavior - 10) > 1 && a->behavior != 0xd) {
                    ActorController_SetBehavior(a, 0xb);
                    return;
                }
                if (a->attackCooldown <= 0) {
                    ActorController_SetBehavior(a, 0xc);
                    a->decision = 0;
                    a->collisionCooldown = 0;
                } else {
                    if (a->fastMode != 0) {
                        ActorController_SetBehavior(a, 0xc);
                        a->decision = 0;
                        a->collisionCooldown = 0;
                    }
                }
                return;
            case 4:
                if (a->attackCooldown <= 0) {
                    ActorController_SetBehavior(a, 2);
                    a->decision = 0;
                    a->fastMode = 0;
                    a->modeTimer = 0;
                    return;
                } else {
                    ActorController_SetBehavior(a, 0x1d);
                    return;
                }
        }
        return;
    }

    if (a->type == 0xE) {
        ActorController_SetBehavior(a, 0x29);
        return;
    }
    stateAddress = (u32) & ((struct ArmorTimedActor *)0)->decision;
    asm("" : "+r"(stateAddress));
    stateAddress += (u32)a;
    decision = (u8 *)stateAddress;
    switch (*(u8 *)stateAddress) {
        case 0:
            if (absoluteX + distance <= 4) {
                { /* Reserve r2 during this load; the asm output defines this scratch value. */
                    register u32 reservedScratch asm("r2");
                    u8 fastMode;
                    asm("" : "=r"(reservedScratch));
                    fastMode = a->fastMode;
                    asm("" : : "r"(stateAddress), "r"(reservedScratch));
                    if (fastMode)
                        ActorController_SetBehavior(a, 0x27);
                    else
                        ActorController_SetBehavior(a, 0x25);
                }
                a->decision = 1;
                a->attackCooldown = 0x168;
            } else {
                if (a->fastMode == 0 && a->secondaryCooldown > 0)
                    goto idle2;
            idle38:
                ActorController_SetBehavior(a, 0x26);
                return;
            idle2:
                ActorController_SetBehavior(a, 2);
            }
            return;
        case 1:
            if (a->behavior == 3) {
                ActorController_SetBehavior(a, 0x1e);
                return;
            }
            {
                s32 total = absoluteX + distance;
                distance = total;
            }
            if (distance > 5) {
                if (a->fastMode == 0 && a->secondaryCooldown > 0)
                    goto reset2;
            reset38:
                ActorController_SetBehavior(a, 0x26);
                goto clearDecision;
            reset2:
                ActorController_SetBehavior(a, 2);
            clearDecision:
                a->decision = 0;
                return;
            }
            {
                s32 dir;
                dir = Math_Atan2(targetX - actorX, targetZ - actorZ);
                dir = (dir + 0x20) & 0xC0;
                if (a->facing == dir && distance <= 1) {
                    ActorController_SetBehavior(a, 3);
                    return;
                }
                if (a->attackCooldown <= 0 && (gridX == 0 || gridZ == 0)) {
                    if (a->fastMode != 0) {
                        ActorController_SetBehavior(a, 0x24);
                    } else {
                        ActorController_SetBehavior(a, 0x18);
                    }
                    a->attackCooldown = 0x168;
                } else {
                    if (a->fastMode == 0 && a->secondaryCooldown > 0)
                        goto wait37;
                wait39:
                    ActorController_SetBehavior(a, 0x27);
                    return;
                wait37:
                    ActorController_SetBehavior(a, 0x25);
                }
            }
            return;
        case 2:
            ActorController_SetBehavior(a, 3);
            *decision = 1;
            a->attackCooldown = 0x168;
            return;
        case 4:
            if (a->attackCooldown <= 0) {
                ActorController_SetBehavior(a, 2);
                *decision = mode;
                a->fastMode = mode;
                a->modeTimer = mode;
                return;
            } else {
                ActorController_SetBehavior(a, 0x1d);
                return;
            }
    }
}
