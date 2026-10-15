#ifndef GUARD_SCENE_ACTOR_RESTORATION_H
#define GUARD_SCENE_ACTOR_RESTORATION_H
#include "scene_actor_registry.h"
/* Embedded response at actor460. These callbacks consume active bytes468-A,
 * the signed threshold46C and the control word470. Gameplay names tentative. */
struct SceneActorRestorationState {
    u8 unknown0[0x468];
    u8 state, phase, behavior, unknown46B;
    s32 threshold;
    const void *control;
};
extern const u8 gUnk_0824E128[];
/* The implementation ignores the forwarded table and index. */
void sub_0812E084(void *, const void *, u32);
void sub_0807FDC0(struct SceneActorRegistryActor *, s32);
#endif
