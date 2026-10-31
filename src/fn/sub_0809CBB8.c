#include "global.h"
#include "scene_actor_reset.h"
void Lun_OnBodyContact(void);
void Lun_HandlePlayerContact(void);
void SceneActorCollision_InitBody(void *, void *, void *, void *, void *);
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void ActorContact_InitializeBodyAndSecondary(struct SceneActorRegistryResetView *a)
{
    struct ContactPackedVector bodySize, bodyOffset, secondarySize, secondaryOffset;
    struct ContactCollider *collider;
    bodySize.x = 0x10;
    bodySize.y = 0x80;
    bodySize.z = 0x10;
    bodyOffset.x = 0;
    bodyOffset.y = 0;
    bodyOffset.z = 0;
    SceneActorCollision_InitBody(a, &bodySize, &bodyOffset, Lun_OnBodyContact, a);
    secondarySize.x = 0x10;
    secondarySize.y = 0x54;
    secondarySize.z = 0x10;
    secondaryOffset.x = 0;
    secondaryOffset.y = 0;
    secondaryOffset.z = 0;
    {
        u32 flags = 0x2003;
        collider = &a->colliderB;
        Collider_Init(collider, a->body.id, flags, 0, 0x10, &secondarySize, &secondaryOffset);
    }
    Collider_SetAttackProperties(collider, 0, 0, 0, 0, 0);
    Collider_SetPosition(collider, &a->body.position, 0);
    Collider_SetCallback(collider, Lun_HandlePlayerContact, a);
}
