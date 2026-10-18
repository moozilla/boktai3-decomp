#define Player_IsParticleSuppressedMode Player_IsInActionTwelveOrThirtyEight
#include "global.h"
#include "player.h"
#include "scene_object.h"
#include "breakable_barrier.h"
s32 Player_IsInActionTwelveOrThirtyEight(struct Player *);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void Player_TickModeParticles(struct Player *p)
{
    struct SceneExtent extent;
    struct BarrierExtent position;
    if (Player_IsParticleSuppressedMode(p))
        return;
    if (++p->particleIntervalCounter <= 14)
        return;
    if ((u8)(p->actionMode - 3) <= 1) {
        position = *(struct BarrierExtent *)&p->colliderPosition;
        extent.x = 32;
        extent.y = 32;
        extent.z = 32;
    } else {
        position = *(struct BarrierExtent *)&p->position;
        position.y += 160;
        extent.x = 64;
        extent.y = 60;
        extent.z = 64;
    }
    SparkParticle_Spawn(&position, 1, &extent, 0, 0, 12, 2, 2);
    p->particleEmissionCount++;
    p->particleIntervalCounter = 0;
}
