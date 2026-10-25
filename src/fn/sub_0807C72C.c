// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reset.h"
#include "scene_actor_attachment.h"
#include "actor_attachment_manager.h"

#ifdef NONMATCHING
// fuzzy: 84.4%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
/* Remaining differences are register lifetimes and inline mask/address selection. */
s32 ActorAttachment_IsUnavailable(struct ActorAttachmentSlotView *);
void *ActorAttachment_CreatePartsSlotWithCount(u32, void *, u32, s32, u32, s16 *, s32, u32, void *,
                                               void *);
void SceneStimulus_AddTagged(s32, s32, s32, u32, u32, u16);
static inline u32 TestWord(u32 *p, s16 mask)
{
    return (*p) & mask;
}

static inline u32 TestHalf(u16 *p, u32 mask)
{
    return *p & mask;
}
inline static u32 TestHalf_1(struct SceneActorRegistryResetView *actor_, u32 mask)
{
    u16 *p = &actor_->flags2D4;
    return (*p) & mask;
}

inline static u32 TestWord_1(struct SceneActorRegistryResetView *actor_, s16 mask)
{
    u32 *p = &actor_->flags2D0;
    return (*p) & mask;
}

void SceneActorAttachment_ReplaceResponseSlot(struct SceneActorRegistryResetView *actor,
                                              u32 inputkind, u32 inputevent, u32 inputcategory)
{
    register s32 kind asm("r5") = (u8)inputkind;
    u8 event = inputevent;
    u8 category = inputcategory;
    void *group = &actor->group;
    struct ActorAttachmentFollowView *follow = (void *)(&actor->id);
    s32 y;
    s32 z;
    typeof(*(((u8 *)actor) + 0x1E6)) t1;
    if (event == 127) {
        switch (kind) {
            case 0:
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 0, (void *)actor->id);
                break;

            case 1:
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 1, (void *)actor->id);
                break;

            case 3:
                actor->attachment =
                    ActorAttachment_ReleasePartsResource(actor->attachment, 3, (void *)actor->id);
                break;

            case 2:
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 2, (void *)actor->id);
                break;
        }

    } else {
        switch (kind) {
            t1 = *(((u8 *)actor) + 0x1E6);
            case 0:
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 0, (void *)actor->id);
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 1, (void *)actor->id);
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 2, (void *)actor->id);
                SceneActorAttachment_SelectOffsets(category, 0, &y, &z);
                if ((event == 0) && TestWord_1(actor, 0x8000))
                    event = 3;
                *((void **)(((u8 *)group) + 0xA0)) = ActorAttachment_CreateSpriteSlot(
                    event, &follow->anchor, (void *)follow->identity, y, z, category, actor,
                    follow);
                SceneActorEffect_PlayCategorySoundIfEnabled(kind, event, category, t1);
                if ((event == 0) || (event == 3))
                    SceneStimulus_AddTagged(follow->anchor.x, follow->anchor.z,
                                            *((s16 *)(((u8 *)group) + 0x4C)), 0, 1,
                                            (u16)follow->identity);
                break;

            case 1:
                if (TestHalf_1(actor, 2))
                    break;
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 0, (void *)actor->id);
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 1, (void *)actor->id);
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 2, (void *)actor->id);
                if ((u8)ActorAttachment_IsUnavailable(actor->attachment)) {
                    s16 *value;
                    s32 initial;
                    u32 flag;
                    SceneActorAttachment_SelectOffsets(category, 1, &y, &z);
                    value = (s16 *)(&actor->fieldAA);
                    initial = actor->fieldA8;
                    flag = (actor->flags2D0 & 0x200) != 0;
                    actor->attachment = ActorAttachment_CreateValueSpriteSlot(
                        event, &actor->position, (void *)actor->id, y, z, category, value, initial,
                        flag, actor, follow);
                    SceneActorEffect_PlayCategorySoundIfEnabled(1, event, category,
                                                                *(((u8 *)actor) + 0x1E6));
                }
                break;

            case 2:
                SceneActorAttachment_SelectOffsets(category, 2, &y, &z);
                actor->attachment = ActorAttachment_CreateSinglePartSlot(
                    1, &actor->position, (void *)actor->id, z, category, follow);
                break;

            case 3:
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 0, (void *)actor->id);
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 1, (void *)actor->id);
                actor->attachment =
                    ActorAttachment_ReleaseResource(actor->attachment, 2, (void *)actor->id);
                actor->attachment =
                    ActorAttachment_ReleasePartsResource(actor->attachment, 3, (void *)actor->id);
                SceneActorAttachment_SelectOffsets(category, 3, &y, &z);
                {
                    u32 count = event;
                    s16 *value;
                    s32 initial;
                    if (!count)
                        count = 3;
                    value = (s16 *)(&actor->fieldAA);
                    initial = actor->fieldA8;
                    actor->attachment = ActorAttachment_CreatePartsSlotWithCount(
                        7, &actor->position, actor->id, z, category, value, initial, count, follow,
                        actor);
                }
                break;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", SceneActorAttachment_ReplaceResponseSlot);
#endif
