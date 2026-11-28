#include "effects/bursts/charged_burst.h"
#include "scene/motion/scene_actor_terrain.h"
#include "engine/math/monster_particle_random.h"
#include "engine/engine.h"
#include "scene/collision/contact_actor.h"
#include "scene/collision/packed_contact_vector.h"

void sub_0823350C(struct ChargedBurst *);
s32 Collider_Refresh(void *);
void ImpactParticleGroup_SpawnBouncing(u32, u32, void *, void *, void *, u32, u32);

void ChargedBurst_EmitTerminalDebris(struct ChargedBurst *object)
{
    sub_0823350C(object);
    Collider_Refresh(&object->collider);
    if (++object->timer > 15) {
        u32 height = object->core.position.y;
        struct ContactPosition *position = &object->position;
        if (height >
            (u32)SceneActorTerrain_GetHeight(SceneActorTerrain_GetCellIndex(position), position)) {
            struct PackedContactVector velocity, extent;

            velocity.x = (ParticleRandom_ReadDirectSample(gRandomTable) & 15) - 7;
            velocity.y = (ParticleRandom_ReadDirectSample(gRandomTable) & 31) - 16;
            velocity.z = (ParticleRandom_ReadDirectSample(gRandomTable) & 15) - 7;
            extent.x = 5;
            extent.y = 10;
            extent.z = 5;
            ImpactParticleGroup_SpawnBouncing(4, 2, &object->core.position, &velocity, &extent, 60,
                                              60);
        }
        Task_Kill(object);
    }
}
