// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#define SceneActorAttachment_UpdatePosition SpriteComponent_Update
struct SceneActorAttachmentPosition {
    u32 xy, z;
};
struct SceneActorAttachment {
    u8 pad0[8];
    u32 flags;
    u8 padC[2];
    u8 sortOrder;
    u8 padF[0x15];
    struct SceneActorAttachmentPosition position;
    u8 pad2C[0x24];
    struct SceneActorAttachmentPosition targetPosition;
    u8 sortOffset;
    u8 pad59;
    u16 contact;
    u8 colliderHeader[0xc];
    struct SceneActorAttachmentPosition colliderPosition;
};
void SpriteComponent_Update(struct SceneActorAttachment *p)
{
    if (p->flags & 1)
        Sound_PlaySong(0x202);
    p->flags &= ~1;
    {
        u32 a = p->targetPosition.xy;
        u32 b = p->targetPosition.z;
        p->position.xy = a;
        p->position.z = b;
    }
    {
        u32 offset = p->sortOffset;
        register s32 base asm("r5") = -0x60;
        s32 order;
        order = base;
        asm("" : "+r"(order));
        order -= offset;
        asm("" ::"r"(base));
        p->sortOrder = order;
    }
    {
        u32 a = p->targetPosition.xy;
        u32 b = p->targetPosition.z;
        p->colliderPosition.xy = a;
        p->colliderPosition.z = b;
    }
    Collider_Refresh(p->colliderHeader);
}
