#include "scene_particle_burst.h"
#define SceneParticleBurst_EnsureSingleton sub_08068270
extern struct SceneParticleBurstPool *gUnk_02000130;
s32 sub_08068258(struct SceneParticleBurstPool *, u32);
s32 sub_08068140(struct SceneParticleBurstPool *);
s32 sub_080681DC(struct SceneParticleBurstPool *);
struct SceneParticleBurstPool *SceneParticleBurst_EnsureSingleton(u32 unusedA, u32 unusedB)
{
    struct SceneParticleBurstPool *p;
    if (gUnk_02000130)
        return gUnk_02000130;
    p = Task_Create(9, sizeof(*p));
    gUnk_02000130 = p;
    if (p) {
        Task_SetCallbacks(p, (void (*)(void *))sub_08068140, (void (*)(void *))sub_080681DC);
        if (sub_08068258(p, 0) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
