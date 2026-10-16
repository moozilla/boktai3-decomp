#include "scene_particle_burst.h"
extern struct SceneParticleBurstPool *gUnk_02000130;
s32 SceneParticleBurst_Initialize(struct SceneParticleBurstPool *, u32);
s32 SceneParticleBurst_Update(struct SceneParticleBurstPool *);
s32 SceneParticleBurst_Destroy(struct SceneParticleBurstPool *);
struct SceneParticleBurstPool *SceneParticleBurst_EnsureSingleton(u32 unusedA, u32 unusedB)
{
    struct SceneParticleBurstPool *p;
    if (gUnk_02000130)
        return gUnk_02000130;
    p = Task_Create(9, sizeof(*p));
    gUnk_02000130 = p;
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
