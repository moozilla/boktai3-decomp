#include "scene_actor_reset.h"

void *SceneActorContacts_SelectForActor(void *, u32);
void SceneActorContacts_ReleaseNode(void *);
void SceneActorControl_OnIdle(void);
void SceneActorReaction_ResetResponseState(void *, const void *, u32);
void SceneActorTarget_ResetContactResponse(void *);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void SceneActorAttachment_ReplaceResponseSlot(void *, s32, s32, u8);
s32 Callback_InvokeActor(void *, void *);
extern const u8 gUnk_0824E230[];

#ifdef NONMATCHING
// fuzzy: 47.7%
// scope shape 106 (differences once low registers are set aside)
// confidence=low: tries a contact-driven reaction on a scene actor, falling back to an idle control reset.
u8 sub_080B3C1C(struct SceneActorRegistryResetView *a)
{
    struct SceneActorReactionMotion *motion;
    u16 sawNode;
    void **nodeSlot;
    void *contact;
    void *object;
    void *data;
    u16 cell;

    {
        u32 mask = 0x10;
        if (a->flags2D4 & mask)
            return 0;
    }
    if (0)
        return 0;
    if (Callback_InvokeActor(a, a->update26C))
        return 1;
    motion = (struct SceneActorReactionMotion *)&a->tracking.target;
    if (!(motion->flags & 1))
        return 0;
    sawNode = a->flags2D4 & 0x100;
    if (sawNode)
        return 0;
    nodeSlot = (void **)((u8 *)a + 0x45C);
    if (!a->tracking.limit)
        return 0;
    contact = SceneActorContacts_SelectForActor(a, 6);
    if (!contact)
        return 0;
    if (a->flags2D4 & 0x1000) {
        SceneActorContacts_ReleaseNode(*nodeSlot);
        a->flags2D4 &= ~0x1000;
        *nodeSlot = (void *)sawNode;
    }
    object = *(void **)contact;
    motion->fieldDC = (u32)object;
    data = *(void **)((u8 *)object + 0x1C);
    cell = *(u16 *)((u8 *)data + 6);
    a->group.flags414 &= ~2;
    if (*(u32 *)((u8 *)a + 0x100) != 0) {
        if (motion->flags & 8) {
            u16 *cellSlot = (u16 *)((u8 *)a + 0x2F8);
            if (*cellSlot != cell)
                *cellSlot = cell;
        } else {
            SceneActorTarget_ResetContactResponse(a);
        }
        a->flags2D6 |= 0x80;
        motion->flags |= 0x10;
        return 0;
    }
    if (a->flags2CC & 0x80)
        return 0;
    a->flags2CC |= 0x80;
    a->flags2D4 &= ~0x100;
    SceneActorReaction_ResetResponseState(&a->word460, gUnk_0824E230, 0x689);
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = 0x10;
    a->target = (void *)SceneActorControl_OnIdle;
    a->field2C8 = 0;
    a->flags114 &= ~2;
    a->word104 = 0;
    a->state2A8 = 3;
    a->phase2A9 = 1;
    a->behavior2AA = 0;
    a->counter2AC = 0;
    a->field2B1 = 1;
    a->flags2D4 &= ~0x400;
    a->field5DA = 0xFF;
    SceneActorAnimation_SelectDirectionalEntry(a, 0);
    *(u16 *)((u8 *)a + 0x428) = 0xB4;
    SceneActorAttachment_ReplaceResponseSlot(a, 0, 1, a->category);
    a->flags2D6 |= 0x100;
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080B3C1C);
#endif
