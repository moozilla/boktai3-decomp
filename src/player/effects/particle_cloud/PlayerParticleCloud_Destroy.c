#include "global.h"
#include "engine/ram.h"
#include "player/effects/player_particle_cloud.h"

void MessageWindow_Clear(struct CloudTask *);
void Effect_Remove(struct CloudSprite *);

u32 PlayerParticleCloud_Destroy(struct CloudTask *task)
{
    struct CloudParticle *particle;
    s32 i;

    MessageWindow_Clear(task);
    particle = task->particles;
    for (i = 0; i < 40; i++, particle++)
        Effect_Remove(&particle->sprite);
    gCloudTask = NULL;
    return 0;
}
