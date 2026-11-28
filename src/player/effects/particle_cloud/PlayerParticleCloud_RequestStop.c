#include "global.h"
#include "engine/ram.h"
#include "player/effects/player_particle_cloud.h"

s32 PlayerParticleCloud_RequestStop(void)
{
    struct CloudTask *task = gCloudTask;

    if (task == NULL)
        return -1;
    task->stop = 1;
    return 0;
}
