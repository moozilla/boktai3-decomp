#include "scene_particle_burst.h"
#include "ram.h"
s32 SceneParticleBurst_Initialize(struct SceneParticleBurstPool *, u32);
s32 SceneParticleBurst_Update(struct SceneParticleBurstPool *);
s32 SceneParticleBurst_Destroy(struct SceneParticleBurstPool *);
struct SceneParticleBurstPool *SceneParticleBurst_EnsureSingleton(u32 unusedA, u32 unusedB)
{
    struct SceneParticleBurstPool *p;
    if (gSceneParticleBurstPool)
        return gSceneParticleBurstPool;
    p = Task_Create(9, sizeof(*p));
    gSceneParticleBurstPool = p;
    if (p) {
        Task_SetCallbacks(p, (void (*)(void *))SceneParticleBurst_Update,
                          (void (*)(void *))SceneParticleBurst_Destroy);
        if (SceneParticleBurst_Initialize(p, 0) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
