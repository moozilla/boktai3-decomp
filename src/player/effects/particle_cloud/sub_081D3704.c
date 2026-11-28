#include "global.h"
#include "engine/ram.h"
#include "player/effects/player_particle_cloud.h"

s32 sub_081D3704(void)
{
    struct CloudTask *task = gCloudTask;
    s32 i;

    if (task == NULL)
        return -1;
    task->mode = 2;
    for (i = 0; i < 40; i++)
        ;
    return 0;
}
