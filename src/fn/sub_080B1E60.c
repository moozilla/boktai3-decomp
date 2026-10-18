#define SceneActor_ResetContextAnimation sub_080B1E60
// Tentative scene-actor family names; behavior inferred from static accesses.
#include "global.h"
#include "scene_actor_reset.h"
extern const u8 gUnk_0824E230[];
void sub_0812E084(void *, const void *, u32);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
s32 Anim_Step(void *, void *);
void sub_080B1E60(struct SceneActorRegistryResetView *p)
{
    struct SceneActorRegistryResetView *q = p->context;
    q->field6D6 = 0;
    q->flags6D4 = 0;
    q->field6E8 = 0;
    q->field698 = 0;
    Memory_Clear(p->state5D4, 8);
    Memory_Clear(&q->vector6E0, 8);
    Memory_Clear(&q->vector6D8, 8);
    sub_0812E084(&p->word460, gUnk_0824E230, 0x2F8);
    SceneActorMovement_ResolveRoutePosition(p);
    SceneActorAnimation_SelectDirectionalEntry(p, 0);
    Anim_Step((u8 *)p->drawable + 0x20, p->drawable);
}
