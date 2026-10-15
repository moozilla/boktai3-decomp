#include "scene_particle_pool.h"
#define SceneParticlePool_EnsureSingleton sub_08068874
extern struct SceneParticlePool *gUnk_02000134;
s32 sub_08068868(struct SceneParticlePool *);
s32 SceneParticlePool_Update(struct SceneParticlePool *);
s32 SceneParticlePool_Destroy(struct SceneParticlePool *);
struct SceneParticlePool *SceneParticlePool_EnsureSingleton(void)
{
    struct SceneParticlePool *pool;
    if (gUnk_02000134) return gUnk_02000134;
    pool = Task_Create(10, 0x2C4);
    gUnk_02000134 = pool;
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))SceneParticlePool_Update,
                          (void (*)(void *))SceneParticlePool_Destroy);
        if (sub_08068868(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
