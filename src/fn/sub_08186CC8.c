// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "game_state.h"
#include "contact_position.h"
#include "scene_actor_components.h"
#include "armor_decision_ops.h"
struct ArmorSelection {
    u8 pad0[0x3C];
    struct SceneActorBody body;
    u8 unknown84[0xB32 - 0x84];
    u16 flags;
    u8 padb34[0x5];
    u8 facing;
    u8 padb3a[0x2];
    u8 behavior;
    u8 phase;
    u8 padb3e[0x5e2];
    u8 decision;
    u8 pad1121[0x1];
    s16 attackCooldown;
};
void ActorController_SetBehavior(void *, s32), ActorController_SetBehaviorIfFinished(void *, s32);
u32 Armor_IsNearSavedPosition(void *);
void Armor_SaveCurrentPosition(void *);
void Armor_SelectAlignedAttack(struct ArmorSelection *p)
{
    s32 x;
    s32 y;
    s32 z;
    s32 ax;
    s32 az;
    s32 playerY;
    s32 actorY;
    u8 *decision;
    if (p->attackCooldown > 0)
        p->attackCooldown--;
    if (((p->phase != 255) && (p->behavior != 2)) && (p->behavior != 19))
        return;
    x = gGameState->positionX - p->body.position.x;
    playerY = gGameState->positionY;
    actorY = p->body.position.y;
    z = gGameState->positionZ - p->body.position.z;
    x = (x + 128) >> 8;
    z = (z + 128) >> 8;
    ax = x;
    y = playerY - actorY;
    az = z;
    ARMOR_ABSOLUTE_COMPONENT(ax, ax);
    ARMOR_ABSOLUTE_COMPONENT(y, y);
    ARMOR_ABSOLUTE_COMPONENT(az, z);
    if (p->flags & 1) {
        decision = &p->decision;
        switch (*decision) {
            case 0:
                if ((u8)(p->flags & 2)) {
                    ActorController_SetBehavior(p, 12);
                    break;
                }
                if (y > 127)
                    goto far;
                if (x == 0) {
                    ARMOR_BRANCH_OUTSIDE_GRID(az, 1, far);
                    if (Armor_IsNearSavedPosition(p))
                        goto near;
                    goto displaced;
                } else {
                    if ((z != 0) || (ax > 1))
                        goto far;
                    if (!Armor_IsNearSavedPosition(p))
                        goto displaced;
                }
            near:
                ActorController_SetBehavior(p, 3);

                goto save;
            displaced:
                ActorController_SetBehavior(p, 4);

            save:
                Armor_SaveCurrentPosition(p);

                break;
            far:
                ActorController_SetBehaviorIfFinished(p, 2);

                break;

            case 1:
                if ((p->attackCooldown <= 0) || (x && z))
                    ARMOR_CLEAR_DECISION(decision);
                else {
                    u32 action = p->behavior;
                    if ((((u8)(action - 10)) > 1) && (action != 13))
                        ActorController_SetBehavior(p, 10);
                }
                break;
        }

    } else if (p->behavior == 3) {
        if (y <= 127) {
            if (!x) {
                if ((z < 0) && (p->facing == 0xc0))
                    goto aligned;
                if (p->facing != 0x40)
                    goto approach;
                goto aligned;
            } else if (!z) {
                if ((x < 0) && (p->facing == 0x80))
                    goto aligned;
                if (!p->facing)
                    goto aligned;
            }
        }
        goto approach;
    aligned:
        ActorController_SetBehavior(p, 22);

    } else {
        if (y > 127)
            goto approach;
        if (x == 0) {
            ARMOR_BRANCH_OUTSIDE_GRID(az, 1, approach);
        } else {
            if (z != 0 || ax > 1)
                goto approach;
        }
        ActorController_SetBehavior(p, 3);
        return;
    approach:
        ActorController_SetBehaviorIfFinished(p, 2);
    }
}
