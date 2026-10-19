#include "global.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 86.1%
// foundry draft (worker best 86.1%); what differs is mostly register allocation and instruction order
#define SceneActorAttachment_UpdatePosition sub_08180570
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
void sub_08180570(struct SceneActorAttachment *p)
{
    if (p->flags & 1) Sound_PlaySong(0x202);
    p->flags &= ~1;
    p->position = p->targetPosition;
    {
        u32 offset = p->sortOffset;
        s8 base = -0x60;
        s32 order = base;
        order -= offset;
        p->sortOrder = order;
    }
    p->colliderPosition = p->targetPosition;
    Collider_Refresh(p->colliderHeader);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08180570);
#endif
