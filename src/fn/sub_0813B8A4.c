#include "global.h"
#include "scene_vec.h"
#include "scene_object.h"
struct SceneActorParticleView {
    u8 unknown0[0x30];
    struct SceneVec position;
    u8 unknown38[0x168 - 0x38];
    struct SceneVec effectPosition;
    u8 unknown170[0x418 - 0x170];
    u8 mode;
};
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void SceneActor_SpawnModeParticles(struct SceneActorParticleView *p)
{
    struct SceneExtent extent;
    struct SceneVec position;
    if ((u8)(p->mode - 3) <= 1) {
        position = p->effectPosition;
        extent.x = 32;
        extent.y = 32;
        extent.z = 32;
    } else {
        position = p->position;
        position.y += 80;
        extent.x = 64;
        extent.y = 60;
        extent.z = 64;
    }
    SparkParticle_Spawn(&position, 1, &extent, 0, 0, 12, 2, 2);
}
