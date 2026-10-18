// Tentative scene-actor family names; behavior inferred from static accesses.
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_collision.h"
void SceneActorCollision_InitBody(void *, void *, void *, void *, void *);
void SceneActorCollision_InitAttack(void *, void *, void *, void *, u32, void *);
void SceneActorCollision_InitExternalAttack(void *, void *, void *, void *, void *, u32, void *);
void Vorn_OnCollisionResponse(void);
void Vorn_OnContactEvent(void);
void Vorn_OnScriptActionRequest(void);
void SceneActor_InitContextColliders(struct SceneActorRegistryResetView *p)
{
    struct ContactPackedVector size, offset;
    u8 *q = p->context;
    size.x = 0x40;
    size.y = 0x80;
    size.z = 0x40;
    *(u32 *)&offset = 0x800000;
    *((u32 *)&offset + 1) &= 0xFFFF0000;
    SceneActorCollision_InitBody(p, &size, &offset, Vorn_OnCollisionResponse, p);
    ((struct ContactPosition *)&size)->x = 0x40;
    ((struct ContactPosition *)&size)->y = 0x64;
    ((struct ContactPosition *)&size)->z = 0x40;
    ((struct ContactPosition *)&offset)->x = 0;
    ((struct ContactPosition *)&offset)->y = 0x80;
    ((struct ContactPosition *)&offset)->z = 0;
    SceneActorCollision_InitAttack(p, &size, &offset, Vorn_OnContactEvent, 0, p);
    ((struct ContactPosition *)&size)->x = 0x40;
    ((struct ContactPosition *)&size)->y = 0x64;
    ((struct ContactPosition *)&size)->z = 0x40;
    ((struct ContactPosition *)&offset)->x = 0;
    ((struct ContactPosition *)&offset)->y = 0x80;
    ((struct ContactPosition *)&offset)->z = 0;
    SceneActorCollision_InitExternalAttack(p, q + 0x628, &size, &offset, Vorn_OnScriptActionRequest, 1, p);
}
