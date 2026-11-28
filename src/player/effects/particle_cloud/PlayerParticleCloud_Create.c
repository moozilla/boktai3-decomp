#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "player/effects/player_particle_cloud.h"

u32 sub_081D31EC(struct CloudTask *task);
u32 PlayerParticleCloud_Destroy(struct CloudTask *task);
s32 PlayerParticleCloud_Initialize(struct CloudTask *task, const void *position, s32 mode);

struct CloudTask *PlayerParticleCloud_Create(const void *position, s32 mode)
{
    struct CloudTask *task;

    if (gCloudTask != NULL)
        return gCloudTask;
    task = Task_Create(0xA, sizeof(struct CloudTask));
    if (task != NULL) {
        Task_SetCallbacks(task, (void (*)(void *))sub_081D31EC,
                          (void (*)(void *))PlayerParticleCloud_Destroy);
        if (PlayerParticleCloud_Initialize(task, position, mode) < 0) {
            Task_Kill(task);
            return NULL;
        }
    }
    return task;
}
