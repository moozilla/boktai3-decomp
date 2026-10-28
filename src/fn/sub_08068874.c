#include "scene_particle_pool.h"
#include "ram.h"
s32 SceneParticlePool_Initialize(struct SceneParticlePool *);
s32 SceneParticlePool_Update(struct SceneParticlePool *);
s32 SceneParticlePool_Destroy(struct SceneParticlePool *);
struct SceneParticlePool *SceneParticlePool_EnsureSingleton(void)
{
    struct SceneParticlePool *pool;
    if (gSceneParticleEmitterPool)
        return gSceneParticleEmitterPool;
    pool = Task_Create(10, 0x2C4);
    gSceneParticleEmitterPool = pool;
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))SceneParticlePool_Update,
                          (void (*)(void *))SceneParticlePool_Destroy);
        if (SceneParticlePool_Initialize(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
