#ifndef GUARD_FRAGMENT_GROUP_H
#define GUARD_FRAGMENT_GROUP_H
#include "display_animation.h"
#include "scene_object.h"

/* Four groups of six ballistic fragments; resources and motion come from the spawner. */
struct FragmentGroup {
    u8 active, count, type, unknown3;
    struct SceneAnimation animation;
    u8 initialized[6], lifetime[6], enabled[6];
    u16 speed[6];
    struct SceneVec velocity[6];
    struct DisplaySequenceSprite sprites[6];
};
struct FragmentManager {
    struct Task task;
    u32 frame, unknown1C;
    struct FragmentGroup groups[4];
};
void FragmentGroup_Update(struct FragmentManager *, struct FragmentGroup *);
u32 FragmentManager_Update(struct FragmentManager *);
u32 FragmentManager_Destroy(struct FragmentManager *);
s32 FragmentManager_Initialize(struct FragmentManager *, void *);
struct FragmentManager *FragmentManager_Create(void *, u32);
void AnimatedParticleManager_ClearSingleton(void);
#endif
