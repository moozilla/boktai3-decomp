#include "scene/actor/scene_actor_state.h"
#include "global.h"
#include "monsters/chase_actor.h"
#include "scene/motion/scene_actor_chase_shared.h"
/* CentipedeBody_Destroy: Ivy_Destroy shape; removes the tentacle segment sprites first (ported by relatives.py, fixed by hand; provisional name) */
void Sprite_Remove(void *);
void Anim_Remove(void *);
void Heap_Free(void *);
void SceneMessages_UnregisterReceiver(void *);
void Shadow_Destroy(void *);
void TerrainOverlay_QueueRemoveActor(void *);
void SceneBody_Unregister(void *);
void Collider_Remove(void *);
void SceneActorRegistry_UnlinkActor(void *);

void CentipedeBody_RemoveSegmentSprites(struct ChaseActor *);
void SpritePool_ConsumeStateFlag(struct ChaseActor *);
u32 CentipedeBody_Destroy(struct ChaseActor *s)
{
    u8 *p;
    u8 *q;
    u32 m;
    u32 v;
    CentipedeBody_RemoveSegmentSprites(s);
    SpritePool_ConsumeStateFlag(s);
    p = s->base.drawable;
    if (s->base.drawableKind == 0)
        Sprite_Remove(p);
    else
        Anim_Remove(p + 0x20);
    Heap_Free(p);
    m = 0x100;
    if (SceneActorChase_TestField(&s->base.placementFlags, m))
        SceneMessages_UnregisterReceiver(s->base.receiver);
    {
        u8 *r = s->base.shadow;
        if (*r == 1)
            Shadow_Destroy(r);
    }
    m = 0x2000000;
    v = s->base.statusFlags & m;
    q = (u8 *)&s->base.body;
    if (v == 0)
        TerrainOverlay_QueueRemoveActor(q);

    SceneBody_Unregister(q);
    Collider_Remove(&s->base.colliderA);
    SceneActorRegistry_UnlinkActor(s);
    return 0;
}
