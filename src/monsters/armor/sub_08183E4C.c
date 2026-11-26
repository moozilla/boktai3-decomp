#include "monsters/armor/actor_controller.h"
#include "effects/particles/scene_particle_burst_api.h"

/* tentative: faces a direction, sets a fixed-speed motion vector, nudges toward the cell centre and spawns a particle burst */

extern u32 gUnk_0824EDF8[];
extern const struct ContactPosition gUnk_0824EE08;

void ArmorActor_UpdateFixedSpeedMotion(struct SceneCombatActor *p)
{
    u8 *s = &p->actionPhase;
    struct ActorControllerVelocity *w;

    if (*s == 0) {
        Armor_SetAnimationFromStart(p, &gUnk_0824EDF8[((p->heading + 0x60) >> 6) & 3]);
        *s = 0xFF;
        p->actionReady = 1;
        p->flags |= 2;
        ActorController_ClearColliderFlags(&p->colliderGroup.colliders[2], 4);
        ActorController_ClearColliderFlags(&p->colliderGroup.colliders[1], 4);
        ActorController_ClearColliderFlags(&p->colliderGroup.colliders[3], 4);
        ActorController_SetColliderFlags(&p->colliderGroup.colliders[0], 4);
        ActorController_ClearMotionFlags(&p->colliderGroup.motion, 1);
        *(u16 *)(s - 14) &= ~0x20;
    }
    if ((p->actionCounter & 0x1F) == 0) {
        u8 *ap = &p->heading;
        u8 a;
        w = &p->velocity;
        a = *ap;
        ActorController_SetMotion(w, a, 32);
        p->flags |= 0x20;
        if (p->velocity.x == 0) {
            s32 distance = (u8)p->navigation.referencePosition.x;
            distance = 0x80 - distance;
            if (distance > 16)
                p->velocity.x = 3;
            else if (distance < -16)
                p->velocity.x = -3;
        } else if (p->velocity.z == 0) {
            s32 distance = (u8)p->navigation.referencePosition.z;
            distance = 0x80 - distance;
            if (distance > 16)
                p->velocity.z = 3;
            else if (distance < -16)
                p->velocity.z = -3;
        }
        SceneParticleBurst_CreateWithPause(&p->navigation.referencePosition, 0, &gUnk_0824EE08, 8, 3, 0);
    } else {
        p->flags &= ~0x20;
    }
}
