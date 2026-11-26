#include "scene/actor/scene_actor_control.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/math/monster_particle_random.h"

void SceneActorControl_TickJitter(void *unused, struct SceneActorControlSlot *slot)
{
    if (slot->timer <= 0) {
        slot->timer = 20;
        slot->flags = (s16)slot->flags & ~4;
    } else {
        if (slot->timer > 10) {
            const u16 *table = gRandomTable;
            slot->position.x += Mod(ParticleRandom_ReadSample(table), 40) - 20;
            slot->position.z += Mod(ParticleRandom_ReadSample(table), 40) - 20;
        }
        slot->timer--;
    }
}
