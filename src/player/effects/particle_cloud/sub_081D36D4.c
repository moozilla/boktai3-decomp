#include "global.h"
#include "engine/ram.h"
#include "player/effects/player_particle_cloud.h"

s32 sub_081D36D4(void)
{
    struct CloudTask *task = gCloudTask;
    struct CloudParticle *particle;
    s32 i;

    if (task == NULL)
        return -1;
    particle = task->particles;
    for (i = 0; i < 40; i++, particle++)
        particle->sprite.flags |= 1;
    return 0;
}
