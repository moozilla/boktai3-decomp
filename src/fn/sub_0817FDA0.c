#include "global.h"
#define SceneActor_RemoveEffectArray SceneActors_RemoveConvergingParticles
struct SceneActorComponents {
    u8 pad0[0x1cc];
    u8 collider[0x100];
    u8 animation[0x60];
    u8 attachment[0xb0];
    u8 shadow[1];
};
void Shadow_Destroy(void *);
void Collider_Remove(void *);
void Anim_Remove(void *);
void SpriteComponent_Destroy(void *);
void SceneActors_RemoveConvergingParticles(struct SceneActorComponents *);
void SceneActors_Destroy(void *);
s32 SceneActor_DestroyComponents(struct SceneActorComponents *p)
{
    Shadow_Destroy(p->shadow);
    Collider_Remove(p->collider);
    Anim_Remove(p->animation);
    SpriteComponent_Destroy(p->attachment);
    SceneActor_RemoveEffectArray(p);
    SceneActors_Destroy(p);
    return 0;
}
