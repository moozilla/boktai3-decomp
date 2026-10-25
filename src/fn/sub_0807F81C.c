#include "global.h"

#include "scene_actor_reset.h"
#include "scene_actor_contact_list.h"
#include "scene_actor_reaction.h"
extern const u8 gUnk_0824E114[];

// confidence=low: arms a contact reaction on an actor after a reset-phase check; returns 1 when armed.
void SceneActorTarget_ResetContactResponse(void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorReaction_ResetResponseState(void *, const void *, u32);

#ifdef NONMATCHING
// fuzzy: 42.2%
// scope shape 186 (differences once low registers are set aside)
// confidence=low: arms a contact reaction on an actor after a reset-phase check; returns 1 when armed.
u8 sub_0807F81C(struct SceneActorRegistryResetView *a)
{
    struct SceneActorReactionMotion *motion;
    struct SceneActorContactNode *contact;
    struct SceneActorContactNode **nodeSlot = (struct SceneActorContactNode **)((u8 *)a + 0x45C);
    u32 zero;
    u16 cell;
    void *route;
    u8 *pos;
    s32 blocked;
    u32 *flags2CC;

    if (a->flags2D4 & 0x10)
        return 0;
    if ((u8)Callback_InvokeActor(a, a->update26C))
        return 1;
    motion = (struct SceneActorReactionMotion *)&a->fieldDC;
    if (!(motion->flags & 1))
        return 0;
    zero = a->flags2D4 & 0x100;
    if (zero)
        return 0;
    if (!a->distanceLimitF8)
        return 0;
    contact = SceneActorContacts_SelectForActor(a, 6);
    if (!contact)
        return 0;
    if (a->flags2D4 & 0x1000) {
        SceneActorContacts_ReleaseNode(*nodeSlot);
        a->flags2D4 &= ~0x1000;
        *nodeSlot = (struct SceneActorContactNode *)zero;
    }
    motion->fieldDC = *(u32 *)contact;
    cell = *(u16 *)(*(u8 **)((u8 *)motion->fieldDC + 0x1C) + 6);
    a->group.flags414 &= -3;
    *nodeSlot = 0;
    if (*(u32 *)((u8 *)a + 0x100) != 0) {
        if (motion->flags & 8) {
            if (*(u16 *)((u8 *)a + 0x2F8) != cell) {
                route = *(void **)((u8 *)a + 0x5D0);
                pos = (u8 *)motion->fieldDC + 0xC;
                if (!Nav_IsPositionInMesh(route, &a->position, (struct ContactPosition *)pos)) {
                    a->flags2D4 &= ~0x200;
                } else {
                    blocked =
                        Terrain_ApplyBlocked(route, &a->position, (struct ContactPosition *)pos);
                    if (blocked == 0)
                        a->flags2D4 &= ~0x200;
                    else if (blocked != 2)
                        Memory_Clear(&a->motion, 8);
                }
                *(u16 *)((u8 *)a + 0x2F8) = cell;
            }
        } else {
            SceneActorTarget_ResetContactResponse(a);
        }
        a->flags2D6 |= 0x80;
        motion->flags |= 0x10;
        return 0;
    }
    flags2CC = &a->flags2CC;
    if (*flags2CC & 0x80)
        return 0;
    *flags2CC |= 0x80;
    a->flags2D4 &= ~0x100;
    SceneActorReaction_ResetResponseState((u8 *)a + 0x460, (const void *)gUnk_0824E114, 0x682);
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = 0x10;
    a->target = SceneActorControl_OnIdle;
    a->field2C8 = 0;
    a->flags114 &= ~1;
    a->word104 = 0;
    a->state2A8 = 3;
    a->phase2A9 = 1;
    a->behavior2AA = 0;
    a->counter2AC = 0;
    a->field2B1 = 1;
    a->flags2D4 &= ~0x400;
    *(u8 *)((u8 *)a + 0x5DA) = 0xFF;
    SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)a, 0);
    *(u16 *)((u8 *)a + 0x428) = 0xB4;
    SceneActorAttachment_ReplaceResponseSlot(a, 0, 1, a->category);
    a->flags2D6 |= 0x100;
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0807F81C);
#endif
