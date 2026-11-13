// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control.h"
#include "engine.h"
#include "ram.h"
#include "monster_particle_random.h"
void SceneActorControl_TickJitter(void *unused, struct SceneActorControlSlot *slot)
{
    register s16 *timer asm("r5") = &slot->timer;
    s32 remaining;
    remaining = *timer;
    if (remaining <= 0) {
        u32 reset = 20;
        *timer = reset;
        {
            u16 *flags = &slot->flags;
            u32 old = *flags;
            reset -= 25;
            reset &= old;
            *flags = reset;
        }
    } else {
        if (remaining > 10) {
            const u16 *table = gRandomTable;
            s32 jitter;
            s32 secondJitter;
            jitter = Mod(ParticleRandom_ReadSample(table), 40);
            {
                u32 x = (u16)slot->position.x - 20;
                slot->position.x = x + jitter;
            }
            secondJitter = Mod(ParticleRandom_ReadSample(table), 40);
            {
                u32 z = (u16)slot->position.z - 20;
                slot->position.z = z + secondJitter;
            }
        }
        (*timer)--;
    }
}
